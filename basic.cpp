/* ---------------------------------------------------------------------------
 * Basic Interpreter
 * Robin Edwards 2014
 * ---------------------------------------------------------------------------
 * This BASIC is modelled on Sinclair BASIC for the ZX81 and ZX Spectrum. It
 * should be capable of running most of the examples in the manual for both
 * machines, with the exception of anything machine specific (i.e. graphics,
 * sound & system variables).
 *
 * Notes
 *  - All numbers (except line numbers) are floats internally
 *  - Multiple commands are allowed per line, seperated by :
 *  - LET is optional e.g. LET a = 6: b = 7
 *  - MOD provides the modulo operator which was missing from Sinclair BASIC.
 *     Both numbers are first rounded to ints e.g. 5 mod 2 = 1
 *  - CONT can be used to continue from a STOP. It does not continue from any
 *     other error condition.
 *  - Arrays can be any dimension. There is no single char limit to names.
 *  - Like Sinclair BASIC, DIM a(10) and LET a = 5 refer to different 'a's.
 *     One is a simple variable, the other is an array. There is no ambiguity
 *     since the one being referred to is always clear from the context.
 *  - String arrays differ from Sinclair BASIC. DIM a$(5,5) makes an array
 *     of 25 strings, which can be any length. e.g. LET a$(1,1)="long string"
 *  - functions like LEN, require brackets e.g. LEN(a$)
 *  - String manipulation functions are LEFT$, MID$, RIGHT$
 *  - RND is a nonary operator not a function i.e. RND not RND()
 *  - PRINT AT x,y ... is replaced by POSITION x,y : PRINT ...
 *  - LIST takes an optional start and end e.g. LIST 1,100 or LIST 50
 *  - INKEY$ reads the last key pressed from the keyboard, or an empty string
 *     if no key pressed. The (single key) buffer is emptied after the call.
 *     e.g. a$ = INKEY$
 *  - LOAD/SAVE load and save the current program to the EEPROM (1k limit).
 *     SAVE+ will set the auto-run flag, which loads the program automatically
 *     on boot. With a filename e.g. SAVE "test" saves to an external EEPROM.
 *  - DIR/DELETE "filename" - list and remove files from external EEPROM.
 *  - PINMODE <pin>, <mode> - sets the pin mode (0=input, 1=output, 2=pullup)
 *  - PIN <pin>, <state> - sets the pin high (non zero) or low (zero)
 *  - PINREAD(pin) returns pin value, ANALOGRD(pin) for analog pins
 * ---------------------------------------------------------------------------
 */

/*
 *  2019/02/03 Modified by Tamakichi 
 */
 
// 日本語訳
/* ---------------------------------------------------------------------------
 * Basic インタープリタ
 * Robin Edwards 2014
 * ---------------------------------------------------------------------------
 * このBASICはZX81とZX Spectrum用のSinclair BASICをモデルにしています。 
 * マシン固有のもの（グラフィック、サウンド、システム変数など）を除いて、
 * 両方のマシンのマニュアルのほとんどの例を実行できるはずです。
 *
 * メモ
 *  - すべての数値(行番号を除く)は内部的にフロート型で管理しています。
 *  - 1行に複数のコマンドが許可されています。
 *  - LETはオプションです。 L = a = 6：b = 7
 *  - MODはSinclair BASICから欠落していた剰余演算子を提供します。
 *    両方の数値は最初に整数に丸められます。 5 mod 2 = 1
 *  - CONTを使ってSTOPから続行することができます。
 *    だたし、エラー状態からの続行は出来ません。
 *  - 配列は任意の次元にすることができます。配列名は１文字という制約はありません。
 *  - Sinclair BASICのように、DIM a(10)とLET a = 5は異なる 'a'を参照します。
 *    前者は単純変数、後者は配列です。言及されているものは文脈から常に明らかなので、
 *    あいまいさはありません。
 *  - 文字列配列はSinclair BASICとは異なります。 DIM a$(5,5)は配列を作る
 *    25文字列のうち、任意の長さにすることができます。例えばLET a $（1,1）= "長い文字列"
 *  - 文字列配列はSinclair BASICとは異なります。 
 *    DIM a$(5,5)は、25個の文字列の配列を作成します。これは任意の長さにすることができます。
 *    例: LET a $（1,1）= "長い文字列"
 *  - 関数(LENなど)は、括弧'()'を必要とします。 例： LEN(a$)
 *  - 文字列操作関数は、LEFT$、MID$、RIGHT$です。
 *  - RNDは関数ではなく単項演算子です。RND()ではなく、RNDの記述で利用します。
 *  - PRINT AT x、y ...はPOSITION x、yに置き換えられます。PRINT ...
 *  - LISTはオプションの開始と終了を取ります。リスト1,100またはリスト50
 *  - INKEY$は、キーボードから最後に押されたキー、または空の文字列を読み取ります。
 *    キーが押されていない場合、シングルキーバッファは呼び出しの後に空になります。
 *    例： a$ = INKEY $
 *  - LOAD、SAVEは現在のプログラムをロードしてEEPROMに保存します（1kバイト制限）。
 *    SAVE+ はプログラムを自動的にロードする自動実行フラグを設定します。
 *    起動時にファイル名でSAVE "test" は外部EEPROMに保存します。
 *  - LOAD、SAVEは現在のプログラムをロードしてEEPROMに保存します（1k制限）。
 *    SAVE+ は起動時にプログラムを自動的にロードするauto-runフラグを設定します。
 *    SAVE "test"はファイル名で外部EEPROMに保存します。
 *  - DIR、DELETE "filename" は外部EEPROMからファイルをリスト、指定ファイルの削除をします。
 *  - PINMODE <pin>、<mode>  - ピンモードを設定します（0 =入力、1 =出力、2 =プルアップ）。
 *  - PIN <pin>、<state>  - ピンをHIGH(ゼロ以外)またはLOW(ゼロ)に設定します
 *  - PINREAD(pin)はpinからの入力、ANALOGRD(pin)はアナログピンの値を返します。
 * ---------------------------------------------------------------------------
 */

// TODO
// ABS, SIN, COS, EXP etc
// DATA, READ, RESTORE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#include "basic.h"
#include "host.h"

#include <avr/pgmspace.h>

int sysPROGEND;
int sysSTACKSTART, sysSTACKEND;
int sysVARSTART, sysVAREND;
int sysGOSUBSTART, sysGOSUBEND;

const char string_0[] PROGMEM = "OK";
const char string_1[] PROGMEM = "Bad number";
const char string_2[] PROGMEM = "Line too long";
const char string_3[] PROGMEM = "Unexpected input";
const char string_4[] PROGMEM = "Unterminated string";
const char string_5[] PROGMEM = "Missing bracket";
const char string_6[] PROGMEM = "Error in expr";
const char string_7[] PROGMEM = "Numeric expr expected";
const char string_8[] PROGMEM = "String expr expected";
const char string_9[] PROGMEM = "Line number too big";
const char string_10[] PROGMEM = "Out of memory";
const char string_11[] PROGMEM = "Div by zero";
const char string_12[] PROGMEM = "Variable not found";
const char string_13[] PROGMEM = "Bad command";
const char string_14[] PROGMEM = "Bad line number";
const char string_15[] PROGMEM = "Break pressed";
const char string_16[] PROGMEM = "NEXT without FOR";
const char string_17[] PROGMEM = "STOP statement";
const char string_18[] PROGMEM = "Missing THEN in IF";
const char string_19[] PROGMEM = "RETURN without GOSUB";
const char string_20[] PROGMEM = "Wrong array dims";
const char string_21[] PROGMEM = "Bad array index";
const char string_22[] PROGMEM = "Bad string index";
const char string_23[] PROGMEM = "Error in VAL input";
const char string_24[] PROGMEM = "Bad parameter";

//PROGMEM const char *errorTable[] = {
const char* const errorTable[] PROGMEM = {
    string_0, string_1, string_2, string_3,
    string_4, string_5, string_6, string_7,
    string_8, string_9, string_10, string_11,
    string_12, string_13, string_14, string_15,
    string_16, string_17, string_18, string_19,
    string_20, string_21, string_22, string_23,
    string_24
};

// Token flags (トークンフォーマット）
// FFTTTRNN[87654321]
// bits 1+2 number of arguments [xxxxxxNN] (※関数用、現時点では、3つの引数までしか対応出来ない)
#define TKN_ARGS_NUM_MASK	0x03
// bit 3 return type (set if string) [xxxxxRxx]
#define TKN_RET_TYPE_STR	0x04
// bits 4-6 argument type (set if string) [xxTTTxxx]
#define TKN_ARG1_TYPE_STR	0x08
#define TKN_ARG2_TYPE_STR	0x10
#define TKN_ARG3_TYPE_STR	0x20

#define TKN_ARG_MASK		  0x38
#define TKN_ARG_SHIFT		  3
// bits 7,8 formatting [FFxxxxxx]
#define TKN_FMT_POST		  0x40
#define TKN_FMT_PRE		    0x80

PROGMEM const TokenTableEntry tokenTable[] = {
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {"(", 0}, {")",0}, {"+",0}, {"-",0},
    {"*",0}, {"/",0}, {"=",0}, {">",0},
    {"<",0}, {"<>",0}, {">=",0}, {"<=",0},
    {":",TKN_FMT_POST}, {";",0}, {",",0}, {"AND",TKN_FMT_PRE|TKN_FMT_POST},
    {"OR",TKN_FMT_PRE|TKN_FMT_POST}, {"NOT",TKN_FMT_POST}, {"PRINT",TKN_FMT_POST}, {"LET",TKN_FMT_POST},
    {"LIST",TKN_FMT_POST}, {"RUN",TKN_FMT_POST}, {"GOTO",TKN_FMT_POST}, {"REM",TKN_FMT_POST},
    {"STOP",TKN_FMT_POST}, {"INPUT",TKN_FMT_POST},  {"CONT",TKN_FMT_POST}, {"IF",TKN_FMT_POST},
    {"THEN",TKN_FMT_PRE|TKN_FMT_POST}, {"LEN",1|TKN_ARG1_TYPE_STR}, {"VAL",1|TKN_ARG1_TYPE_STR}, {"RND",0},
    {"INT",1}, {"STR$", 1|TKN_RET_TYPE_STR}, {"FOR",TKN_FMT_POST}, {"TO",TKN_FMT_PRE|TKN_FMT_POST},
    {"STEP",TKN_FMT_PRE|TKN_FMT_POST}, {"NEXT", TKN_FMT_POST}, {"MOD",TKN_FMT_PRE|TKN_FMT_POST}, {"NEW",TKN_FMT_POST},
    {"GOSUB",TKN_FMT_POST}, {"RETURN",TKN_FMT_POST}, {"DIM", TKN_FMT_POST}, {"LEFT$",2|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR},
    {"RIGHT$",2|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR}, {"MID$",3|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR}, {"CLS",TKN_FMT_POST}, {"PAUSE",TKN_FMT_POST},
    {"POSITION", TKN_FMT_POST},  {"PIN",TKN_FMT_POST}, {"PINMODE", TKN_FMT_POST}, {"INKEY$", 0},
    {"SAVE", TKN_FMT_POST}, {"LOAD", TKN_FMT_POST}, {"PINREAD",1}, {"ANALOGRD",1},
    {"DIR", TKN_FMT_POST}, {"DELETE", TKN_FMT_POST}
};


/* **************************************************************************
 * PROGRAM FUNCTIONS （プログラム領域操作関数）
 * **************************************************************************/
