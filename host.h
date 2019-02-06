#include <Arduino.h>
#include <stdint.h>

#define SCREEN_WIDTH        21
#define SCREEN_HEIGHT       8

#define EXTERNAL_EEPROM         0
#define EXTERNAL_EEPROM_ADDR    0x50    // I2C address (7 bits)
#define EXTERNAL_EEPROM_SIZE    32768   // only <=32k tested (64k might work?)

#define MAGIC_AUTORUN_NUMBER    0xFC

#define MAXTEXTLEN          128  // 1行の最大文字数
#if !defined (__STM32F1__)
#define WiringPinMode  int
#endif

void host_init(int buzzerPin=0);
void host_sleep(long ms);
void host_digitalWrite(int pin,int state);
int host_digitalRead(int pin);
int host_analogRead(int pin);
void host_pinMode(int pin, WiringPinMode mode);

#if !defined (__STM32F1__)
void host_click();
void host_startupTone();
#endif

void host_cls();
void host_showBuffer();
void host_moveCursor(int x, int y);
void host_outputString(char *str);
void host_outputProgMemString(const char *str);
void host_outputChar(char c);
void host_outputFloat(float f);
char *host_floatToStr(float f, char *buf);
int host_outputInt(long val);
void host_newLine();
char *host_readLine();
char host_getKey();
bool host_ESCPressed();
void host_outputFreeMem(unsigned int val);

#if !defined (__STM32F1__)
void host_saveProgram(bool autoexec);
void host_loadProgram();
#endif

#if EXTERNAL_EEPROM
#include <I2cMaster.h>
void writeExtEEPROM(unsigned int address, byte data);
void host_directoryExtEEPROM();
bool host_saveExtEEPROM(char *fileName);
bool host_loadExtEEPROM(char *fileName);
bool host_removeExtEEPROM(char *fileName);
#endif
