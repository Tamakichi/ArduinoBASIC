#include "host.h"
#include "basic.h"

#if EXTERNAL_EEPROM
#include <Wire.h>
#endif

//#include <SSD1306ASCII.h>
//#include <PS2Keyboard.h>
#if !defined (__STM32F1__)
#include <EEPROM.h>
#endif

//extern SSD1306ASCII oled;
//extern PS2Keyboard keyboard;
#if !defined (__STM32F1__)
extern EEPROMClass EEPROM;
#endif
//int timer1_counter;

// シリアルコンソール 制御キーコード
#define PS2_DELETE  0x7f
#define PS2_BS      0x08
#define PS2_ENTER   0x0d
#define PS2_ESC     0x1b
#define PS2_CTRL_C  0x03
#define VT100_CLS   "\x1b[2J"

// 文字入出力
#define c_getch()     Serial.read()
#define c_kbhit()     Serial.available()
#define c_putc(c)     Serial.write(c)
#define c_newLine(c)  Serial.println()
#define c_cls()       Serial.write(VT100_CLS)

char lineBuffer[MAXTEXTLEN];    // ラインバッファ

//char screenBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
//char lineDirty[SCREEN_HEIGHT];
//int curX = 0, curY = 0;

int curX = 0;
volatile char flash = 1, redraw = 1;
char inputMode = 0;
char inkeyChar = 0;
#if !defined (__STM32F1__)
char buzPin = 0;
#endif

const char bytesFreeStr[] PROGMEM = "bytes free";

/*
void initTimer() {
    noInterrupts();           // disable all interrupts
    TCCR1A = 0;
    TCCR1B = 0;
    timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
    TCNT1 = timer1_counter;   // preload timer
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
    interrupts();             // enable all interrupts
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
    TCNT1 = timer1_counter;   // preload timer
    flash = !flash;
    redraw = 1;
}
*/

// ホスト画面の初期化
void host_init(int buzzerPin) {

#if EXTERNAL_EEPROM
  Wire.begin();
#endif

  //  oled.clear();
#if !defined (__STM32F1__)
    buzPin = buzzerPin;
    if (buzPin)
        pinMode(buzPin, OUTPUT);
#endif
//  initTimer();
}

// スリープ
void host_sleep(long ms) {
  delay(ms);
}

// デジタル出力
void host_digitalWrite(int pin,int state) {
  digitalWrite(pin, state ? HIGH : LOW);
}

// デジタル入力
int host_digitalRead(int pin) {
  return digitalRead(pin);
}

// アナログ入力
int host_analogRead(int pin) {
  return analogRead(pin);
}

// ピンモードの設定
void host_pinMode(int pin, WiringPinMode mode) {
  pinMode(pin, mode);
}

#if !defined (__STM32F1__)
void host_click() {
    if (!buzPin) return;
    digitalWrite(buzPin, HIGH);
    delay(1);
    digitalWrite(buzPin, LOW);
}

void host_startupTone() {
    if (!buzPin) return;
    for (int i=1; i<=2; i++) {
        for (int j=0; j<50*i; j++) {
            digitalWrite(buzPin, HIGH);
            delay(3-i);
            digitalWrite(buzPin, LOW);
            delay(3-i);
        }
        delay(100);
    }    
}
#endif

// スクリーンクリア
void host_cls() {
//  memset(screenBuffer, 32, SCREEN_WIDTH*SCREEN_HEIGHT);
//  memset(lineDirty, 1, SCREEN_HEIGHT);  
    memset(lineBuffer, 32, MAXTEXTLEN);  // ラインバッファのクリア
    curX = 0;                            // カーソル初期化
    c_cls();
    host_moveCursor(0,0);
//  curY = 0;
}

// カーソル移動
void host_moveCursor(int x, int y) {
/*
    if (x<0) x = 0;
    if (x>=SCREEN_WIDTH) x = SCREEN_WIDTH-1;
    if (y<0) y = 0;
    if (y>=SCREEN_HEIGHT) y = SCREEN_HEIGHT-1;
    curX = x;
    curY = y; 
*/
  Serial.write(0x1b);Serial.write('[');
  Serial.print(y+1);Serial.write(';');
  Serial.print(x+1);Serial.write('H');
}

/*
void host_showBuffer() {
    for (int y=0; y<SCREEN_HEIGHT; y++) {
        if (lineDirty[y] || (inputMode && y==curY)) {
            oled.setCursor(0,y);
            for (int x=0; x<SCREEN_WIDTH; x++) {
                char c = screenBuffer[y*SCREEN_WIDTH+x];
                if (c<32) c = ' ';
                if (x==curX && y==curY && inputMode && flash) c = 127;
                oled.print(c);
            }
            lineDirty[y] = 0;
        }
    }
}
*/

