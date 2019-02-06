#include "basic.h"
#include "host.h"

#if !defined (__STM32F1__)
  #include <EEPROM.h>  // AVRの対応
#endif

// BASIC
unsigned char mem[MEMORY_SIZE];
#define TOKEN_BUF_SIZE    64
unsigned char tokenBuf[TOKEN_BUF_SIZE];

const char welcomeStr[] PROGMEM = "Arduino BASIC";
char autorun = 0;

void setup() {
    Serial.begin(115200);

    reset();
    host_init();
    host_cls();
    
    // 起動メッセージ出力
    host_outputProgMemString(welcomeStr);

    // show memory size （メモリサイズ出力）
    host_outputFreeMem(sysVARSTART - sysPROGEND);
    //host_showBuffer();

#if !defined (__STM32F1__)    
    // IF USING EXTERNAL EEPROM
    // The following line 'wipes' the external EEPROM and prepares
    // it for use. Uncomment it, upload the sketch, then comment it back
    // in again and upload again, if you use a new EEPROM.
    // writeExtEEPROM(0,0); writeExtEEPROM(1,0);

    if (EEPROM.read(0) == MAGIC_AUTORUN_NUMBER)
        autorun = 1;
    else
        host_startupTone();
#endif

}

void loop() {
    int ret = ERROR_NONE;
  
    if (!autorun) {
        // 自動実行でない場合の処理
        // get a line from the user
        char *input = host_readLine();

        // special editor commands
        //（特別な編集コマンドの入力チェック）
        if (input[0] == '?' && input[1] == 0) {
            // 空きメモリ容量出力
            host_outputFreeMem(sysVARSTART - sysPROGEND);
            //host_showBuffer();
               return;
        }
        
        // otherwise tokenize
        // （ライン入力をトークンへ変換し、トークンバッファに格納）
        ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE);

    } else {
#if !defined (__STM32F1__)  
        // 自動実行の場合の処理、トークンバッファに"RUN"をセット
        host_loadProgram();
        tokenBuf[0] = TOKEN_RUN; 
        tokenBuf[1] = 0;
        autorun = 0;
#endif
    }
  
    // execute the token buffer
    // (トークンバッファのトークン実行）
    if (ret == ERROR_NONE) {
        host_newLine();
        ret = processInput(tokenBuf);  // インタープリタの処理
    }
  
    if (ret != ERROR_NONE) {
        // 実行エラー発生
        host_newLine();
        if (lineNumber !=0) {
            // 行番号付きの場合、行番号表示
            host_outputInt(lineNumber);
            host_outputChar('-');
        }
        
        // エラーメッセージ出力
        host_outputProgMemString((char *)pgm_read_word(&(errorTable[ret])));
    
    } else {
        host_outputProgMemString((char *)pgm_read_word(&(errorTable[ERROR_NONE])));      
    }
}