// 1行分トークン出力
// 引数
//  p : トークンへのポインタ
//
void printTokens(unsigned char *p) {
    int modeREM = 0;
    while (*p != TOKEN_EOL) {
        if (*p == TOKEN_IDENT) {              // 識別子(変数名など)
            p++;                              
            while (*p < 0x80)                 // 識別子は、0x7F以下で終端文字は7ビット目に1がセットされている
                host_outputChar(*p++);        // 文字出力
            host_outputChar((*p++)-0x80);     // 最後の文字出力
        } else if (*p == TOKEN_NUMBER) {      // 数値(Float)
            p++;
            host_outputFloat(*(float*)p);
            p+=4;
        } else if (*p == TOKEN_INTEGER) {     // 整数
            p++;
            host_outputInt(*(long*)p);
            p+=4;
        } else if (*p == TOKEN_STRING) {      // 文字列
          p++;
          if (modeREM) {                      //  コメントの場合
              host_outputString((char*)p);
              p+=1 + strlen((char*)p);
          } else {                            // それ以外の文字列は(")を付加
              host_outputChar('\"');
              while (*p) {
                  if (*p == '\"') host_outputChar('\"');
                  host_outputChar(*p++);
              }
              host_outputChar('\"');
              p++;
            }
        } else {                               // それ以外（登録済トークン・キーワード）
            uint8_t fmt = pgm_read_byte_near(&tokenTable[*p].format);
            if (fmt & TKN_FMT_PRE)             // トークンフォーマット：前に空白
                host_outputChar(' ');
            host_outputString((char *)pgm_read_word(&tokenTable[*p].token));
            if (fmt & TKN_FMT_POST)            // トークンフォーマット：後に空白
                host_outputChar(' ');
            if (*p==TOKEN_REM)                 // コメント文
                modeREM = 1;
            p++;
        }
    }
}

// プログラムリスト出力
// 引数
//   first : 出力開始行番号
//   last  : 出力終了行番号
// (メモ)
//   p: [1行のバイト長 2バイト][行番号 2バイト][命令文]
//
void listProg(uint16_t first, uint16_t last) {
    unsigned char *p = &mem[0];              // ポインタに先頭をセット
    while (p < &mem[sysPROGEND]) {
        uint16_t lineNum = *(uint16_t*)(p+2);  // 行番号取得
        if ((!first || lineNum >= first) && (!last || lineNum <= last)) {
            host_outputInt(lineNum);             // 行番号出力
            host_outputChar(' ');                // 空白出力
            printTokens(p+4);                    // 1行分トークン出力
            host_newLine();                      // 改行
        }
        p+= *(uint16_t *)p;                    // ポインタを次の行に移動
    }
}

// 指定行番号のポインタ取得
// 引数
//  targetLineNumber : ポインタを取得したい行番号
// (メモ)
//   p: [1行のバイト長 2バイト][行番号 2バイト][命令文]
//
unsigned char *findProgLine(uint16_t targetLineNumber) {
    unsigned char *p = &mem[0];
    while (p < &mem[sysPROGEND]) {
        uint16_t lineNum = *(uint16_t*)(p+2);
        if (lineNum >= targetLineNumber)
            break;
        p+= *(uint16_t *)p;
    }
    return p;
}

// 指定ポインタの行の削除
// 引数
//  p : 削除対象の行へのポインタ
// (メモ)
//   p: [1行のバイト長 2バイト][行番号 2バイト][命令文]
//
void deleteProgLine(unsigned char *p) {
    uint16_t lineLen = *(uint16_t*)p;            // 行の長さ取得
    sysPROGEND -= lineLen;                       // 終端位置の更新
    memmove(p, p+lineLen, &mem[sysPROGEND] - p); // 行を詰める
}

// 1行登録
// 引数
//   lineNumber   : 行番号
//   tokenPtr     : トークン先頭アドレス
//   tokensLength : トークン長さ
// 戻り値
//   0 登録した 、1 登録しなかった
//
int doProgLine(uint16_t lineNumber, unsigned char* tokenPtr, int tokensLength) {
    // find line of the at or immediately after the number (番号の位置、ない場合は挿入可能位置を検索）
    unsigned char *p = findProgLine(lineNumber);  // 指定行のポインタ取得
    uint16_t foundLine = 0;                       // 該当行番号

    if (p < &mem[sysPROGEND])
        foundLine = *(uint16_t*)(p+2);              // 該当行番号を取得

    // if there's a line matching this one - delete it （既に同じ行がある場合は、一旦削除する）
    if (foundLine == lineNumber)
        deleteProgLine(p);

    // now check to see if this is an empty line, if so don't insert it (空行なら新たに登録しない）
    if (*tokenPtr == TOKEN_EOL)
        return 1;

    // we now need to insert the new line at p (行を挿入）
    int bytesNeeded = 4 + tokensLength;	// length, linenum + tokens
    if (sysPROGEND + bytesNeeded > sysVARSTART)
        return 0; // 領域不足のため登録しない
    // make room if this isn't the last line (登録のためのスペースを確保）
    if (foundLine)
        memmove(p + bytesNeeded, p, &mem[sysPROGEND] - p);
  
    *(uint16_t *)p = bytesNeeded;       // 行のサイズ（バイト数）のセット 
    p += 2;
    *(uint16_t *)p = lineNumber;       // 行番号のセット
    p += 2;
    memcpy(p, tokenPtr, tokensLength); // トークン本体のセット
    sysPROGEND += bytesNeeded;         // プログラム終端位置の更新
    return 1;                          // 登録完了
}

/* **************************************************************************
 * CALCULATOR STACK FUNCTIONS （計算器スタック操作関数）
 * **************************************************************************/

// Calculator stack starts at the start of memory after the program
// and grows towards the end
// contains either floats or null-terminated strings with the length on the end
// (計算器スタックはプログラム領域の終端から前方向に使っていき、
// 浮動小数点または、長さ・null終端子をもつ文字列を格納します)

// 数値をスタックに入れる
// 引数
//   val : 格納する数値
// 戻り値 
//   正常終了 1、異常終了 0
//
int stackPushNum(float val) {
    if (sysSTACKEND + sizeof(float) > sysVARSTART)
        return 0;	// out of memory
    unsigned char *p = &mem[sysSTACKEND];
    *(float *)p = val;
    sysSTACKEND += sizeof(float);
    return 1;
}

// 数値をスタックから取り出す
// 戻り値 
//   取り出した数値
//
float stackPopNum() {
    sysSTACKEND -= sizeof(float);
    unsigned char *p = &mem[sysSTACKEND];
    return *(float *)p;
}

// 文字列をスタックに入れる
// 引数
//   str : 格納する文字列
// 戻り値 
//   正常終了 1、異常終了 0
//
int stackPushStr(char *str) {
    int len = 1 + strlen(str);
    if (sysSTACKEND + len + 2 > sysVARSTART)
        return 0;	// out of memory
    unsigned char *p = &mem[sysSTACKEND];
    strcpy((char*)p, str);
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
    return 1;
}

// スタック内文字列を参照
// 戻り値 
//   文字列へのポインタ
//
char *stackGetStr() {
    // returns string without popping it
    unsigned char *p = &mem[sysSTACKEND];
    int len = *(uint16_t *)(p-2);
    return (char *)(p-len-2);
}

// 文字列をスタックから取り出す
// 戻り値 
//   文字列へのポインタ
//
char *stackPopStr() {
    unsigned char *p = &mem[sysSTACKEND];
    int len = *(uint16_t *)(p-2);
    sysSTACKEND -= (len+2);
    return (char *)&mem[sysSTACKEND];
}

// スタック内の２つの文字列を１つに連結する
void stackAdd2Strs() {
    // equivalent to popping 2 strings, concatenating them and pushing the result
    // (2つの文字列をポップし、それらを連結して結果をプッシュするのと同じです)
    unsigned char *p = &mem[sysSTACKEND];
    int str2len = *(uint16_t *)(p-2);
    sysSTACKEND -= (str2len+2);
    char *str2 = (char*)&mem[sysSTACKEND];
    p = &mem[sysSTACKEND];
    int str1len = *(uint16_t *)(p-2);
    sysSTACKEND -= (str1len+2);
    char *str1 = (char*)&mem[sysSTACKEND];
    p = &mem[sysSTACKEND];

    // shift the second string up (overwriting the null terminator of the first string)
    // (2番目の文字列を上にシフトします（最初の文字列のnullターミネータを上書きします）)
    memmove(str1 + str1len - 1, str2, str2len);

    // write the length and update stackend
    int newLen = str1len + str2len - 1;
    p += newLen;
    *(uint16_t *)p = newLen;
    sysSTACKEND += newLen + 2;
}

// スタック内文字列切り取り(LEFT$,RIGHT$)
// 引数 
//   len  : 操作文字数
//   mode : 0 = LEFT$, 1 = RIGHT$
//
void stackLeftOrRightStr(int len, int mode) {
    // equivalent to popping the current string, doing the operation then pushing it again
    // 現在の文字列をポップして、操作を実行してからもう一度プッシュするのと同じです。
    unsigned char *p = &mem[sysSTACKEND];
    int strlen = *(uint16_t *)(p-2);

    len++; // include trailing null
    if (len > strlen)
        len = strlen;

    if (len == strlen)
        return;	// nothing to do
    sysSTACKEND -= (strlen+2);
    p = &mem[sysSTACKEND];

    if (mode == 0) {
        // truncate the string on the stack
        *(p+len-1) = 0;
    } else {
        // copy the rightmost characters
        memmove(p, p + strlen - len, len);
    }

    // write the length and update stackend
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
}

// スタック内文字列抽出(MID$)
// 引数 
//  start : 切り取り開始位置
//  len   : 切り取り長さ
//
void stackMidStr(int start, int len) {
    // equivalent to popping the current string, doing the operation then pushing it again
    // (現在の文字列をポップして、操作を実行してからもう一度プッシュするのと同じです)
    unsigned char *p = &mem[sysSTACKEND];
    int strlen = *(uint16_t *)(p-2);
    len++; // include trailing null
    if (start > strlen)
        start = strlen;

    start--;	// basic strings start at 1
    if (start + len > strlen)
        len = strlen - start;

    if (len == strlen)
        return;	// nothing to do

    sysSTACKEND -= (strlen+2);
    p = &mem[sysSTACKEND];

    // copy the characters
    memmove(p, p + start, len-1);
    *(p+len-1) = 0;

    // write the length and update stackend
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
}

/* **************************************************************************
 * VARIABLE TABLE FUNCTIONS (変数テーブル操作関数)
 * **************************************************************************/

// Variable table starts at the end of memory and grows towards the start
// (変数テーブルはメモリの末尾から始まり、先頭に向かって大きくなります)
// Simple variable (単純な変数）
// table +--------+-------+-----------------+-----------------+ . . .
//  <--- | len    | type  | name            | value           |
// grows | 2bytes | 1byte | null terminated | float/string    | 
//       +--------+-------+-----------------+-----------------+ . . .
//
// Array (配列）
// +--------+-------+-----------------+----------+-------+ . . .+-------+-------------+. . 
// | len    | type  | name            | num dims | dim1  |      | dimN  | elem(1,..1) |
// | 2bytes | 1byte | null terminated | 2bytes   | 2bytes|      | 2bytes| float       |
// +--------+-------+-----------------+----------+-------+ . . .+-------+-------------+. . 

// variable type byte (変数型の定義)
#define VAR_TYPE_NUM		    0x1   // 数値 
#define VAR_TYPE_FORNEXT	  0x2   // FORNEXT用変数
#define VAR_TYPE_NUM_ARRAY	0x4   // 数値配列 
#define VAR_TYPE_STRING		  0x8   // 文字列
#define VAR_TYPE_STR_ARRAY	0x10  // 文字列配列

// 変数の検索
//  引数
//   searchName ： 変数名
//   searchMask ： 検索マスク
//  戻り値
//   0以外 見つかった(変数テーブルへのポインタ)
//   NULL  該当なし
//
unsigned char *findVariable(char *searchName, int searchMask) {
    unsigned char *p = &mem[sysVARSTART];
    while (p < &mem[sysVAREND]) {
        int type = *(p+2);
        if (type & searchMask) {
            unsigned char *name = p+3;
            if (strcasecmp((char*)name, searchName) == 0)
                return p;
        }
        p+= *(uint16_t *)p;
    }
    return NULL;
}

// 変数の削除
//  引数
//   pos : 変数へのポインタ
//
void deleteVariableAt(unsigned char *pos) {
    int len = *(uint16_t *)pos;
    if (pos == &mem[sysVARSTART]) {
        sysVARSTART += len;
        return;
    }
    memmove(&mem[sysVARSTART] + len, &mem[sysVARSTART], pos - &mem[sysVARSTART]);
    sysVARSTART += len;
}

// todo - consistently return errors rather than 1 or 0?
// (課題 - 1や0ではなく一貫してエラーを返すのべきか？)

