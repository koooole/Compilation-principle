#include<iostream>
#include<fstream>
#include<cstring>

#include "mywidget.h"
#include "ui_mywidget.h"
#include <QDebug>

using namespace std;
const int MAXKEYWORDS = 8;
const int MAXOPT1 = 20;
const int MAXOPT2 = 20;

string KEYWORDS[MAXKEYWORDS] = { "include","main", "int", "float","double", "cin", "if", "cout" }; //关键字， 不完全，仅针对本次实验文本
const char OPT1[MAXOPT1] = { '#','<','>','(', ')','{', '}','[',']',',',';',':','/','*','!','&','|','~','^','?'};  //特殊符号，单目
string OPT2[MAXOPT2] = {"&&","||","==",">=","<=","!=","++","--","::","<<",">>","+=","-=","*=","/=","%=","&=","^=","->",":=" }; // 双目

bool letter(char); // 判断是否字母
bool dight(char); // 判断是否数字
bool key(string ); // 判断是否关键字
bool lable1(char); // 判断是否单目特殊字符
bool lable2(string ); //判断是否双目特殊字符
void Scanner(const char*, int); // 词法分析
string str_cat(const char*, int); // 字符数组转字符串

/*
int main() {

	cout << "读取文件" << endl;
	ifstream infile("D:\\Test.cpp");
	if (!infile) {
		cout << "文件不能打开" << endl;
	}
	char str_arr[100] ;  //用于存储按行从文件读取的数据
	int count = 1;  // 读取的行数 
	while (infile.getline(str_arr, 300)) {
		//cout << str_arr << endl;
		//cout << "第" << count << "行" << endl;

		int i=0;
		while (str_arr[i] != '\0') {
			cout << str_arr[i++] << endl;
		}

		Scanner(str_arr, count);
		for (int i = 0; i < 100; i++) {
			str_arr[i] = '\0';
		}
		count++;
	}
	cout << "扫描完成" << endl;

	return 0;
}
*/

// 词法分析
void Scanner(const char *arr, int row) {

    //qDebug() << arr;
	int index = 0;
	char token = arr[index];
	/*
	cout << arr << endl;
	while (token != '\0') {
		cout << token << endl;
		token = arr[++index];
	}
	*/
    while (token != '\r' ) {  //qt打开文件换行标志为“\r\n”
		// 空格跳过
		if (token == ' ') {
			token = arr[++index];
		}

		// 数字
		if (dight(token)) {
			char number[10];
			int numlen = 0;
			number[numlen++] = token;
			token = arr[++index];
			while (dight(token) || token == '.') {
				number[numlen++] = token;
				token = arr[++index];
			}
			string snumber = str_cat(number, numlen-1);
            //0关键字， 1特殊符号， 2标识符， 3数， 4串， 5注释
            addNew(row, snumber, 3);
            //cout << snumber << "     数" << endl;

		}

		// 字母或者下划线
		if (letter(token) || token == '_') {
			char word[20];
			int wordlen = 0;
			word[wordlen++] = token;
			token = arr[++index];
			while (letter(token) || token == '_' || dight(token)) {
				word[wordlen++] = token;
				token = arr[++index];
			}
			// 关键字
			string sword = str_cat(word, wordlen - 1);
			if (key(sword)) {
                addNew(row, sword, 0);
                //cout << sword << "     关键字" << endl;
			}
			// 标识符
			else {
                addNew(row, sword, 2);
                //cout << sword << "     标识符" << endl;
			}
		}

		// 特殊符号（含特殊的关键字)
		if (lable1(token)) {
			char key[5];
			int keylen = 0;
			key[0] = token;
			token = arr[++index];

			//特殊关键字情况
			if (key[0] == '<' && letter(token)) {
                string skey = str_cat(key, 0);
                addNew(row, skey, 1);
                //cout << key[0] << "     特殊符号" << endl;
				char word[20];
				int wordlen = 0;
				while (1) {
					word[wordlen++] = token;
					token = arr[++index];
					if (token == '>') {
						string sword = str_cat(word, wordlen-1);
                        string stoken = " ";
                        stoken[0] = token;
                        addNew(row, sword, 0);
                        addNew(row, stoken, 1);
                        //cout << sword << "     关键字" << endl;
                        //cout << token << "     特殊符号" << endl;
						token = arr[++index];
						break;
					}
				}
			
			}

			//注释1/* */
			else if (key[0] == '/' && token == '*') {
				char word[100];
				int wordlen = 1;
				word[0] = key[0];
				while (1) {
					word[wordlen++] = token;
					token = arr[++index];
					if (token == '*') {
						word[wordlen++] = token;
						token = arr[++index];
						if (token == '/') {
							word[wordlen++] = token;
							token = arr[++index];
							string sword = str_cat(word, wordlen - 1);
                            addNew(row, sword, 5);
                            //cout << sword << "     注释" << endl;
							break;
						}
					}
				}
			}

			//注释2 //
			else if (key[0] == '/' && token == '/') {
				char word[100];
				int wordlen = 1;
				word[0] = key[0];
                while (token != '\r') {
					word[wordlen++] = token;
					token = arr[++index];
				}
				string sword = str_cat(word, wordlen - 1);
                addNew(row, sword, 5);
                //cout << sword << "     注释" << endl;
			}

			else {
				// 双目运算符
				if (lable1(token)) { 
					key[++keylen] = token;
					token = arr[++index];
					string skey = str_cat(key, keylen);
					if (lable2(skey)) {
                        addNew(row, skey, 1);
                        //cout << skey << "     特殊符号" << endl;
					}
					else {
                        string skey1 = " ";
                        skey1[0] = key[0];
                        string skey2 = " ";
                        skey2[0] = key[1];
                        addNew(row, skey1, 1);
                        addNew(row, skey2, 1);
                        //cout << key[0] << "     特殊符号" << endl;
                        //cout << key[1] << "     特殊符号" << endl;
					}
				}
				// 单目运算符
				else {
                    string skey = str_cat(key, 0);
                    addNew(row, skey, 1);
                    //cout << key[0] << "     特殊符号" << endl;
				}
			}
		}

		// 串
		if(token == '\"'){
			char word[50];
			int wordlen = 0;
			word[wordlen++] = token;
			token = arr[++index];
			while (1) {
				if (token == '\"') {
					word[wordlen++] = token;
					token = arr[++index];
					string sword = str_cat(word, wordlen-1);
                    addNew(row, sword, 4);
                    //cout << sword << "     串" << endl;
					break;
				}
				word[wordlen++] = token;
				token = arr[++index];
			}
		}

	}
	return;
}


//判断是否字母
bool letter(char ch)
{
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
		return true;
	else
		return false;
}

//判断是否数字
bool dight(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	else
		return false;
}

//判断是否为关键字
bool key(string s) {
	for (int i = 0; i < MAXKEYWORDS; i++) {
		if (s == KEYWORDS[i])
			return true;
	}
	return false;
}

//判断是否为单目符号
bool lable1(char ch) {
	for (int i = 0; i < MAXOPT1; i++) {
		if (ch == OPT1[i])
			return true;
	}
	return false;
}

//判断是否为双目符号
bool lable2(string s) {

	for (int i = 0; i < MAXOPT2; i++) {
		if (s == OPT2[i])
			return true;
	}
	return false;
}

//字符数字转字符串
string str_cat(const char *arr, int length) {

	string s = " ";
	s[0] = arr[0];
	for (int i = 1; i <= length; i++) {
		s += arr[i];
	}
	return s;
}

