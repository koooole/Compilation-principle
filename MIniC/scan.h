/***************************************************
 * File: scan.h
 * Function: Mini C 编译器的词法分析部分的实现
 * version: 1.10
 * Author: yongdong chen
 * Date: 2020/7/10
 ****************************************************/


#ifndef SCAN_H
#define SCAN_H

/* token的最大长度 */
#define MAXTOKENLEN 40
extern char tokenString[MAXTOKENLEN+1];

/* 对扫描的字符进行匹配判断(获取token) */
TokenType getToken(void);

/* 生成词法分析结果 */
void genToken(TokenType, const char*);

#endif // SCAN_H