// 変数テーブルに数値変数の登録
// 引数
//  name : 変数名(VAR_TYPE_NUM or VAR_TYPE_FORNEXT)
//  val  : 値
// 戻り値
//  0 登録失敗（メモリー不足）、1 正常終了
//
int storeNumVariable(char *name, float val) {
    // these can be modified in place
    int nameLen = strlen(name);
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);

    if (p != NULL)  {	// replace the old value
        // (could either be VAR_TYPE_NUM or VAR_TYPE_FORNEXT)
        p += 3;	// len + type;
        p += nameLen + 1;
        *(float *)p = val;

    } else {	// allocate a new variable
        int bytesNeeded = 3;	// len + flags
        bytesNeeded += nameLen + 1;	// name
        bytesNeeded += sizeof(float);	// val
    
        if (sysVARSTART - bytesNeeded < sysSTACKEND)
          return 0;	// out of memory (メモリ不足）
        sysVARSTART -= bytesNeeded;
    
        unsigned char *p = &mem[sysVARSTART];
        *(uint16_t *)p = bytesNeeded; 
        p += 2;
        *p++ = VAR_TYPE_NUM;
        strcpy((char*)p, name); 
        p += nameLen + 1;
        *(float *)p = val;
    }
    return 1;
}

// ForNext引数の登録
// 引数
//   name     : 変数名
//   start    : 初期値
//   step     : 刻み値
//   end      : 最終値
//   lineNum  : 行番号
//   stmtNum  : ステートメント番号
// 戻り値
//  0 登録失敗（メモリー不足）、1 正常終了
//  
int storeForNextVariable(char *name, float start, float step, float end, uint16_t lineNum, uint16_t stmtNum) {
    int nameLen = strlen(name);
    int bytesNeeded = 3;	// len + flags
    bytesNeeded += nameLen + 1;	// name
    bytesNeeded += 3 * sizeof(float);	// vals
    bytesNeeded += 2 * sizeof(uint16_t);
  
    // unlike simple numeric variables, these are reallocated if they already exist
    // since the existing value might be a simple variable or a for/next variable
    // (単純な数値変数とは異なり、既存の値は単純な変数またはfor/next変数である可能性があるため、
    //  これらはすでに存在する場合は再割り当てされます)
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);
    if (p != NULL) {
        // check there will actually be room for the new value
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND)
            return 0;	// not enough memory (メモリ不足)
        deleteVariableAt(p);
    }
  
    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory  (メモリ不足)
    sysVARSTART -= bytesNeeded;
  
    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = VAR_TYPE_FORNEXT;
    strcpy((char*)p, name); 

    p += nameLen + 1;
    *(float *)p = start; 
    p += sizeof(float);
    *(float *)p = step; 
    p += sizeof(float);
    *(float *)p = end; 
    p += sizeof(float);
    *(uint16_t *)p = lineNum; 
    p += sizeof(uint16_t);
    *(uint16_t *)p = stmtNum;

    return 1;
}

// 変数テーブルに文字列変数の登録
// 引数
//  name : 変数名(VAR_TYPE_STRING)
//  val  : 値
// 戻り値
//  0 登録失敗（メモリー不足）、1 正常終了
//
int storeStrVariable(char *name, char *val) {
    int nameLen = strlen(name);
    int valLen = strlen(val);
    int bytesNeeded = 3;	// len + type
    bytesNeeded += nameLen + 1;	// name
    bytesNeeded += valLen + 1;	// val
  
    // strings and arrays are re-allocated if they already exist
    // (文字列と配列は、すでに存在する場合は再割り当てされます)
    unsigned char *p = findVariable(name, VAR_TYPE_STRING);

    if (p != NULL) {
      // check there will actually be room for the new value
      uint16_t oldVarLen = *(uint16_t*)p;
      if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND)
          return 0;	// not enough memory (十分なメモリが無い）
      deleteVariableAt(p);
    }
  
    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory (メモリ不足）
    sysVARSTART -= bytesNeeded;
  
    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = VAR_TYPE_STRING;
    strcpy((char*)p, name); 
    p += nameLen + 1;
    strcpy((char*)p, val);

    return 1;
}

// 配列の作成
//  - 計算器スタックに積まれた配列情報を取り出して、配列を作成する
//    var(n,m, .. , z) => 計算器スタックにn,m,... , z が積まれている
//  引数
//   name     : 配列名
//   isString : 文字列配列指定
// 戻り値
//  0 登録失敗（メモリー不足）、1 正常終了
// 
int createArray(char *name, int isString) {

    // dimensions and number of dimensions on the calculator stack
    // (計算器スタックに積まれた次元と次元数の取得)
    int nameLen = strlen(name);
    int bytesNeeded = 3;	// len + flags
    bytesNeeded += nameLen + 1;	// name
    bytesNeeded += 2;		// num dims
    int numElements = 1;
    int i = 0;
    int numDims = (int)stackPopNum(); // 次数の取得

    // keep the current stack position, since we'll need to pop these values again
    // (これらの値をもう一度ポップする必要があるので、現在のスタック位置を維持します)
    int oldSTACKEND = sysSTACKEND;	

    for (int i=0; i<numDims; i++) {
        int dim = (int)stackPopNum();  // 計算器スタックに積まれた添え字の取得
        numElements *= dim;            // 配列要素数の計算
    }

    bytesNeeded += 2 * numDims + (isString ? 1 : sizeof(float)) * numElements;
    // strings and arrays are re-allocated if they already exist
    // (文字列と配列は、すでに存在する場合は再割り当てされます)
    unsigned char *p = findVariable(name, (isString ? VAR_TYPE_STR_ARRAY : VAR_TYPE_NUM_ARRAY));

    if (p != NULL) {
        // check there will actually be room for the new value
        // (実際に新しい価値の余地があることを確認してください)
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND)
            return 0;	// not enough memory (メモリが十分でない）
        deleteVariableAt(p);
    }
  
    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory (メモリ不足）
    sysVARSTART -= bytesNeeded;
  
    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = (isString ? VAR_TYPE_STR_ARRAY : VAR_TYPE_NUM_ARRAY);
    strcpy((char*)p, name); 
    p += nameLen + 1;
    *(uint16_t *)p = numDims; 
    p += 2;
    sysSTACKEND = oldSTACKEND;

    for (int i=0; i<numDims; i++) { // 計算器スタックから添え字を取り出し、変数テーブルに格納
        int dim = (int)stackPopNum();
        *(uint16_t *)p = dim; 
        p += 2;
    }
    memset(p, 0, numElements * (isString ? 1 : sizeof(float)));

    return 1;
}

// 配列内要素の格納オ位置（オフセット）の取得
//  - 計算器スタックに積まれた添え字を取り出し、該当配列変数の値格納位置を取得する 
//    var(a,b,c,d,...z) の a,b,c, ... ,zの添え字がスタックに積まれている
//    添え字を取り出し、登録済みの配列変数の値格納位置を取得する
// 引数
//  p       : 配列の次元数格納アドレスへのポインタ(IN/OUT) 
//  pOffset : オフセット(OUT)
// 戻り値
//  0 正常終了、0以外(エラーコード) 異常終了
//
int _getArrayElemOffset(unsigned char **p, int *pOffset) {
    // check for correct dimensionality
    // (正しい次元数をチェックする)
    int numArrayDims = *(uint16_t*)*p;     // 配列の次元数
    *p+=2;
    int numDimsGiven = (int)stackPopNum(); // 計算器スタックから次数取り出し
  
    if (numArrayDims != numDimsGiven)
        return ERROR_WRONG_ARRAY_DIMENSIONS; // 次元数異常
      
    // now lookup the element (要素の検索）
    int offset = 0;
    int base = 1;
    
    for (int i=0; i<numArrayDims; i++) {
        int index = (int)stackPopNum();      // 計算器スタックから添え字取り出し
        int arrayDim = *(uint16_t*)*p;       // 要素位置計算
        *p+=2;
    
        if (index < 1 || index > arrayDim)
            return ERROR_ARRAY_SUBSCRIPT_OUT_RANGE; // 添え字が範囲外

        offset += base * (index-1);
        base *= arrayDim;
    }

    *pOffset = offset;  // 配列内オフセット位置
    return 0;
}

// 数値配列要素への値のセット
//  - 計算器スタックに積まれた添え字を取り出し、該当配列要素に値をセットする
//  引数
//   name  : 配列変数名
//   val   : 値
//  戻り値
//   エラーコード(正常終了 ERROR_NONE、異常終了 それ以外）
//
int setNumArrayElem(char *name, float val) {
    // each index and number of dimensions on the calculator stack
    // (計算器スタック上の各インデックスと次元数)
    unsigned char *p = findVariable(name, VAR_TYPE_NUM_ARRAY);

    if (p == NULL)
        return ERROR_VARIABLE_NOT_FOUND; // 該当配列変数なし
    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset); // 配列内要素位置のオフセット位置取得
    if (ret) 
        return ret; // オフセット取得異常
    
    p += sizeof(float)*offset; // ポインタをオフセット位置に移動
    *(float *)p = val;         // 値のセット

    return ERROR_NONE;
}

// 文字列配列要素への値のセット
//  - 計算器スタックに積まれた添え字・値をを取り出し、該当配列要素に値をセットする
//  引数
//   name  : 配列変数名
//   val   : 値
//  戻り値
//   エラーコード(正常終了 ERROR_NONE、異常終了 それ以外）
//
int setStrArrayElem(char *name) {
    // string is top of the stack
    // each index and number of dimensions on the calculator stack
    // (文字列は計算器スタックにおいて、各添え字・次元数の一番上にあります)
    
    // keep the current stack position, since we can't overwrite the value string
    // (値の文字列を上書きすることはできないので、現在のスタック位置を維持します)
    int oldSTACKEND = sysSTACKEND;

    // how long is the new value? (新しい文字列の長さを調べる）
    char *newValPtr = stackPopStr();
    int newValLen = strlen(newValPtr);
  
    unsigned char *p = findVariable(name, VAR_TYPE_STR_ARRAY);
    unsigned char *p1 = p;	// so we can correct the length when done (完了したら長さを修正できます)

    if (p == NULL)
        return ERROR_VARIABLE_NOT_FOUND; // 変数が見つからない
  
    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset);  // 計算器スタックに積まれた添え字からオフセット位置を取得
    if (ret) 
        return ret; // オフセット位置取得失敗
    
    // find the correct element by skipping over null terminators
    // (NULL終端文字をスキップして正しい要素を見つける)
    int i = 0;

    while (i < offset) {
        if (*p == 0) i++;
        p++;
    }
    int oldValLen = strlen((char*)p);
    int bytesNeeded = newValLen - oldValLen;

    // check if we've got enough room for the new value
    // (新しい値に十分なスペースがあるかどうかを確認します)
    if (sysVARSTART - bytesNeeded < oldSTACKEND)
        return 0;	// out of memory (メモリ不足）

    // correct the length of the variable (変数の長さの修正)
    *(uint16_t*)p1 += bytesNeeded;
    memmove(&mem[sysVARSTART - bytesNeeded], &mem[sysVARSTART], p - &mem[sysVARSTART]);

    // copy in the new value (新しい値をコピーする)
    strcpy((char*)(p - bytesNeeded), newValPtr);
    sysVARSTART -= bytesNeeded;

    return ERROR_NONE;
}

// 数値配列の値取得
//  - 添え字は計算器スタックから取得する
//  引数
//   name(IN)    配列変数名
//   error(OUT)  エラーコード
//  戻り値
//   取得した値
//
float lookupNumArrayElem(char *name, int *error) {
    // each index and number of dimensions on the calculator stack
    unsigned char *p = findVariable(name, VAR_TYPE_NUM_ARRAY);

    if (p == NULL) {
        *error = ERROR_VARIABLE_NOT_FOUND;
        return 0.0f;
    }
    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset); // 計算器スタックに積まれた添え字からオフセット位置を取得

    if (ret) {
        *error = ret;
        return 0.0f;
    }
    p += sizeof(float)*offset;
    return *(float *)p;
}


