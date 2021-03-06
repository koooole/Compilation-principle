#include "mygraph.h"
#include "mylex.h"
using namespace std;

//默认构造函数,初始边和顶点数目都为0
MyGraph::MyGraph():mVexNum(0),mEdgeNum(0){

    for(int i =0;i<100;i++){
        for(int j =0;j<100;j++){
            mMatrix[i][j].push_back('^') ;
        }
    }
}


//增加一条边 a：节点名称、b：另一个节点名称、edgeCondition ：边上的值，即转换条件
void MyGraph::addEdge(int a, int b, char edgeCondition){
    if (edgeCondition == '^'){
        cout << "获取连接边("<<a << "," << b <<")值错误" << endl;
    }else{
        cout << "连接边的值为" << edgeCondition << endl;
    }
    if (mMatrix[a][b].at(0) == '^'){
        mMatrix[a][b].clear();
    }
    bool flag = true;
    for (int i = 0; i < mMatrix[a][b].size() ; ++i) {
        if (mMatrix[a][b].at(i) == edgeCondition){
            flag = false;
            break;
        }
    }
    if (flag){
        mMatrix[a][b].push_back(edgeCondition);
        cout << "[" << a << "," << b << "]" << "连接" << endl;
    }else{
        cout << "[" << a << "," << b << "]" << "已经连接（重复连接）" << endl;
    }
}

//删除某条边
void MyGraph::deleteEdge(int a, int b){
    mMatrix[a][b].clear();
    mMatrix[a][b].push_back('^');
    cout << "[" << a << "," << b << "]" << "断开连接" << endl;
}

void MyGraph::deleteEdge(int a, int b,char condition){
    mMatrix[a][b].clear();
    mMatrix[a][b].push_back('^');
    cout << "[" << a << "," << b << "]" << "断开连接" << endl;
}

//获取边上的值
vector<char> MyGraph::getEdgeValue(int a, int b) {
    return mMatrix[a][b];
}

//打印输出图内容
void MyGraph::printMyGraph() {
    for(int i = 0 ;i<100 ;i ++){
        for(int j = 0;j<100;j++){
            if (getEdgeValue(i,j).at(0) != '^'){
                cout << "状态" << i << "—— " << getEdgeValue(i,j).at(0) << " ——>" << "状态" << j << endl;
            }
        }
    }
}
