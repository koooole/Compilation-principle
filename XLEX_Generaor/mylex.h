#ifndef MYLEX_H
#define MYLEX_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "mygraph.h"
#include <QtWidgets/QTextBrowser>
#include <QtGlobal>

using namespace std;

class NFA{
public:
    vector<int> mVexs;//DFA节点集合，存储的是节点的序号,位置的信息并没有存储内容，和DFA的节点集合不一样
    MyGraph NFAGraph;//NFA图的结构

    int startStatus;//开始状态的序号
    int endStatus;//终止状态的序号

    /*int getGraphPointNum(){
        return this->NFAGraph.mVexNum;
    }*/
};

class DFA{
public:
    vector<vector<int>> mVexs; //值为对于的NFA集合,位置为节点序号
    MyGraph DFAGraph; //DFA图的结构
    int startStatus; //开始状态
    vector <int> endStatus; //可能有多个终止状态
    int getTargetStatus(int node, char condition); //获取与某个节点相连的节点，转换条件确定
};

//给NFA栈中的元素的定义的一个结构，存储的是起点和终点的信息。
struct node{
public:
    int nArray[2];
    node(int a[2]){
        for(int i =0;i<2;i++)
            nArray[i] = a[i];
    }
};

class Lex{
public:
    string dot; //环境变量路径
    void setDot(); //设置环境变量路径

    NFA lexNFA;//NFA结构
    DFA lexDFA;//DFA结构
    stack <char> operatorStack;//操作符栈
    stack <node> NFAStatusPointStack;//NFA栈中的NFA的起始点序号，0的位置代表起点，1的位置代表终点
    vector<char> alphabet;//字母表，存储是正则表达式中的字母
    string projectFile;
    //构造函数
    Lex(){projectFile = "..\\XLEX_Generaor";}

    void getNFA(string regxInput); //获取NFA
    bool isOperator(char ch); //判断是否是操作符
    void createBasicNFA(char ch);  //创建基本的NFA，两个节点一条边，具体就是是把两个节点连起来，边的值为转移条件
    void repeatCharacterOperation(); //遇到重复符号的操作
    void selectorCharacterOperation(); //遇到选择符的操作
    void joinerCharacterOperation();  //遇到连接符的操作
    string generateNFADotString(MyGraph myGraph); //生成NFA的dot文本

    void getDFA(); //获取DFA
    vector<int> e_closure(vector<int> statusArray);
    vector<int> nfaMove(vector<int> statusArray,char condition);
    int isDFAStatusRepeat(vector<int> a);
    string generateDFADotString(MyGraph myGraph,int choice);
    bool isEndDFAStatus(vector<int> nfaArray); //判断是否是终止状态

    void minimizeDFA(); //最小化DFA
    bool isInDFAEndStatus(int i); //判断节点序号是否在DFA终止节点集合中
    void mergeTwoNode(int a, int b); //合并两个节点

    void generateCCode(MyGraph myGraph); //生成c语言词法分析程序
};

#endif // MYLEX_H