// 文字列配列の値取得
//  - 添え字は計算器スタックから取得する
//  引数
//   name(IN)    配列変数名
//   error(OUT)  エラーコード
//  戻り値
//   取得した文字列へのポインタ
//
char *lookupStrArrayElem(char *name, int *error) {

    // each index and number of dimensions on the calculator stack
    // (計算器スタックには各添え字と次元数)
    unsigned char *p = findVariable(name, VAR_TYPE_STR_ARRAY);
    if (p == NULL) {
        *error = ERROR_VARIABLE_NOT_FOUND;
        return NULL;
    }
    p += 3 + strlen(name) + 1;
  
    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) {
        *error = ret;
        return NULL;
    }

    // find the correct element by skipping over null terminators
    // (NULL終端文字をスキップして正しい要素を見つける)
    int i = 0;
    while (i < offset) {
        if (*p == 0) i++;
        p++;
    }

    return (char *)p;
}

// 数値変数の値取得
//  引数
//   name(IN)    変数名
//  戻り値
//   取得した値
//
float lookupNumVariable(char *name) {
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);
    if (p == NULL) {
      return FLT_MAX;
    }

    p += 3 + strlen(name) + 1;
    return *(float *)p;
}

// 文字列変数の値取得
//  引数
//   name(IN)    変数名
//  戻り値
//   取得した文字列へのポインタ
//
char *lookupStrVariable(char *name) {
    unsigned char *p = findVariable(name, VAR_TYPE_STRING);
    if (p == NULL) {
        return NULL;
    }
    p += 3 + strlen(name) + 1;
    return (char *)p;
}

// ForNextの引数の取得
//  引数
//   name : 変数名
//  戻り値
//   取得した値
//
ForNextData lookupForNextVariable(char *name) {
    ForNextData ret;
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);

    if (p == NULL)
        ret.val = FLT_MAX;
    else if (*(p+2) != VAR_TYPE_FORNEXT)
        ret.step = FLT_MAX;
    else {
        p += 3 + strlen(name) + 1;
        ret.val = *(float *)p; 
        p += sizeof(float);
        ret.step = *(float *)p; 
        p += sizeof(float);
        ret.end = *(float *)p; 
        p += sizeof(float);
        ret.lineNumber = *(uint16_t *)p; 
        p += sizeof(uint16_t);
        ret.stmtNumber = *(uint16_t *)p;
    }
    return ret;
}

/* **************************************************************************
 * GOSUB STACK (GOSUBスタック操作関数）
 * **************************************************************************/
// gosub stack (if used) is after the variables
// (gosubスタック（使用されている場合）は変数の後にあります)

// gosubスタックにプッシュ
//  引数
//   lineNumber : 行番号
//   stmtNumber : ステートメント番号
// 戻り値
//   1:正常終了、0:異常終了
//
int gosubStackPush(int lineNumber,int stmtNumber) {
    int bytesNeeded = 2 * sizeof(uint16_t);

    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory (メモリ不足)

    // shift the variable table (プログラム領域をシフトし領域確保)
    memmove(&mem[sysVARSTART]-bytesNeeded, &mem[sysVARSTART], sysVAREND-sysVARSTART);
    sysVARSTART -= bytesNeeded;
    sysVAREND -= bytesNeeded;

    // push the return address (戻るアドレスをプッシュする）
    sysGOSUBSTART = sysVAREND;
    uint16_t *p = (uint16_t*)&mem[sysGOSUBSTART];
    *p++ = (uint16_t)lineNumber;
    *p = (uint16_t)stmtNumber;

    return 1;
}

// gosubスタックの値取り出し（ポップ）
//  引数
//   lineNumber : 行番号へのポインタ（OUT)
//   stmtNumber : ステートメント番号のポインタ（OUT)
// 戻り値
//   1:正常終了、0:異常終了（データ無し）
//
int gosubStackPop(int *lineNumber, int *stmtNumber) {
    if (sysGOSUBSTART == sysGOSUBEND)
      return 0;

    uint16_t *p = (uint16_t*)&mem[sysGOSUBSTART];
    *lineNumber = (int)*p++;
    *stmtNumber = (int)*p;
    int bytesFreed = 2 * sizeof(uint16_t);

    // shift the variable table （プログラム領域をシフトし領域削除）
    memmove(&mem[sysVARSTART]+bytesFreed, &mem[sysVARSTART], sysVAREND-sysVARSTART);
    sysVARSTART += bytesFreed;
    sysVAREND += bytesFreed;
    sysGOSUBSTART = sysVAREND;

    return 1;
}

/* **************************************************************************
 * LEXER (字句解析器)
 * **************************************************************************/

static unsigned char *tokenIn, *tokenOut;
static int tokenOutLeft;

// nextToken returns -1 for end of input, 0 for success, +ve number = error code
// (nextTokenは、入力の終わり: -1、成功: 0、エラー: エラーコード(1以上の値) を返す)
// テキストから次のトークンの取り出し
//  - tokenInから字句解析してトークンを取り出し、tokenOutにトークンコードをセットする
// 戻り値
//  0    : 正常終了
// -1    : 終端に達した
// 1以上 : エラーコード
//
int nextToken() {
    // Skip any whitespace. (空白文字のスキップ)
    while (isspace(*tokenIn))
        tokenIn++;

    // check for end of line (行終端のチェック）
    if (*tokenIn == 0) {
        *tokenOut++ = TOKEN_EOL;
        tokenOutLeft--;
        return -1;
    }

    // Number: [0-9.]+ (数値)
    // TODO - handle 1e4 etc (課題 1e4等の記述形式を扱う)
    if (isdigit(*tokenIn) || *tokenIn == '.') {   // Number: [0-9.]+
        int gotDecimal = 0;
        char numStr[MAX_NUMBER_LEN+1];
        int numLen = 0;
    
       do {
            if (numLen == MAX_NUMBER_LEN) 
                return ERROR_LEXER_BAD_NUM;
            if (*tokenIn == '.') {
              if (gotDecimal) 
                  return ERROR_LEXER_BAD_NUM;
              else 
                  gotDecimal = 1;
            }
            numStr[numLen++] = *tokenIn++;
       } while (isdigit(*tokenIn) || *tokenIn == '.');
    
      numStr[numLen] = 0;
      if (tokenOutLeft <= 5) return ERROR_LEXER_TOO_LONG;
      tokenOutLeft -= 5;

      if (!gotDecimal) {
          long val = strtol(numStr, 0, 10);
          if (val == LONG_MAX || val == LONG_MIN)
              gotDecimal = true;
          else {
              *tokenOut++ = TOKEN_INTEGER;
              *(long*)tokenOut = (long)val;
              tokenOut += sizeof(long);
          }
      }

      if (gotDecimal)  {
          *tokenOut++ = TOKEN_NUMBER;
          *(float*)tokenOut = (float)strtod(numStr, 0);
          tokenOut += sizeof(float);
      }
      return 0;
    }

    // identifier: [a-zA-Z][a-zA-Z0-9]*[$] （識別子）
    if (isalpha(*tokenIn)) {
        char identStr[MAX_IDENT_LEN+1];
        int identLen = 0;
        identStr[identLen++] = *tokenIn++; // copy first char

        while (isalnum(*tokenIn) || *tokenIn=='$') {
            if (identLen < MAX_IDENT_LEN)
                identStr[identLen++] = *tokenIn;
            tokenIn++;
        }

        identStr[identLen] = 0; // 終端文字

        // check to see if this is a keyword (キーワードと被るかチェック）
        for (int i = FIRST_IDENT_TOKEN; i <= LAST_IDENT_TOKEN; i++) {
            if (strcasecmp(identStr, (char *)pgm_read_word(&tokenTable[i].token)) == 0) {
                if (tokenOutLeft <= 1) 
                    return ERROR_LEXER_TOO_LONG;

                tokenOutLeft--;
                *tokenOut++ = i;

                // special case for REM
                if (i == TOKEN_REM) {
                    *tokenOut++ = TOKEN_STRING;

                    // skip whitespace
                    while (isspace(*tokenIn))
                        tokenIn++;

                    // copy the comment
                    while (*tokenIn) {
                        *tokenOut++ = *tokenIn++;
                    }
                    *tokenOut++ = 0;
                }
                return 0;
            }
        }
 
        // no matching keyword - this must be an identifier
        // (キーワードと一致しないため、識別子として扱う)
 
        // $ is only allowed at the end  ($は末尾にのみ使用可能)
        char *dollarPos = strchr(identStr, '$');
        if (dollarPos && dollarPos!= &identStr[0] + identLen - 1) 
            return ERROR_LEXER_UNEXPECTED_INPUT;

        if (tokenOutLeft <= 1+identLen) 
            return ERROR_LEXER_TOO_LONG;

        tokenOutLeft -= 1+identLen;
        *tokenOut++ = TOKEN_IDENT;
        strcpy((char*)tokenOut, identStr);

        tokenOut[identLen-1] |= 0x80;  // 識別子の終端文字は7ビット目をセットする
        tokenOut += identLen;

        return 0;
    }

    // string (文字列）
    if (*tokenIn=='\"') {
        *tokenOut++ = TOKEN_STRING;
        tokenOutLeft--;
        if (tokenOutLeft <= 1) return ERROR_LEXER_TOO_LONG;
        tokenIn++;

        while (*tokenIn) {
            if (*tokenIn == '\"' && *(tokenIn+1) != '\"')
                break;
            else if (*tokenIn == '\"')
                tokenIn++;
            *tokenOut++ = *tokenIn++;
            tokenOutLeft--;
            if (tokenOutLeft <= 1) 
                return ERROR_LEXER_TOO_LONG;
        }
        if (!*tokenIn) 
            return ERROR_LEXER_UNTERMINATED_STRING;

        tokenIn++;
        *tokenOut++ = 0;
        tokenOutLeft--;
        return 0;
    }

    // handle non-alpha tokens e.g. = (アルファベット以外のトークン：例 "=")
    for (int i=LAST_NON_ALPHA_TOKEN; i>=FIRST_NON_ALPHA_TOKEN; i--) {

        // do this "backwards" so we match >= correctly, not as > then =
        // (">"や"="としてではなく、">="と正しく一致するように "後方から"処理する)
        int len = strlen((char *)pgm_read_word(&tokenTable[i].token));

        if (strncmp((char *)pgm_read_word(&tokenTable[i].token), (char*)tokenIn, len) == 0) {
            if (tokenOutLeft <= 1) 
                return ERROR_LEXER_TOO_LONG;

            *tokenOut++ = i;
            tokenOutLeft--;
            tokenIn += len;

            return 0;
        }
    }
    return ERROR_LEXER_UNEXPECTED_INPUT;
}

// 中間コードトークンへの変換
//  引数
//   input      : トークン取り出し文字列
//   output     : トークン格納アドレス
//   outputSize : トークン格納領域サイズ
//  戻り値
//   0     : 正常終了
//   0以外（=エラーコード) : 異常終了
//
int tokenize(unsigned char *input, unsigned char *output, int outputSize) {
    tokenIn = input;
    tokenOut = output;
    tokenOutLeft = outputSize;
    int ret;

    while (1) {
        ret = nextToken();
        if (ret) 
          break;
    }

    return (ret > 0) ? ret : 0;
}

/* **************************************************************************
 * PARSER / INTERPRETER （構文解析器・インタープリタ）
 * **************************************************************************/

static char executeMode;	// 0 = syntax check only, 1 = execute
uint16_t lineNumber, stmtNumber;

// stmt number is 0 for the first statement, then increases after each command seperator (:)
// (stmtNumberは、最初のステートメント0で、以降 コマンド区切り文字`:` 毎に増加します)
// Note that IF a=1 THEN PRINT "x": print "y" is considered to be only 2 statements
// ( `IF = 1 THEN PRINT "x"：print "y"` は、2つのステートメントのみと見なされることに注意してください)
static uint16_t jumpLineNumber, jumpStmtNumber;
static uint16_t stopLineNumber, stopStmtNumber;
static char breakCurrentLine;

static unsigned char *tokenBuffer, *prevToken; // トークンバッファ
static int curToken;                           // カレントトークンコード
static char identVal[MAX_IDENT_LEN+1];         // 識別子文字列
static char isStrIdent;                        // 文字列トークン識別チェック
static float numVal;                           // 数値トークンの値（数値）
static char *strVal;                           // 文字列トークンの値（文字列）
static long numIntVal;                         // 整数値トークンの値（数値）