// 画面バッファに文字列を出力
void host_outputString(char *str) {
/*
  int pos = curX;
  while (*str) {
    if (pos < MAXTEXTLEN-1) {
      lineBuffer[pos++] = *str;
      c_putc(*str);
    }
    str++;
  }
  curX = pos;
*/
  while (*str) {
    host_outputChar(*str);
    str++;
  }
}

// フラシュメモリ上の文字列を出力
void host_outputProgMemString(const char *p) {
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    host_outputChar(c);
  }
}

// 文字を出力
void host_outputChar(char c) {
/*
  int pos = curX;
  if (pos < MAXTEXTLEN-1) {
     lineBuffer[pos++] = c;
     c_putc(c);
  }
  curX = pos;
*/
  c_putc(c);
  curX++;
  if (curX == SCREEN_WIDTH) {
    host_newLine();
  }    
}

// 数値を出力
int host_outputInt(long num) {
  // returns len
  long i = num, xx = 1;
  int c = 0;
  do {
    c++;
    xx *= 10;
    i /= 10;
  } 
  while (i);

  for (int i=0; i<c; i++) {
    xx /= 10;
    char digit = ((num/xx) % 10) + '0';
    host_outputChar(digit);
  }
  return c;
}

// フロート型を文字列変換
char *host_floatToStr(float f, char *buf) {
  // floats have approx 7 sig figs
  float a = fabs(f);
  if (f == 0.0f) {
    buf[0] = '0'; 
    buf[1] = 0;
  } else if (a<0.0001 || a>1000000) {
    // this will output -1.123456E99 = 13 characters max including trailing nul
#if defined(__AVR__) 
   dtostre(f, buf, 6, 0);
#else
   sprintf(buf, "%6.2e", f);
#endif
  } else {
    int decPos = 7 - (int)(floor(log10(a))+1.0f);
    dtostrf(f, 1, decPos, buf);
    if (decPos) {
      // remove trailing 0s
      char *p = buf;
      while (*p) p++;
      p--;
      while (*p == '0') {
          *p-- = 0;
      }
      if (*p == '.') *p = 0;
    }   
  }
  return buf;
}

// フロート型数値出力
void host_outputFloat(float f) {
  char buf[16];
  host_outputString(host_floatToStr(f, buf));
}

// 改行
void host_newLine() {
  curX = 0;
  //memset(lineBuffer, 32, MAXTEXTLEN);
  c_newLine(c);
}

// ライン入力
char *host_readLine() {
  inputMode = 1;

  if (curX == 0) {
    // 行先頭なら、その行のバッファクリア
    memset(lineBuffer, 32, MAXTEXTLEN);
  } else {
    // そうでないなら、改行して次の行から入力
    host_newLine();
  }
  
  int startPos = curX; // バッファ書き込み先頭位置
  int pos = startPos;  // バッファ書き込み位置

  bool done = false;
  while (!done) {
    while (c_kbhit()) {
      // read the next key
      char c = c_getch();
      if (c>=32 && c<=126) {
        // 通常の文字の場合
        lineBuffer[pos++] = c;             // 画面バッファに書き込み
        c_putc(c);
      } else if ( (c==PS2_DELETE || c==PS2_BS) && pos > startPos) {
        // [DELETE]/[BS]の場合、１文字削除
        lineBuffer[--pos] = 0;
        c_putc(PS2_BS);c_putc(' ');c_putc(PS2_BS); //文字を消す
      } else if (c==PS2_ENTER) {
        // [ENTER]の場合、入力確定
        done = true;        
      }
      // カーソル更新
      curX = pos;
    }
  }
  
  lineBuffer[pos] = 0;         // 文字列終端設定
  inputMode = 0;
  return &lineBuffer[startPos]; // 入力文字列を返す
}

char host_getKey() {
  char c = inkeyChar;
  inkeyChar = 0;
  if (c >= 32 && c <= 126)
      return c;
  else return 0;
}

// 中断キー入力チェック
bool host_ESCPressed() {
  while (c_kbhit()) {
    inkeyChar = c_getch();
    if ( (inkeyChar == PS2_ESC) || (inkeyChar == PS2_CTRL_C))
      return true;
  }
  return false;
}

// 空き領域の表示
void host_outputFreeMem(unsigned int val) {
  host_newLine();
  host_outputInt(val);
  host_outputChar(' ');
  host_outputProgMemString(bytesFreeStr);      
}

#if !defined (__STM32F1__)
void host_saveProgram(bool autoexec) {
    EEPROM.write(0, autoexec ? MAGIC_AUTORUN_NUMBER : 0x00);
    EEPROM.write(1, sysPROGEND & 0xFF);
    EEPROM.write(2, (sysPROGEND >> 8) & 0xFF);
    for (int i=0; i<sysPROGEND; i++)
        EEPROM.write(3+i, mem[i]);
}

