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

string KEYWORDS[MAXKEYWORDS] = { "include","main", "int", "float","double", "cin", "if", "cout" }; //�ؼ��֣� ����ȫ������Ա���ʵ���ı�
const char OPT1[MAXOPT1] = { '#','<','>','(', ')','{', '}','[',']',',',';',':','/','*','!','&','|','~','^','?'};  //������ţ���Ŀ
string OPT2[MAXOPT2] = {"&&","||","==",">=","<=","!=","++","--","::","<<",">>","+=","-=","*=","/=","%=","&=","^=","->",":=" }; // ˫Ŀ

bool letter(char); // �ж��Ƿ���ĸ
bool dight(char); // �ж��Ƿ�����
bool key(string ); // �ж��Ƿ�ؼ���
bool lable1(char); // �ж��Ƿ�Ŀ�����ַ�
bool lable2(string ); //�ж��Ƿ�˫Ŀ�����ַ�
void Scanner(const char*, int); // �ʷ�����
string str_cat(const char*, int); // �ַ�����ת�ַ���

/*
int main() {

	cout << "��ȡ�ļ�" << endl;
	ifstream infile("D:\\Test.cpp");
	if (!infile) {
		cout << "�ļ����ܴ�" << endl;
	}
	char str_arr[100] ;  //���ڴ洢���д��ļ���ȡ������
	int count = 1;  // ��ȡ������ 
	while (infile.getline(str_arr, 300)) {
		//cout << str_arr << endl;
		//cout << "��" << count << "��" << endl;

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
	cout << "ɨ�����" << endl;

	return 0;
}
*/

// �ʷ�����
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
    while (token != '\r' ) {  //qt���ļ����б�־Ϊ��\r\n��
		// �ո�����
		if (token == ' ') {
			token = arr[++index];
		}

		// ����
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
            //0�ؼ��֣� 1������ţ� 2��ʶ���� 3���� 4���� 5ע��
            addNew(row, snumber, 3);
            //cout << snumber << "     ��" << endl;

		}

		// ��ĸ�����»���
		if (letter(token) || token == '_') {
			char word[20];
			int wordlen = 0;
			word[wordlen++] = token;
			token = arr[++index];
			while (letter(token) || token == '_' || dight(token)) {
				word[wordlen++] = token;
				token = arr[++index];
			}
			// �ؼ���
			string sword = str_cat(word, wordlen - 1);
			if (key(sword)) {
                addNew(row, sword, 0);
                //cout << sword << "     �ؼ���" << endl;
			}
			// ��ʶ��
			else {
                addNew(row, sword, 2);
                //cout << sword << "     ��ʶ��" << endl;
			}
		}

		// ������ţ�������Ĺؼ���)
		if (lable1(token)) {
			char key[5];
			int keylen = 0;
			key[0] = token;
			token = arr[++index];

			//����ؼ������
			if (key[0] == '<' && letter(token)) {
                string skey = str_cat(key, 0);
                addNew(row, skey, 1);
                //cout << key[0] << "     �������" << endl;
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
                        //cout << sword << "     �ؼ���" << endl;
                        //cout << token << "     �������" << endl;
						token = arr[++index];
						break;
					}
				}
			
			}

			//ע��1/* */
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
                            //cout << sword << "     ע��" << endl;
							break;
						}
					}
				}
			}

			//ע��2 //
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
                //cout << sword << "     ע��" << endl;
			}

			else {
				// ˫Ŀ�����
				if (lable1(token)) { 
					key[++keylen] = token;
					token = arr[++index];
					string skey = str_cat(key, keylen);
					if (lable2(skey)) {
                        addNew(row, skey, 1);
                        //cout << skey << "     �������" << endl;
					}
					else {
                        string skey1 = " ";
                        skey1[0] = key[0];
                        string skey2 = " ";
                        skey2[0] = key[1];
                        addNew(row, skey1, 1);
                        addNew(row, skey2, 1);
                        //cout << key[0] << "     �������" << endl;
                        //cout << key[1] << "     �������" << endl;
					}
				}
				// ��Ŀ�����
				else {
                    string skey = str_cat(key, 0);
                    addNew(row, skey, 1);
                    //cout << key[0] << "     �������" << endl;
				}
			}
		}

		// ��
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
                    //cout << sword << "     ��" << endl;
					break;
				}
				word[wordlen++] = token;
				token = arr[++index];
			}
		}

	}
	return;
}


//�ж��Ƿ���ĸ
bool letter(char ch)
{
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
		return true;
	else
		return false;
}

//�ж��Ƿ�����
bool dight(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	else
		return false;
}

//�ж��Ƿ�Ϊ�ؼ���
bool key(string s) {
	for (int i = 0; i < MAXKEYWORDS; i++) {
		if (s == KEYWORDS[i])
			return true;
	}
	return false;
}

//�ж��Ƿ�Ϊ��Ŀ����
bool lable1(char ch) {
	for (int i = 0; i < MAXOPT1; i++) {
		if (ch == OPT1[i])
			return true;
	}
	return false;
}

//�ж��Ƿ�Ϊ˫Ŀ����
bool lable2(string s) {

	for (int i = 0; i < MAXOPT2; i++) {
		if (s == OPT2[i])
			return true;
	}
	return false;
}

//�ַ�����ת�ַ���
string str_cat(const char *arr, int length) {

	string s = " ";
	s[0] = arr[0];
	for (int i = 1; i <= length; i++) {
		s += arr[i];
	}
	return s;
}