// 中間コードトークンバッファからトークン取り出し
//  - 取り出したトークンの値等はグローバル変数に格納
//  戻り値
//   トークンコード
//
int getNextToken() {
    prevToken = tokenBuffer;
    curToken = *tokenBuffer++;               // トークン取り出し

    if (curToken == TOKEN_IDENT) {           // 識別子の場合
        int i=0;

        while (*tokenBuffer < 0x80)
            identVal[i++] = *tokenBuffer++;  // 識別子取り出し

        identVal[i] = (*tokenBuffer++)-0x80;
        isStrIdent = (identVal[i++] == '$');
        identVal[i++] = '\0';

    } else if (curToken == TOKEN_NUMBER) {   // 数値の場合
        numVal = *(float*)tokenBuffer;
        tokenBuffer += sizeof(float);

    } else if (curToken == TOKEN_INTEGER) {  // 整数値の場合
        // these are really just for line numbers
        numVal = (float)(*(long*)tokenBuffer);
        tokenBuffer += sizeof(long);

    } else if (curToken == TOKEN_STRING) {   // 文字列の場合
        strVal = (char*)tokenBuffer;
        tokenBuffer += 1 + strlen(strVal);
    }
    return curToken;
}

// value (int) returned from parseXXXXX
#define ERROR_MASK						0x0FFF
#define TYPE_MASK						  0xF000
#define TYPE_NUMBER						0x0000
#define TYPE_STRING						0x1000

#define IS_TYPE_NUM(x) ((x & TYPE_MASK) == TYPE_NUMBER)
#define IS_TYPE_STR(x) ((x & TYPE_MASK) == TYPE_STRING)

// forward declarations （前方宣言）
int parseExpression();
int parsePrimary();
int expectNumber();

// parse a number （数値を解析する）
//  - 計算器スタックに数値(numVal)を積み、トークンバッファから次のトークンを取り出す
// 戻り値
//  正常終了 トークンコード TYPE_NUMBER (数値）
//  異常終了 エラーコード ERROR_OUT_OF_MEMORY （メモリ不足）
//
int parseNumberExpr() {
    if (executeMode && !stackPushNum(numVal))
        return ERROR_OUT_OF_MEMORY; // メモリ不足

    getNextToken(); // consume the number

    return TYPE_NUMBER;
}

// parse (x1,....xn) e.g. DIM a(10,20)
// 添え字の解析
// - トークンバッファから取り出し、添え字を計算器スタックに積む
// 戻り値
//  正常終了 0
//  異常終了 エラーコード ERROR_EXPR_MISSING_BRACKET or ERROR_OUT_OF_MEMORY
//
int parseSubscriptExpr() {
    // stacks x1, .. xn followed by n
    int numDims = 0;                            // 次数（添え字数）
    if (curToken != TOKEN_LBRACKET)             // '('左括弧 チェック
        return ERROR_EXPR_MISSING_BRACKET;      // 書式エラー  
    getNextToken();                             // 次のトークン取り出し

    while(1) {
        numDims++;
        int val = expectNumber();                 // 数値の解析
        if (val) 
            return val;	// error                  // エラー
        if (curToken == TOKEN_RBRACKET)           // ')' 右括弧検出
            break;
        else if (curToken == TOKEN_COMMA)         // ',' カンマ検出
            getNextToken();
        else
            return ERROR_EXPR_MISSING_BRACKET;    // 書式エラー
  }

  getNextToken(); // eat )                       // 次のトークン取り出し
  if (executeMode && !stackPushNum(numDims))     // 次数（添え字数）をスタックに積む
      return ERROR_OUT_OF_MEMORY;                // メモリ不足

  return 0;
}

// parse a function call e.g. LEN(a$)
// 関数呼び出しの解析
// 戻り値
//  正常終了 関数の戻り値型
//  異常終了 エラーコード
//
int parseFnCallExpr() {
    int op = curToken;                                                 // トークンコード
    int fnSpec = pgm_read_byte_near(&tokenTable[curToken].format);     // 引数形式の取得

    getNextToken();
    // get the required arguments and types from the token table
    if (curToken != TOKEN_LBRACKET)                                    //  '(' のチェック
        return ERROR_EXPR_MISSING_BRACKET;                             //  記述エラー
    getNextToken();                                                    // 次のトークン取り出し

    int reqdArgs = fnSpec & TKN_ARGS_NUM_MASK;                         // 引数の個数
    int argTypes = (fnSpec & TKN_ARG_MASK) >> TKN_ARG_SHIFT;           // 引数の形式
    int ret = (fnSpec & TKN_RET_TYPE_STR) ? TYPE_STRING : TYPE_NUMBER; // 戻り値の型

    // 引数をreqdArgs数分取り出し
    for (int i=0; i<reqdArgs; i++) {
        int val = parseExpression();
        if (val & ERROR_MASK)
            return val;

        // check we've got the right type （正しい型を持っているか確認する）
        if (!(argTypes & 1) && !IS_TYPE_NUM(val))
            return ERROR_EXPR_EXPECTED_NUM;  // エラー 数値型でない

        if ((argTypes & 1) && !IS_TYPE_STR(val))
            return ERROR_EXPR_EXPECTED_STR;  // エラー 文字列型でない

        argTypes >>= 1;
        // if this isn't the last argument, eat the , (最後の引数ではない場合','を取り出す)
        if (i+1<reqdArgs) {
            if (curToken != TOKEN_COMMA)     // ',' のチェック
                return ERROR_UNEXPECTED_TOKEN; // 予期しないトークン
            getNextToken();                  // 次のトークン取り出し
        }
    }
  
    // now all the arguments will be on the stack (last first)
    // (すべての引数はスタック上に置かれているものとします（最後のものが先）)
    if (executeMode) {
        int tmp;
        switch (op) {       // トークンコードによる分岐処理
        case TOKEN_INT:     // INT(number)
            stackPushNum((float)floor(stackPopNum()));
            break;
    
        case TOKEN_STR:     // STR$(number)
          {
            char buf[16];
            if (!stackPushStr(host_floatToStr(stackPopNum(), buf)))
                return ERROR_OUT_OF_MEMORY;
          }
          break;
        
        case TOKEN_LEN:     // LEN(string)
            tmp = strlen(stackPopStr());
            if (!stackPushNum(tmp)) 
                return ERROR_OUT_OF_MEMORY;
            break;

        case TOKEN_VAL:     // VAL(string) :string文字列を式として評価し、数値を得る
            {
              // tokenise str onto the stack(strをスタックにトークン化する)
              int oldStackEnd = sysSTACKEND;
              unsigned char *oldTokenBuffer = prevToken;
              int val = tokenize((unsigned char*)stackGetStr(), &mem[sysSTACKEND], sysVARSTART - sysSTACKEND);
              if (val) {
                  if (val == ERROR_LEXER_TOO_LONG) 
                      return ERROR_OUT_OF_MEMORY;
                  else 
                      return ERROR_IN_VAL_INPUT;
              }
              
              // set tokenBuffer to point to the new set of tokens on the stack
              // (tokenBufferにスタック上の新しいトークンへの参照を設定)
              tokenBuffer = &mem[sysSTACKEND];
  
              // move stack end to the end of the new tokens
              // (スタックの終わりを新しいトークンの終わりに移動する)
              sysSTACKEND = tokenOut - &mem[0];
              getNextToken();
  
              // then parseExpression （式として評価し、数値を得る）
              val = parseExpression();
              if (val & ERROR_MASK) {
                  if (val == ERROR_OUT_OF_MEMORY) 
                      return val;
                  else 
                      return ERROR_IN_VAL_INPUT;
              }
              
              if (!IS_TYPE_NUM(val))
                  return ERROR_EXPR_EXPECTED_NUM;
  
              // read the result from the stack (スタックから結果を読み込む)
              float f = stackPopNum();
  
              // pop the tokens from the stack (スタックからトークンをポップする)
              sysSTACKEND = oldStackEnd;
  
              // and pop the original string (元の文字列をポップする)
              stackPopStr();
  
              // finally, push the result and set the token buffer back
              // (最後に、結果をプッシュしてトークンバッファを元に戻す)
              stackPushNum(f);
              tokenBuffer = oldTokenBuffer;
              getNextToken();
            }
            break;

        case TOKEN_LEFT:     // LEFT$(string,n)
            tmp = (int)stackPopNum();
            if (tmp < 0) 
                return ERROR_STR_SUBSCRIPT_OUT_RANGE;

            stackLeftOrRightStr(tmp, 0);

            break;

        case TOKEN_RIGHT:     // RIGHT$(string,n)
          tmp = (int)stackPopNum();
          if (tmp < 0) 
              return ERROR_STR_SUBSCRIPT_OUT_RANGE;

          stackLeftOrRightStr(tmp, 1);

          break;

        case TOKEN_MID:       // MID$(string,start,n)
          {
            tmp = (int)stackPopNum();
            int start = stackPopNum();
            if (tmp < 0 || start < 1) 
                return ERROR_STR_SUBSCRIPT_OUT_RANGE;

            stackMidStr(start, tmp);
          }

          break;

        case TOKEN_PINREAD:   // PINREAD(pin)
            tmp = (int)stackPopNum();
            if (!stackPushNum(host_digitalRead(tmp))) 
                return ERROR_OUT_OF_MEMORY;

            break;

        case TOKEN_ANALOGRD:  // ANALOGRD(pin)
            tmp = (int)stackPopNum();
            if (!stackPushNum(host_analogRead(tmp))) 
                return ERROR_OUT_OF_MEMORY;

            break;

        default:
            return ERROR_UNEXPECTED_TOKEN;
        }
    }
  
    if (curToken != TOKEN_RBRACKET) return ERROR_EXPR_MISSING_BRACKET;
    getNextToken();	// eat )

    return ret;
}

// parse an identifer e.g. a$ or a(5,3)
// 識別子の解析
//  戻り値
//   正常終了 識別子の型
//   異常終了 エラーコード
//
int parseIdentifierExpr() {
    char ident[MAX_IDENT_LEN+1];

    if (executeMode)
        strcpy(ident, identVal);

    int isStringIdentifier = isStrIdent;
    getNextToken();	// eat ident
    if (curToken == TOKEN_LBRACKET) {
        // array access （配列の操作)
        int val = parseSubscriptExpr();
        if (val)
            return val;

        if (executeMode) {
            if (isStringIdentifier) {
                int error = 0;
                char *str = lookupStrArrayElem(ident, &error);
                if (error)
                    return error;
                else if (!stackPushStr(str))
                    return ERROR_OUT_OF_MEMORY;
            } else {
                int error = 0;
                float f = lookupNumArrayElem(ident, &error);
                if (error)
                    return error;
                else if (!stackPushNum(f)) 
                    return ERROR_OUT_OF_MEMORY;
            }
        }

    } else {
      // simple variable (単純な変数）
      if (executeMode) {
          if (isStringIdentifier) {
              char *str = lookupStrVariable(ident);
              if (!str) 
                  return ERROR_VARIABLE_NOT_FOUND;
              else if (!stackPushStr(str)) 
                  return ERROR_OUT_OF_MEMORY;
          } else {
              float f = lookupNumVariable(ident);
              if (f == FLT_MAX) 
                  return ERROR_VARIABLE_NOT_FOUND;
              else if (!stackPushNum(f)) 
                  return ERROR_OUT_OF_MEMORY;
          }
        }
    }

    return isStringIdentifier ? TYPE_STRING : TYPE_NUMBER;
}

// parse a string e.g. "hello" （文字列の解析）
// - 計算器スタックに文字列を積み、次のトークン取り出し
//  戻り値
//   正常終了 識別子の型 TYPE_STRING
//   異常終了 エラーコード
//
int parseStringExpr() {
    if (executeMode && !stackPushStr(strVal))
        return ERROR_OUT_OF_MEMORY;

    getNextToken(); // consume the string

    return TYPE_STRING;
}

// parse a bracketed expressed e.g. (5+3)
// (括弧式の評価）
//  戻り値
//   正常終了 識別子の型
//   異常終了 エラーコード
//
int parseParenExpr() {
    getNextToken();  // eat (
    int val = parseExpression();            // 式の評価

    if (val & ERROR_MASK) 
        return val;

    if (curToken != TOKEN_RBRACKET)         // 右括弧 ')'のチェック
        return ERROR_EXPR_MISSING_BRACKET;
    getNextToken();  // eat )

    return val;
}

