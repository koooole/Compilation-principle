/***************************************************
 * File: analyze.h
 * Function: Mini C编译器的语义分析器实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/8/3
 ***************************************************/

#ifndef ANALYZE_H_
#define ANALYZE_H_

/* 通过语法树的前序遍历构建符号表 */
void buildSymtab(TreeNode *);

/* 通过后置语法树遍历来执行类型检查 */
void typeCheck(TreeNode *);

#endif
