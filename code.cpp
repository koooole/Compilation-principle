/****************************************************
 * File: code.cpp
 * Function: 代码指令的解释执行的实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/8/3
 ****************************************************/

#include "globals.h"
#include "code.h"

/* 当前指令发送的TM位置号 */
static int emitLoc = 0 ;

/* 到目前为止发送的最高TM位置
 * 与emitSkip,emitBackup和emitRestore结合使用
 */
static int highEmitLoc = 0;

/* 在代码文件中打印带有注释c的注释行 */
void emitComment( char * c )
{
    if (TraceCode)
        fprintf(code,"* %s\n",c);
}

/* 发出一个仅寄存器的TM指令
 * op = 操作码
 * r = 目标寄存器
 * s = 第一个源寄存器
 * t = 第二个源寄存器
 * c = TraceCode为真时要打印的注释
 */
void emitRO( char *op, int r, int s, int t, char *c)
{
    fprintf(code,"%3d:  %5s  %d,%d,%d ",emitLoc++,op,r,s,t);
    if (TraceCode)
        fprintf(code,"\t%s",c) ;
    fprintf(code,"\n") ;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc ;
} /* emitRO */

/* 发出一个从寄存器到存储器的指令
 * op = 操作码
 * r = 目标寄存器
 * d = 偏移量
 * s = 基本寄存器
 * c = TraceCode为真时要打印的注释
 */
void emitRM( char * op, int r, int d, int s, char *c)
{
    fprintf(code,"%3d:  %5s  %d,%d(%d) ",emitLoc++,op,r,d,s);
    if (TraceCode)
        fprintf(code,"\t%s",c) ;
    fprintf(code,"\n") ;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc ;
} /* emitRM */

/* 跳过"howMany"代码位置，以备后用。
 * 返回当前代码位置
 */
int emitSkip( int howMany)
{
    int i = emitLoc;
    emitLoc += howMany ;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc ;
    return i;
} /* emitSkip */

/* 备份，至loc = 一个先前跳过的位置 */
void emitBackup( int loc)
{
    if (loc > highEmitLoc)
        emitComment("BUG in emitBackup");
    emitLoc = loc ;
} /* emitBackup */

/* 将当前代码位置恢复到之前未发送的最高位置 */
void emitRestore(void)
{
    emitLoc = highEmitLoc;
}

/* 发送一个从寄存器到内存转换指令时，将绝对引用转换为相对于pc的引用
 * op = 操作码
 * r = 目标寄存器
 * a = 内存中的绝对位置
 * c = TraceCode为真时要打印的注释
 */
void emitRM_Abs( char *op, int r, int a, char * c)
{
    fprintf(code,"%3d:  %5s  %d,%d(%d) ",emitLoc,op,r,a-(emitLoc+1),pc);
    ++emitLoc ;
    if (TraceCode)
        fprintf(code,"\t%s",c) ;
    fprintf(code,"\n") ;
    if (highEmitLoc < emitLoc)
        highEmitLoc = emitLoc ;
} /* emitRM_Abs */