void host_loadProgram() {
    // skip the autorun byte
    sysPROGEND = EEPROM.read(1) | (EEPROM.read(2) << 8);
    for (int i=0; i<sysPROGEND; i++)
        mem[i] = EEPROM.read(i+3);
}
#endif

#if EXTERNAL_EEPROM

void writeExtEEPROM(unsigned int address, byte data)
{
  if (address % 32 == 0) host_click();
  uint8_t   i2caddr = (uint8_t)EXTERNAL_EEPROM_ADDR | (uint8_t)(address >> 16);
  Wire.beginTransmission(i2caddr);
  Wire.write((byte)(address >> 8));   // MSB
  Wire.write((byte)(address & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(5);
}

byte readExtEEPROM(unsigned int address)
{
  uint8_t   i2caddr = (uint8_t)EXTERNAL_EEPROM_ADDR | (uint8_t)(address >> 16);
  Wire.beginTransmission(i2caddr);
  Wire.write((byte)(address >> 8));   // MSB
  Wire.write((byte)(address & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(i2caddr, (uint8_t)1);
  byte b = Wire.read();
  return b;
}

// get the EEPROM address of a file, or the end if fileName is null
unsigned int getExtEEPROMAddr(char *fileName) {
  unsigned int addr = 0;
  while (1) {
    uint16_t len = readExtEEPROM(addr) | (readExtEEPROM(addr + 1) << 8);
    if (len == 0) break;
    if (fileName) {
      bool found = true;
      for (int i = 0; i <= strlen(fileName); i++) {
        if (fileName[i] != readExtEEPROM(addr + 2 + i)) {
          found = false;
          break;
        }
      }
      if (found) return addr;
    }
    addr += len;
  }
  return fileName ? EXTERNAL_EEPROM_SIZE : addr;
}

void host_directoryExtEEPROM() {
  unsigned int addr = 0;
  while (1) {
    unsigned int len = readExtEEPROM(addr) | (readExtEEPROM(addr + 1) << 8);
    if (len == 0) break;
    int i = 0;
    while (1) {
      char ch = readExtEEPROM(addr + 2 + i);
      if (!ch) break;
        host_outputChar(readExtEEPROM(addr + 2 + i));
      i++;
    }
    addr += len;
    host_outputChar(' ');
  }
  host_outputFreeMem(EXTERNAL_EEPROM_SIZE - addr - 2);
}

bool host_removeExtEEPROM(char *fileName) {
  unsigned int addr = getExtEEPROMAddr(fileName);
  if (addr == EXTERNAL_EEPROM_SIZE) return false;
  unsigned int len = readExtEEPROM(addr) | (readExtEEPROM(addr + 1) << 8);
  unsigned int last = getExtEEPROMAddr(NULL);
  unsigned int count = 2 + last - (addr + len);
  while (count--) {
    byte b = readExtEEPROM(addr + len);
    writeExtEEPROM(addr, b);
    addr++;
  }
  return true;
}

bool host_loadExtEEPROM(char *fileName) {
  unsigned int addr = getExtEEPROMAddr(fileName);
  if (addr == EXTERNAL_EEPROM_SIZE) return false;

  // skip filename
  addr += 2;
  while (readExtEEPROM(addr++)) ;
  sysPROGEND = readExtEEPROM(addr) | (readExtEEPROM(addr + 1) << 8);
  for (uint16_t i = 0; i < sysPROGEND; i++) {
    mem[i] = readExtEEPROM(addr + 2 + i);
  }
  return true;
}

bool host_saveExtEEPROM(char *fileName) {
  unsigned int addr = getExtEEPROMAddr(fileName);
  if (addr != EXTERNAL_EEPROM_SIZE)
    host_removeExtEEPROM(fileName);
  addr = getExtEEPROMAddr(NULL);
  uint8_t fileNameLen = strlen(fileName);
  uint16_t len = 2 + fileNameLen + 1 + 2 + sysPROGEND;
  if ((uint16_t)EXTERNAL_EEPROM_SIZE - addr - len - 2 < 0)
    return false;

  // write overall length
  writeExtEEPROM(addr++, len & 0xFF);
  writeExtEEPROM(addr++, (len >> 8) & 0xFF);

  // write filename
  for (uint8_t i = 0; i < strlen(fileName); i++)
    writeExtEEPROM(addr++, fileName[i]);
  writeExtEEPROM(addr++, 0);

  // write length & program
  writeExtEEPROM(addr++, sysPROGEND & 0xFF);
  writeExtEEPROM(addr++, (sysPROGEND >> 8) & 0xFF);
  for (int i = 0; i < sysPROGEND; i++)
    writeExtEEPROM(addr++, mem[i]);

  // 0 length marks end
  writeExtEEPROM(addr++, 0);
  writeExtEEPROM(addr++, 0);
  return true;
}

#endif