// RND の評価
//  戻り値
//   正常終了 識別子の型  TYPE_NUMBER
//   異常終了 エラーコード
//
int parse_RND() {
    getNextToken();

    if (executeMode && !stackPushNum((float)rand()/(float)RAND_MAX))
        return ERROR_OUT_OF_MEMORY;

    return TYPE_NUMBER;	
}

// INKEY の評価
//  戻り値
//   正常終了 識別子の型  TYPE_STRING
//   異常終了 エラーコード
//
int parse_INKEY() {
    getNextToken();

    if (executeMode) {
        char str[2];
        str[0] = host_getKey();
        str[1] = 0;
        if (!stackPushStr(str))
            return ERROR_OUT_OF_MEMORY;
    }

    return TYPE_STRING;	
}

// 数値(単項)の評価
//  戻り値
//   正常終了 識別子の型  TYPE_NUMBER
//   異常終了 エラーコード
//
int parseUnaryNumExp() {
    int op = curToken;          // 現在のトークン

    getNextToken();             // 次のトークン取り出し
    int val = parsePrimary();   // 式の評価
    if (val & ERROR_MASK) 
        return val;

    if (!IS_TYPE_NUM(val))      // 型の判定
        return ERROR_EXPR_EXPECTED_NUM;

    switch (op) {
    case TOKEN_MINUS:         // "-"
        if (executeMode) stackPushNum(stackPopNum() * -1.0f);
          return TYPE_NUMBER;
    case TOKEN_NOT:           // "NOT"
        if (executeMode) stackPushNum(stackPopNum() ? 0.0f : 1.0f);
          return TYPE_NUMBER;
    default:
        return ERROR_UNEXPECTED_TOKEN;
    }
}

/// primary (式の一次評価）
//  戻り値
//   正常終了 識別子の型
//   異常終了 エラーコード
//
int parsePrimary() {
    switch (curToken) {
    case TOKEN_IDENT:	            // インデント
        return parseIdentifierExpr();
    case TOKEN_NUMBER:            // 数値
    case TOKEN_INTEGER:           // 整数
        return parseNumberExpr(); // 数値
    case TOKEN_STRING:	
        return parseStringExpr(); // 文字列
    case TOKEN_LBRACKET:          
        return parseParenExpr();  // 括弧式

        // "psuedo-identifiers" 疑似識別子
    case TOKEN_RND:	
        return parse_RND();
    case TOKEN_INKEY:
        return parse_INKEY();

        // unary ops (数値の評価)
    case TOKEN_MINUS:
    case TOKEN_NOT:
        return parseUnaryNumExp();

        // functions (関数の評価)
    case TOKEN_INT: 
    case TOKEN_STR: 
    case TOKEN_LEN: 
    case TOKEN_VAL:
    case TOKEN_LEFT: 
    case TOKEN_RIGHT: 
    case TOKEN_MID: 
    case TOKEN_PINREAD:
    case TOKEN_ANALOGRD:
        return parseFnCallExpr();

    default:
        return ERROR_UNEXPECTED_TOKEN;
    }
}

// トークンの優先順位取得
//  戻り値
//   優先度
//
int getTokPrecedence() {
    if (curToken == TOKEN_AND || curToken == TOKEN_OR)              // "AND","OR" 
        return 5;        

    if (curToken == TOKEN_EQUALS || curToken == TOKEN_NOT_EQ)       // "=","<>"    
        return 10;

    if (curToken == TOKEN_LT    || curToken == TOKEN_GT ||          // "<",">","<=",">="
        curToken == TOKEN_LT_EQ || curToken == TOKEN_GT_EQ)     
        return 20;
  
    if (curToken == TOKEN_MINUS || curToken == TOKEN_PLUS)          // "+","-"
        return 30;
    else if (curToken == TOKEN_MULT || curToken == TOKEN_DIV ||      // "*","/","MOD"
             curToken == TOKEN_MOD)       
        return 40;
    else              // それ以外
        return -1;
}

// Operator-Precedence Parsing (優先順位付き演算評価)
// ２項演算子の右項評価
//  引数
//   ExprPrec : 式の優先度
//   lhsVal   : 左項の型
// 戻り値
//   正常終了 左項の型
//   異常終了 エラーコード
//   
int parseBinOpRHS(int ExprPrec, int lhsVal) {
    // If this is a binop, find its precedence.
    // (二項演算子の場合、その優先優先度を見つける)
    while (1) {
        int TokPrec = getTokPrecedence();  // カレントトークン(二項演算子)の優先度を取得
    
        // If this is a binop that binds at least as tightly as the current binop,
        // consume it, otherwise we are done.
        // (これが現在の演算子と少なくとも同じくらい強く結合する演算子であるなら、
        //  それを処理する、そうでなければ終了する)
        if (TokPrec < ExprPrec)
            return lhsVal;
    
        // Okay, we know this is a binop. （さて、これが二項演算子だと知っています）
        int BinOp = curToken;
        getNextToken();  // eat binop
    
        // Parse the primary expression after the binary operator.
        // (二項演算子の右項の式を評価します)
        int rhsVal = parsePrimary();  // 右項の評価
        if (rhsVal & ERROR_MASK)
            return rhsVal;
    
        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        // (演算子が右項の後の演算子よりも右項との結びつきが弱い場合は、
        //  保留中の演算子に右項をその左項として取り込ませます。)
        int NextPrec = getTokPrecedence();  // 右項の後の演算子の優先度の取得
        if (TokPrec < NextPrec) {
            rhsVal = parseBinOpRHS(TokPrec+1, rhsVal);
            if (rhsVal & ERROR_MASK) 
                return rhsVal;
        }
    
        if (IS_TYPE_NUM(lhsVal) && IS_TYPE_NUM(rhsVal)) {	// Number operations （数値演算）
            float r, l;
            if (executeMode) {
                r = stackPopNum();                // 右項
                l = stackPopNum();                // 左項
          }

          if (BinOp == TOKEN_PLUS) {            // "+"
              if (executeMode)
                  stackPushNum(l+r);
          } else if (BinOp == TOKEN_MINUS) {    // "-"
              if (executeMode)
                  stackPushNum(l-r);
          } else if (BinOp == TOKEN_MULT) {     // "*"
              if (executeMode)
                  stackPushNum(l*r);
          } else if (BinOp == TOKEN_DIV) {      // "/"
              if (executeMode) {
                if (r)
                    stackPushNum(l/r);
                else
                    return ERROR_EXPR_DIV_ZERO;
            }
          } else if (BinOp == TOKEN_MOD) {      // "MOD"
              if (executeMode) {
                  if ((int)r) 
                      stackPushNum((float)((int)l % (int)r));
                  else
                      return ERROR_EXPR_DIV_ZERO;
              }
          } else if (BinOp == TOKEN_LT) {       // "<"
              if (executeMode) stackPushNum(l < r ? 1.0f : 0.0f);
          } else if (BinOp == TOKEN_GT) {       // ">"
              if (executeMode) stackPushNum(l > r ? 1.0f : 0.0f);
          } else if (BinOp == TOKEN_EQUALS) {   // "="
              if (executeMode) stackPushNum(l == r ? 1.0f : 0.0f);
          } else if (BinOp == TOKEN_NOT_EQ) {   // "<>"
              if (executeMode) stackPushNum(l != r ? 1.0f : 0.0f);
          } else if (BinOp == TOKEN_LT_EQ) {    // "<="
              if (executeMode) stackPushNum(l <= r ? 1.0f : 0.0f);
          } else if (BinOp == TOKEN_GT_EQ) {    // ">="
              if (executeMode) stackPushNum(l >= r ? 1.0f : 0.0f);
          } else if (BinOp == TOKEN_AND) {      // "AND"
              if (executeMode) stackPushNum(r != 0.0f ? l : 0.0f);
          } else if (BinOp == TOKEN_OR) {       // "OR"
              if (executeMode) stackPushNum(r != 0.0f ? 1 : l);
          } else                                // トークンエラー
              return ERROR_UNEXPECTED_TOKEN;

      } else if (IS_TYPE_STR(lhsVal) && IS_TYPE_STR(rhsVal)) {	// String operations （文字列の演算）
          if (BinOp == TOKEN_PLUS) {            // "+"
              if (executeMode)
                  stackAdd2Strs();
          } else if (BinOp >= TOKEN_EQUALS && BinOp <=TOKEN_LT_EQ) {  // "=" ～ "<="
              if (executeMode) {
                  char *r = stackPopStr();      // 右項
                  char *l = stackPopStr();      // 左項
                  int ret = strcmp(l,r);        // 文字列比較
                  if (BinOp == TOKEN_EQUALS && ret == 0)
                      stackPushNum(1.0f);       // "="
                  else if (BinOp == TOKEN_NOT_EQ && ret != 0)
                      stackPushNum(1.0f);  // "<>"
                  else if (BinOp == TOKEN_GT && ret > 0) 
                      stackPushNum(1.0f);       // ">"
                  else if (BinOp == TOKEN_LT && ret < 0)
                      stackPushNum(1.0f);       // "<"
                  else if (BinOp == TOKEN_GT_EQ && ret >= 0)
                      stackPushNum(1.0f);   // ">="
                  else if (BinOp == TOKEN_LT_EQ && ret <= 0)
                      stackPushNum(1.0f);   // "<="
                  else 
                      stackPushNum(0.0f);
              }
              lhsVal = TYPE_NUMBER;
          } else
              return ERROR_UNEXPECTED_TOKEN;
      } else
            return ERROR_UNEXPECTED_TOKEN;
   }
}

// 式の処理
//  戻り値
//   正常終了 評価した式の型
//   異常終了 エラーコード
//   
int parseExpression() {
  int val = parsePrimary();
  if (val & ERROR_MASK) 
      return val;
  return parseBinOpRHS(0, val);
}

// 数値の評価
//  戻り値
//   正常終了 評価した式の型
//   異常終了 エラーコード
//   
int expectNumber() {
  int val = parseExpression();
  if (val & ERROR_MASK) return val;
  if (!IS_TYPE_NUM(val))
    return ERROR_EXPR_EXPECTED_NUM;

  return 0;
}

// RUN コマンドの処理
//  書式 RUN [lineNumber]
//   正常終了 0
//   異常終了 エラーコード
//   
int parse_RUN() {

    getNextToken();          // トークン取り出し
    uint16_t startLine = 1;  // デフォルトの開始行（=先頭行)

    // 引数のチェック
    if (curToken != TOKEN_EOL) {
        int val = expectNumber(); // 引数はスタックに積まれる
        if (val) // TYPE_NUMBER(=0x0000)でない？
            return val;	// error

        // 実行モードの場合、引数の開始行をセット
        if (executeMode) {
            startLine = (uint16_t)stackPopNum();
            if (startLine <= 0)
                return ERROR_BAD_LINE_NUM;
        }
    }

    // 実行モードの場合、変数を初期化し、実行開始位置をセット
    if (executeMode) {
        // clear variables
        sysVARSTART = sysVAREND = sysGOSUBSTART = sysGOSUBEND = MEMORY_SIZE;
        jumpLineNumber = startLine;
        stopLineNumber = stopStmtNumber = 0;
    }
    return 0;
}

// GOTO コマンドの処理
//  書式 GOTO lineNumber
//   正常終了 0
//   異常終了 エラーコード
//   
int parse_GOTO() {
  
  getNextToken();
  
  // 引数の評価
  int val = expectNumber();  // 行番号がスタックに積まれる
  if (val) 
      return val;	// error

  // 実行モードの場合、ジャンプ位置をセット
  if (executeMode) {
    uint16_t startLine = (uint16_t)stackPopNum(); // 行番号取り出し
    if (startLine <= 0)
        return ERROR_BAD_LINE_NUM;
    jumpLineNumber = startLine;
  }

  return 0;
}

