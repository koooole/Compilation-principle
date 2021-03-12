/****************************************************
 * File: code.h
 * Function: 代码指令的解释执行的实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/8/3
 ****************************************************/

#ifndef CODE_H_
#define CODE_H_

/* pc = 程序计数器  */
#define  pc 7

/* mp = "memory pointer"：指向内存顶部(用于临时存储) */
#define  mp 6

/* gp = "global pointer"：指向(全局)变量存储的内存底部 */
#define gp 5

/* 累加器 */
#define  ac 0

/* 第二累加器 */
#define  ac1 1


/* 代码发送实用程序 */


/* 在代码文件中打印带有注释c的注释行 */
void emitComment( char * c );

/* 发出一个仅寄存器的TM指令
 * op = 操作码
 * r = 目标寄存器
 * s = 第一个源寄存器
 * t = 第二个源寄存器
 * c = TraceCode为真时要打印的注释
 */
void emitRO( char *op, int r, int s, int t, char *c);

/* 发出一个从寄存器到存储器的指令
 * op = 操作码
 * r = 目标寄存器
 * d = 偏移量
 * s = 基本寄存器
 * c = TraceCode为真时要打印的注释
 */
void emitRM( char * op, int r, int d, int s, char *c);

/* 跳过"howMany"代码位置，以备后用。
 * 返回当前代码位置
 */
int emitSkip( int howMany);

/* 备份，至loc = 一个先前跳过的位置 */
void emitBackup( int loc);

/* 将当前代码位置恢复到之前未发送的最高位置 */
void emitRestore(void);

/* 发送一个从寄存器到内存转换指令时，将绝对引用转换为相对于pc的引用
 * op = 操作码
 * r = 目标寄存器
 * a = 内存中的绝对位置
 * c = TraceCode为真时要打印的注释
 */
void emitRM_Abs( char *op, int r, int a, char * c);

#endif
