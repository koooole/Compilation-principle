/****************************************************
 * File: code.h
 * Function: 符号表的构建
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/8/3
 ****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* 将行号和存储位置插入符号表
 * loc = 第一次插入存储位置，否则将被忽略
 */
void st_insert( char * name, int lineno, int loc );

/* 返回变量的内存位置或者定义变量的位置行号，如果找不到，则返回-1
 * name为变量名
 * flag为返回内存位置还是定义变量的位置行号的标记
 * 如果flag=0，返回内存位置，否则返回定义变量的位置行号
 */
int st_lookup ( char * name, int flag );

/* 将符号表内容中，格式化后的listing打印到listing文件中 */
void printSymTab(FILE * listing);

/* 释放哈希表内存 */
void hashRelease();

#endif