// PAUSE コマンドの処理
//  書式 PAUSE milliseconds
//   正常終了 0
//   異常終了 エラーコード
//   
int parse_PAUSE() {

  getNextToken();

  // 引数の評価
  int val = expectNumber(); // ミリ秒がスタックに積まれる
  if (val)
      return val;	// error

  // 実行モードの場合、時間待ちを行う
  if (executeMode) {
    long ms = (long)stackPopNum(); // スタックから引数取り出し
    if (ms < 0)
      return ERROR_BAD_PARAMETER;
    host_sleep(ms);
  }
  return 0;
}

// LIST コマンドの処理
//  書式 LIST [start][,end]
//   正常終了 0
//   異常終了 エラーコード
//   
int parse_LIST() {
    getNextToken();
    uint16_t first = 0, last = 0;
  
    // 第1引数の処理
    if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = expectNumber();
        if (val)
            return val;	// error
    
        if (executeMode)
            first = (uint16_t)stackPopNum();
    }
  
    // "," がある場合、第2引数の処理を行う
    if (curToken == TOKEN_COMMA) {
        getNextToken();
        int val = expectNumber();
        if (val) 
            return val;	// error
    
        if (executeMode)
            last = (uint16_t)stackPopNum();
    }
  
    // 実行モードの場合、リスト出力  
    if (executeMode) {
        listProg(first,last);
        //host_showBuffer();
    }
  
    return 0;
}

// PRINT コマンドの処理
//  書式 PRINT <expr>;<expr>...
//   正常終了 0
//   異常終了 エラーコード
//  
int parse_PRINT() {
    getNextToken();
    // zero + expressions seperated by semicolons
    // (引数無し、または セミコロンで区切られた式)
    int newLine = 1; // 改行フラグ（1:改行あり、0:改行なし)

    while (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = parseExpression(); // <expr> の評価
        if (val & ERROR_MASK) 
            return val;

        // 実行モードの場合、<expr>を出力する
        if (executeMode) {
            if (IS_TYPE_NUM(val))  // 数値
                host_outputFloat(stackPopNum());
            else                   // 文字列
                host_outputString(stackPopStr());
            newLine = 1;           // 改行あり
        }

        // セミコロンがある場合、改行なしをセット
        if (curToken == TOKEN_SEMICOLON) {
            newLine = 0;
            getNextToken();
        }
    }

    // 実行モードの場合、改行出力の処理を行う
    if (executeMode) {
        if (newLine)
            host_newLine();
          //host_showBuffer();
    }
    return 0;
}

// parse a stmt that takes two int parameters 
// e.g. POSITION 3,2
// (2つの整数パラメータをとるステートメントの処理
//  例 POSITION 3,2 )
//
// 処理対象コマンド
//   POSITION x,y sets the cursor
//   PIN pinNum, value (0 = low, non-zero = high)
//   PINMODE pinNum, mode ( 0 = input, 1 = output)
//
//   正常終了 0
//   異常終了 エラーコード
//  
int parseTwoIntCmd() {
    int op = curToken;

    // 第1引数の処理
    getNextToken();
    int val = expectNumber();
    if (val) 
        return val;	// error

    // カンマ ","のチェック
    if (curToken != TOKEN_COMMA)
      return ERROR_UNEXPECTED_TOKEN;
    
    // 第2引数の処理
    getNextToken();
    val = expectNumber();
    if (val) 
        return val;	// error

    // 実行モードの場合、コマンドの実行を行う
    if (executeMode) {
        WiringPinMode second = (WiringPinMode)stackPopNum();
        int first = (int)stackPopNum();
        switch(op) {
        case TOKEN_POSITION: // POSITION x,y
            host_moveCursor(first,second); 
            break;
        case TOKEN_PIN:      // PIN pinNum, value (0 = low, non-zero = high)
            host_digitalWrite(first,second); 
            break;
        case TOKEN_PINMODE:  // PINMODE pinNum, mode ( 0 = input, 1 = output)
            host_pinMode(first, second); 
            break;
        }
    }
    return 0;
}

// this handles both LET a$="hello" and INPUT a$ type assignments
// (これはLET a$="hello"とINPUT a$ 形式の割り当ての両方を処理する)
// ※ 変数は数値、文字列変数に対応
// 書式
//   LET variable = <expr> or variable = <expr>
//   INPUT variable
//
// 引数
//  inputStmt : 0以外 INPUT、0 LET
// 戻り値
//   正常終了 0
//   異常終了 エラーコード
//
int parseAssignment(bool inputStmt) {
    char ident[MAX_IDENT_LEN+1];
    int val;

    // 識別子の取得
    if (curToken != TOKEN_IDENT) 
        return ERROR_UNEXPECTED_TOKEN; 
    if (executeMode)
          strcpy(ident, identVal);
    int isStringIdentifier = isStrIdent;
    int isArray = 0;
    getNextToken();	// eat ident

    // 配列の場合の添え字の処理
    if (curToken == TOKEN_LBRACKET) {
        // array element being set
        val = parseSubscriptExpr();
        if (val) 
            return val;
        isArray = 1;
    }

    if (inputStmt) {
        // from INPUT statement　（INPUTステートメント）
        if (executeMode) {
            char *inputStr = host_readLine();
            if (isStringIdentifier) {
                if (!stackPushStr(inputStr)) 
                    return ERROR_OUT_OF_MEMORY;
            } else {
                float f = (float)strtod(inputStr, 0);
                if (!stackPushNum(f))
                    return ERROR_OUT_OF_MEMORY;
            }
            host_newLine();
            //host_showBuffer();
      }
      val = isStringIdentifier ? TYPE_STRING : TYPE_NUMBER;
    } else {
        // from LET statement （LETステートメント）
        if (curToken != TOKEN_EQUALS)
            return ERROR_UNEXPECTED_TOKEN;
        getNextToken(); // eat =
        val = parseExpression();
        if (val & ERROR_MASK)
            return val;
    }
    
    // type checking and actual assignment
    if (!isStringIdentifier) {	// numeric variable　（数値変数）
        if (!IS_TYPE_NUM(val))
            return ERROR_EXPR_EXPECTED_NUM;
        if (executeMode) {
            if (isArray) {
                val = setNumArrayElem(ident, stackPopNum());
                if (val) 
                    return val;
            } else {
                if (!storeNumVariable(ident, stackPopNum())) 
                    return ERROR_OUT_OF_MEMORY;
            }
        }
    }  else {	// string variable  （文字列変数）
        if (!IS_TYPE_STR(val))
            return ERROR_EXPR_EXPECTED_STR;
        if (executeMode) {
            if (isArray) {
                // annoyingly, we've got the string at the top of the stack
                // (from parseExpression) and the array index stuff (from
                // parseSubscriptExpr) underneath.
                // ( 面倒なことに(parseExpressionから)スタックの一番上に文字列があり、
                //  (parseSubscriptExprから)配列インデックスのものがあります)
                val = setStrArrayElem(ident);
                if (val)
                    return val;
            } else {
                if (!storeStrVariable(ident, stackGetStr()))
                    return ERROR_OUT_OF_MEMORY;
                stackPopStr();
            }
        }
    }
    return 0;
}

// IF 式 THEN の処理
//  戻り値
//   エラーコード
//
int parse_IF() {

    // 式の評価
    getNextToken();	// eat if
    int val = expectNumber();
    if (val) 
        return val;	// error

    // THEN のチェック
    if (curToken != TOKEN_THEN)
          return ERROR_MISSING_THEN;

    getNextToken();

    // 実行モードの場合、式の評価値が偽の場合、以降の処理をスキップ設定
    if (executeMode && stackPopNum() == 0.0f) {
        // condition not met
        breakCurrentLine = 1;
        return 0;
    } else
        return 0;
}

// FOR 変数=初期値 TO 最終値 [STEP 刻み値] の処理
//  戻り値
//   エラーコード
//
int parse_FOR() {
      char ident[MAX_IDENT_LEN+1];
      float start, end, step = 1.0f;

      // 変数の取得
      getNextToken();	// eat for
      if (curToken != TOKEN_IDENT || isStrIdent) {
          // 数値変数でない
          return ERROR_UNEXPECTED_TOKEN;
      }
      
      if (executeMode)
            strcpy(ident, identVal);

      // '='のチェック
      getNextToken();	// eat ident
      if (curToken != TOKEN_EQUALS) {
          // トークンエラー
          return ERROR_UNEXPECTED_TOKEN;
      }
      
      // 初期値の取得
      getNextToken(); // eat =
      // parse START
      int val = expectNumber();
      if (val)
          return val;	// error

      if (executeMode)
            start = stackPopNum();
  
    // parse TO （TOの処置）
    if (curToken != TOKEN_TO) {
        // トークンエラー
        return ERROR_UNEXPECTED_TOKEN;
    }
    
    // 最終値の処理
    getNextToken(); // eat TO
    // parse END
    val = expectNumber();
    if (val)
        return val;	// error

    if (executeMode)
          end = stackPopNum();

    // STEP の処理
    // parse optional STEP
    if (curToken == TOKEN_STEP) {
        // 刻みの処理
        getNextToken(); // eat STEP
        val = expectNumber();
        if (val)
            return val;	// error

        if (executeMode)
            step = stackPopNum();
    }
    
    if (executeMode) {
        if (!storeForNextVariable(ident, start, step, end, lineNumber, stmtNumber)) 
            return ERROR_OUT_OF_MEMORY;
    }
    return 0;
}

//  NEXT 変数名 の処理
//   戻り値
//    エラーコード
//
int parse_NEXT() {

    // トークン取り出し
    getNextToken();	// eat next
    if (curToken != TOKEN_IDENT || isStrIdent) {
        // 数値変数でない場合、エラーとする
        return ERROR_UNEXPECTED_TOKEN;
    }

    // 実行モードの場合、処理を行う
    if (executeMode) {
        // NEXTで指定した変数をForで登録した変数と照合
        ForNextData data = lookupForNextVariable(identVal);
        if (data.val == FLT_MAX) {
            // 見つからない
            return ERROR_VARIABLE_NOT_FOUND;
        } else if (data.step == FLT_MAX) {
            // 該当する変数でない
            return ERROR_NEXT_WITHOUT_FOR;
        }
        
        // update and store the count variable
        // カウントの更新
        data.val += data.step;
        storeNumVariable(identVal, data.val);
        
        // loop?（ループの継続チェック）
        if ((data.step >= 0 && data.val <= data.end) || (data.step < 0 && data.val >= data.end)) {
            jumpLineNumber = data.lineNumber;
            jumpStmtNumber = data.stmtNumber+1;
        }
    }

    // 次のトークン取り出し
    getNextToken();	// eat ident
    return 0;
}

// GOSUB の処理
//  戻り値
//    エラーコード
//
int parse_GOSUB() {

    // 行番号の取得
    getNextToken();  // eat gosub
    int val = expectNumber();
    if (val)
      return val;	// error

    // 実行モードの場合、処理を行う
    if (executeMode) {
        // ジャンプ先行番号の取得とセット
        uint16_t startLine = (uint16_t)stackPopNum();
        if (startLine <= 0) {
            return ERROR_BAD_LINE_NUM;
        }
        jumpLineNumber = startLine;

        // スタックに現在位置をプッシュ
        if (!gosubStackPush(lineNumber,stmtNumber)) {
            return ERROR_OUT_OF_MEMORY;
        }
    }
    return 0;
}

// LOAD or LOAD "x"
// SAVE, SAVE+ or SAVE "x"
// DELETE "x"
int parseLoadSaveCmd() {
    int op = curToken;
    char autoexec = 0, gotFileName = 0;
  
    getNextToken();
    if (op == TOKEN_SAVE && curToken == TOKEN_PLUS) {
        getNextToken();
        autoexec = 1;
    } else if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = parseExpression();
        if (val & ERROR_MASK) return val;
        if (!IS_TYPE_STR(val))
            return ERROR_EXPR_EXPECTED_STR;
        gotFileName = 1;
    }
  
    if (executeMode) {
        if (gotFileName) {
#if EXTERNAL_EEPROM
            char fileName[MAX_IDENT_LEN+1];
            if (strlen(stackGetStr()) > MAX_IDENT_LEN)
                return ERROR_BAD_PARAMETER;
            strcpy(fileName, stackPopStr());
            if (op == TOKEN_SAVE) {
                if (!host_saveExtEEPROM(fileName))
                    return ERROR_OUT_OF_MEMORY;
            } else if (op == TOKEN_LOAD) {
                reset();
                if (!host_loadExtEEPROM(fileName))
                    return ERROR_BAD_PARAMETER;
            } else if (op == TOKEN_DELETE) {
                if (!host_removeExtEEPROM(fileName))
                    return ERROR_BAD_PARAMETER;
            }
#endif
        } else {
            if (op == TOKEN_SAVE) {
#if !defined (__STM32F1__)
              host_saveProgram(autoexec);
#endif
            } else if (op == TOKEN_LOAD) {
                reset();
#if !defined (__STM32F1__)
                host_loadProgram();
#endif
            }  else
                return ERROR_UNEXPECTED_CMD;
        }
    }
    return 0;
}

