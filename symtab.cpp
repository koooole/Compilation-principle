/****************************************************
 * File: code.cpp
 * Function: 符号表的构建
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/8/3
 ****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE: 哈希表的大小 */
#define SIZE 211

/* SHIFT: 在哈希函数中，2的幂作为乘数 */
#define SHIFT 4

/* 哈希函数 */
static int hash ( char * key )
{
    int temp = 0;
    int i = 0;
    while (key[i] != '\0')
    {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/* 引用变量的源代码的行号列表 */
typedef struct LineListRec
{
    int lineno;
    struct LineListRec * next;
} * LineList;

/* 桶列表中每个变量的记录
 * 包括名称、分配的内存位置以及它在源代码中出现的行号列表 */
typedef struct BucketListRec
{   char * name;
    LineList lines;
    int memloc ;     // 变量的存储位置
    struct BucketListRec * next;
} * BucketList;

/* 哈希表 */
static BucketList hashTable[SIZE];

/* 将行号和存储位置插入符号表
 * loc = 第一次插入存储位置，否则将被忽略
 */
void st_insert( char * name, int lineno, int loc )
{
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
    if (l == NULL) // 变量尚未在表中
    {
        l = (BucketList) malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->memloc = loc;
        l->lines->next = NULL;
        l->next = hashTable[h];
        hashTable[h] = l;
    }

    else // 在表中找到，所以只需添加行号
    {
        LineList t = l->lines;
        while (t->next != NULL)
            t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
} /* st_insert */


/* 返回变量的内存位置或者定义变量的位置行号，如果找不到，则返回-1
 * name为变量名
 * flag为返回内存位置还是定义变量的位置行号的标记
 * 如果flag=0，返回内存位置，否则返回定义变量的位置行号
 */
int st_lookup ( char * name, int flag)
{
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
    if (l == NULL)
        return -1;
    else{
        if(flag == 0) return l->memloc;
        else return l->lines->lineno;
    }

}

/* 将符号表内容中，格式化后的Analyze_listing打印到Analyze_listing文件中 */
void printSymTab(FILE * Analyze_listing)
{
    int i;
    fprintf(Analyze_listing,"Variable Name  Location   Line Numbers\n");
    fprintf(Analyze_listing,"-------------  --------   ------------\n");
    for (i=0;i<SIZE;++i)
    {
        if (hashTable[i] != NULL)
        {
            BucketList l = hashTable[i];
            while (l != NULL)
            {
                LineList t = l->lines;
                fprintf(Analyze_listing,"%-14s ",l->name);
                fprintf(Analyze_listing,"%-8d  ",l->memloc);

                int count = 0;
                while (t != NULL)
                {
                    if(count > 6){
                        fprintf(Analyze_listing,"\n");
                        fprintf(Analyze_listing,"                         ");
                        fprintf(Analyze_listing,"%4d ",t->lineno);
                        t = t->next;
                        count = 0;
                    }
                    else{
                        fprintf(Analyze_listing,"%4d ",t->lineno);
                        t = t->next;
                    }
                    count++;

                }
                fprintf(Analyze_listing,"\n");
                l = l->next;
            }
        }
    }

} /* printSymTab */

/* 释放哈希表内存 */
void hashRelease(){
    for (int i=0;i<SIZE;++i){
        hashTable[i] = NULL;
    }
}
