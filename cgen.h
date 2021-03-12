/****************************************************
 * File: cgen.h
 * Function: Mini C编译器的代码生成器实现(为TM机器生成代码)
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/8/3
 ****************************************************/

#ifndef _CGEN_H_
#define _CGEN_H_

/* 通过遍历语法树生成代码文件的代码。
 * 第二个参数(codefile)是代码文件的文件名，用于将文件名打印为代码文件中的注释
 */
void codeGen(TreeNode * syntaxTree, char * codefile);

#endif