// 引数無しコマンドの処理
//  戻り値
//   エラーコード
//
int parseSimpleCmd() {
    int op = curToken;
    getNextToken();	// eat op
   
    // 実行モードの場合、各コマンドの処理を行う
    if (executeMode) {
        switch (op) {
        case TOKEN_NEW:     // NEW コマンド
            reset();
            breakCurrentLine = 1;
            break;

        case TOKEN_STOP:    // STOP コマンド
            stopLineNumber = lineNumber;
            stopStmtNumber = stmtNumber;
            return ERROR_STOP_STATEMENT;

        case TOKEN_CONT:    // CONT コマンド
            if (stopLineNumber) {
                jumpLineNumber = stopLineNumber;
                jumpStmtNumber = stopStmtNumber+1;
            }
            break;

        case TOKEN_RETURN: { // RETURN コマンド
            int returnLineNumber, returnStmtNumber;
            if (!gosubStackPop(&returnLineNumber, &returnStmtNumber))
                return ERROR_RETURN_WITHOUT_GOSUB;
            jumpLineNumber = returnLineNumber;
            jumpStmtNumber = returnStmtNumber+1;
            break;
        }

        case TOKEN_CLS:     // CLS コマンド
            host_cls();
            break;

        case TOKEN_DIR:     // DIR コマンド
#if EXTERNAL_EEPROM
            host_directoryExtEEPROM();
#endif
            break;
        }
    }
    return 0;
}

// 配列の処理
// 戻り値
//  エラーコード
//
int parse_DIM() {
    char ident[MAX_IDENT_LEN+1];  // 配列名

    // 配列名トークン取り出し
    getNextToken();	// eat DIM
    if (curToken != TOKEN_IDENT) {
        return ERROR_UNEXPECTED_TOKEN;
    }
    
    // 実行モードの場合、配列名を保持
    if (executeMode) {
        strcpy(ident, identVal);
    }
    
    int isStringIdentifier = isStrIdent; // 文字列配列フラグ保持
    getNextToken();	// eat ident         // 次のトークン取り出し

    // 添え字の処理
    int val = parseSubscriptExpr();      // 添え字の取り出し（スタックに積まれる）
    if (val) {
      return val;
    }

    // 実行モードの場合、配列を作成する
    if (executeMode && !createArray(ident, isStringIdentifier ? 1 : 0)) {
        return ERROR_OUT_OF_MEMORY;
    }
    
    return 0;
}

static int targetStmtNumber;

// ステートメントの処理
//  戻り値
//   エラーコード
int parseStmts() {
    int ret = 0;
    breakCurrentLine = 0;
    jumpLineNumber = 0;
    jumpStmtNumber = 0;

    // ステートメントの処理ループ
    while (ret == 0) {
        if (curToken == TOKEN_EOL)
            break;

        // 実行モードの場合、計算器スタックを初期化する
        if (executeMode)
            sysSTACKEND = sysSTACKSTART = sysPROGEND;	// clear calculator stack

        int needCmdSep = 1;
        switch (curToken) {
        case TOKEN_PRINT: ret = parse_PRINT();  break;
        case TOKEN_LET:   getNextToken(); 
                          ret = parseAssignment(false); break;
        case TOKEN_IDENT: ret = parseAssignment(false); break;
        case TOKEN_INPUT: getNextToken();
                          ret = parseAssignment(true); break;
        case TOKEN_LIST:  ret = parse_LIST();   break;
        case TOKEN_RUN:   ret = parse_RUN();    break;
        case TOKEN_GOTO:  ret = parse_GOTO();   break;
        case TOKEN_REM:   getNextToken();
                          getNextToken();       break;
        case TOKEN_IF:    ret = parse_IF();
                          needCmdSep = 0;       break;
        case TOKEN_FOR:   ret = parse_FOR();    break;
        case TOKEN_NEXT:  ret = parse_NEXT();   break;
        case TOKEN_GOSUB: ret = parse_GOSUB();  break;
        case TOKEN_DIM:   ret = parse_DIM();    break;
        case TOKEN_PAUSE: ret = parse_PAUSE();  break;
        
        // ロード・セーブコマンド
        case TOKEN_LOAD:
        case TOKEN_SAVE:
        case TOKEN_DELETE:
            ret = parseLoadSaveCmd();
            break;
        
        // 整数型引数を２つもつコマンド
        case TOKEN_POSITION:
        case TOKEN_PIN:
        case TOKEN_PINMODE:
            ret = parseTwoIntCmd(); 
            break;

        // 引数なしコマンド
        case TOKEN_NEW:
        case TOKEN_STOP:
        case TOKEN_CONT:
        case TOKEN_RETURN:
        case TOKEN_CLS:
        case TOKEN_DIR:
            ret = parseSimpleCmd();
            break;
            
        default: 
            ret = ERROR_UNEXPECTED_CMD;
    }
      
    // if error, or the execution line has been changed, exit here
    // (エラーが発生した場合、または実行行が変更された場合は、ここで終了します)
    if (ret || breakCurrentLine || jumpLineNumber || jumpStmtNumber)
        break;
        
    // it should either be the end of the line now, and (generally) a command seperator
    // before the next command
    // (今すぐ行末か、（一般的に）次のコマンドの前のコマンドセパレータのどちらかになります)
    if (curToken != TOKEN_EOL) {
        if (needCmdSep) {
            if (curToken != TOKEN_CMD_SEP) {
                ret = ERROR_UNEXPECTED_CMD;
            } else {
                getNextToken();
                // don't allow a trailing :
                if (curToken == TOKEN_EOL) {
                    ret = ERROR_UNEXPECTED_CMD;
                }
            }
        }
    }
    
    // increase the statement number
    // ステートメント数を増やす
    stmtNumber++;
    // if we're just scanning to find a target statement, check
    // (ターゲットステートメントを探すためだけにスキャンしている場合は、チェックしてください)
    if (stmtNumber == targetStmtNumber)
        break;
    }
    return ret;
}

// インタープリタの処理
//  引数
//    tokenBuf : 中間コードトークンバッファ
//  戻り値
//    エラーコード  
//
int processInput(unsigned char *tokenBuf) {
    // first token can be TOKEN_INTEGER for line number - stored as a float in numVal
    // (最初のトークンは行番号のTOKEN_INTEGERにすることができます -  numValにfloatとして格納されます)
    // store as WORD line number (max 65535)
    // (行番号はWORD型として保存（最大 65535）)
  
    tokenBuffer = tokenBuf;                   // トークン取り出し位置の保持
    getNextToken();                           // トークン取り出し（グローバル変数に格納）

    // check for a line number at the start
    // (先頭行番号のための確認)
    uint16_t gotLineNumber = 0;               // 行番号
    unsigned char *lineStartPtr = 0;          // 行へのポインタ

    if (curToken == TOKEN_INTEGER) {          // 型チェック
        long val = (long)numVal;              // 行番号取得
        if (val <=65535) {
            gotLineNumber = (uint16_t)val;    // 行番号保持
            lineStartPtr = tokenBuffer;       // 行へのポインタの保持
            getNextToken();                   // 次のトークン取り出し
        } else {
            return ERROR_LINE_NUM_TOO_BIG;    // 行番号が大きすぎる（終了する）
        }
    }

    executeMode = 0;                            // 実行モード off
    targetStmtNumber = 0;                       // ターゲットステートメント番号 = 0

    // syntax check （文法チェック）
    int ret = parseStmts();                    // ステートメントのチェック
    if (ret != ERROR_NONE)
        return ret;                            // 文法エラー（終了する）

    if (gotLineNumber) {
        // 行番号がある場合、行をプログラム領域に登録
        if (!doProgLine(gotLineNumber, lineStartPtr, tokenBuffer - lineStartPtr))
            ret = ERROR_OUT_OF_MEMORY;        // メモリ不足
    } else {
        // we start off executing from the input buffer
        // (行番号無しの場合、入力バッファからの実行を開始)
        tokenBuffer = tokenBuf;               // トークンバッファの保持
        executeMode = 1;                      // 実行モード on
        lineNumber = 0;	// buffer             // バッファのステートメントの実行
        unsigned char *p;

        // ステートメントの実行ループ
        while (1) {
            getNextToken();                 // トークン取り出し
            stmtNumber = 0;                 // ステートメント番号 0

            // skip any statements? (e.g. for/next)
            // (文をスキップするか？（for/next等の場合）
            if (targetStmtNumber) {
                executeMode = 0; 
                parseStmts(); 
                executeMode = 1;
                targetStmtNumber = 0;
            }

            // now execute
            // ステートメントの実行
            ret = parseStmts();
            if (ret != ERROR_NONE)
                break; // エラー発生なら処理を中断
    
            // are we processing the input buffer?
            // (入力バッファを処理しているか)
            if (!lineNumber && !jumpLineNumber && !jumpStmtNumber)
                break;	// if no control flow jumps, then just exit
                        // （制御フローがジャンプしない場合は、そのまま終了する）
    
            // are we RETURNing to the input buffer?
            // (入力バッファに戻るか)
            if (lineNumber && !jumpLineNumber && jumpStmtNumber)
                lineNumber = 0;
      
            if (!lineNumber && !jumpLineNumber && jumpStmtNumber) {
                // we're executing the buffer, and need to jump stmt (e.g. for/next)
                // (バッファを実行しているので、stmtにジャンプする必要がある（例：for / next）)
                tokenBuffer = tokenBuf;
            } else {
                // we're executing the program （プログラムを実行してる場合）
                if (jumpLineNumber || jumpStmtNumber) {
                    // line/statement number was changed e.g. goto
                    // (//行番号/文番号が変更された 例：gotoで）
                    p = findProgLine(jumpLineNumber);  // 指定行にジャンプ
                } else {
                    // line number didn't change, so just move one to the next one
                    // (行番号は変わらないので、次の行に移動)
                    p+= *(uint16_t *)p;                // 次の行に移動
                }
            
                // end of program? （プログラムの終わりに達したか？）
                if (p == &mem[sysPROGEND])
                    break;	// end of program （プログラムの終了）
        
                lineNumber = *(uint16_t*)(p+2);  // 行番号
                tokenBuffer = p+4;               // 実行対象トークンへのポインタ
        
                // if the target for a jump is missing (e.g. line deleted) and we're on the next line
                // reset the stmt number to 0
                // (ジャンプのターゲットが欠落している（例えば行が削除されている）場合は、次の行に移動します。
                //  stmt番号を0にリセットします)
                if (jumpLineNumber && jumpStmtNumber && lineNumber > jumpLineNumber)
                    jumpStmtNumber = 0;
            }

            if (jumpStmtNumber)
                targetStmtNumber = jumpStmtNumber;
        
            // 中断チェック
            if (host_ESCPressed()) { 
                ret = ERROR_BREAK_PRESSED; 
                break; 
            }
        }
    }
    return ret;
}

// インタープリタの初期化
void reset() {
    // program at the start of memory 
    // (プログラムはメモリの先頭から利用)
    sysPROGEND = 0;
  
    // stack is at the end of the program area 
    // (スタックはプログラム域の終わりにあります)
    sysSTACKSTART = sysSTACKEND = sysPROGEND;
    
    // variables/gosub stack at the end of memory
    //（メモリの末尾に変数/gosubスタック）
    sysVARSTART = sysVAREND = sysGOSUBSTART = sysGOSUBEND = MEMORY_SIZE;
    
    memset(&mem[0], 0, MEMORY_SIZE);
  
    stopLineNumber = 0;  // 中断行番号
    stopStmtNumber = 0;  // 中断ステートメント番号
    lineNumber = 0;      // 行番号
}
