/***************************************************
 * File: util.h
 * Function: 实现 Mini C 编译器的解析部分和词法分析部分时，
 *           需要用到的一些有效函数
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/7/10
 ****************************************************/

#ifndef UTIL_H
#define UTIL_H

/* 标记输出 */
void printToken( TokenType, const char* );

/* 为语法树构造创建一个新的语句节点 */
TreeNode * newStmtNode(StmtKind);

/* 为语法树构造创建新的表达式节点 */
TreeNode * newExpNode(ExpKind);

/* 分配并复制现有字符串 */
char * copyString( char * );

/* 将语法树输出到Syntax_listing文件，用缩进表示子树 */
void printTree( TreeNode * );


#endif // UTIL_H
