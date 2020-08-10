#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <utility>
#include <iomanip>
#include "SLR1.h"

using namespace std;

//DFA的边
struct GOTO
{
    int from;
    int to;
    string path;

    GOTO(int s,string p,int t)
    {
        from=s;
        path=p;
        to=t;
    }
};

//DFA中状态
struct State
{
    int id;//状态编号
    set<Item>items;//项目集
};

/*一些操作符的重载*/
bool operator <(const State &x,const State &y)
{

    return x.id<y.id;
}

bool operator <(const Item &x,const Item &y)
{

    return x.id<y.id;
}

bool operator <(const GOTO &x,const GOTO &y)
{

    return x.from<y.from;
}

bool operator ==(const GOTO &x,const GOTO &y)
{
    return x.from==y.from && x.path==y.path && x.to==y.to;
}

bool operator ==(const set<Item> &x,const set<Item> &y)
{
    auto it1=x.begin();
    auto it2=y.begin();

    for(; it1!=x.end(),it2!=y.end(); it1++,it2++)
    {
        Item a=*it1;
        Item b=*it2;
        if(a==b)
            continue;

        //有一个项目不相等，两项目集一定不相等
        else
            return false;
    }
    return true;
}

class SLR
{
private:
    int number=0;
    vector<string>T;//终结符号集合
    vector<string>NT;//非终结符号集合
    string S;//开始符号
    map<string,vector<string>>production;//产生式
    map<string,int>numPro;//编号的产生式集合，用于规约
    set<State>States;//状态集合
    vector<GOTO>GO;//转换函数
    map<string,set<string>>FIRST;//FIRST集
    map<string,set<string>>FOLLOW;//FOLLOW集
    map<pair<int,string>,string>actionTable;//action表
    map<pair<int,string>,int>gotoTable;//goto表

    //读取文法规则
    void readGrammar(string fileText)
    {
        char * strc = new char[strlen(fileText.c_str())+1];
        strcpy(strc,fileText.c_str());
        char* q = strtok(strc,"\n");

        while(q != NULL){

            int i;
            string line = string(q);

            //读取左部
            string left="";
            for(i=0; line[i]!='-'&&i<line.size(); i++)
            {
                left+=line[i];
            }

            NT.push_back(left);//左部加入非终结符号集

            //读取右部
            string right=line.substr(i+2,line.size()-i);//获取产生式右部
            addP(left,right);//添加产生式

            q = strtok(NULL,"\n");
        }
        addT();//添加终极符
        S=*NT.begin();
        //input.close();
    }

    //填产生式
    void addP(string left,string right)
    {
        right+="#";//'#'作为每句文法结尾标志
        string pRight="";
        for(int i=0; i<right.size(); i++)
        {
            if(right[i]=='|'||right[i]=='#')
            {
                production[left].push_back(pRight);
                pRight="";
            }
            else
            {
                pRight+=right[i];
            }
        }
    }

    //带标号的产生式集
    void addNumP()
    {
        int i=0;
        for(string left:NT)
        {
            for(string right:production[left])
            {
                numPro[left+"->"+right]=i;
                i++;
            }
        }

//        for(auto it=numPro.begin();it!=numPro.end();it++)
//            cout<<it->first<<"\t"<<it->second<<endl;
    }

    //填终结符
    void addT()
    {
        string temp="";
        for(string left:NT)
        {
            for(string right:production[left])
            {
                right+="#";
                for(int i=0; i<right.size(); i++)
                {
                    if(right[i]=='|'||right[i]==' '||right[i]=='#')
                    {
                        //不是非终结，且不是空，则加入终结符号
                        if((find(NT.begin(),NT.end(),temp)==NT.end())&&temp!="@")
                        {
                            T.push_back(temp);
                        }
                        temp="";
                    }
                    else
                    {
                        temp+=right[i];
                    }
                }
            }
        }//end left

        //终结符去重
        sort(T.begin(),T.end());
        T.erase(unique(T.begin(), T.end()), T.end());
    }
    //判断是否是非终极符号
    bool isNT(string token)
    {
        if(find(NT.begin(),NT.end(),token)!=NT.end())
            return true;
        return false;
    }
    //判断temp在不在集合c中
    bool isIn(Item temp,set<Item>c)
    {
        for(Item i:c)
        {
            if(i.getItem()==temp.getItem())
                return true;
        }
        return false;
    }
    //判断是否应该规约
    string tableReduce(int num)
    {
        for(State s:States)
        {
            //目标状态
            if(s.id==num)
            {
                //遍历项目集
                for(Item i:s.items)
                {
                    //还有下一个点，肯定不是规约项目
                    if(i.hasNextDot())
                        return "";
                    //是规约项目
                    else
                        return i.getLeft();//返回左部NT
                }
            }
        }
        return "";
    }

    //找到item规约到的产生式，返回其编号
    int findReduce(int num)
    {
        for(State s:States)
        {
            if(s.id==num)
            {
                for(Item i:s.items)
                {
                    string temp=i.getItem();
                    temp.erase(temp.find("."));
                    temp.pop_back();
                    return numPro.find(temp)->second;
                }
            }
        }
        return -1;
    }

    //找到产生式序号为pro的产生式右部数量
    int rightCount(string &left,int pro)
    {
        for(auto it=numPro.begin(); it!=numPro.end(); it++)
        {
            if(it->second==pro)
            {
                cout<<it->first<<endl;
                string target=it->first;
                left=target.substr(0,target.find("->"));
                string right=target.substr(target.find("->")+2);
                vector<string>temp=split(right," ");
                return temp.size();
            }
        }
        return 0;
    }
public:
    //构造函数，读入所需的四元组
    SLR(string fileName)
    {
        readGrammar(fileName);
        Extension();//拓广文法
    }

    //拓广文法
    void Extension()
    {
        string newS=S;
        newS+="'";
        NT.insert(NT.begin(),newS);
        production[newS].push_back(S);
        S=newS;
    }

    //状态集是否已经包含该状态
    int hasState(set<Item>J)
    {
        for(State s:States)
        {
            if(s.items.size()!=J.size())
                continue;

            if(s.items==J)
                return s.id;
            else
                continue;

        }
        return -1;
    }
    //获得FIRST集合
    void getFirst()
    {
        FIRST.clear();

        //终结符号或@
        FIRST["@"].insert("@");
        for(string X:T)
        {
            FIRST[X].insert(X);
        }

        //非终结符号
        int j=0;
        while(j<4)
        {
            for(int i=0; i<NT.size(); i++)
            {
                string A=NT[i];

                //遍历A的每个产生式
                for(int k=0; k<production[A].size(); k++)
                {
                    int Continue=1;//是否添加@
                    string right=production[A][k];

                    //X是每条产生式第一个token
                    string X;
                    if(right.find(" ")==string::npos)
                        X=right;
                    else
                        X=right.substr(0,right.find(" "));
                    //cout<<A<<"\t"<<X<<endl;

                    //FIRST[A]=FIRST[X]-@
                    if(!FIRST[X].empty())
                    {
                        for(string firstX:FIRST[X])
                        {
                            if(firstX=="@")
                                continue;
                            else
                            {
                                FIRST[A].insert(firstX);
                                Continue=0;
                            }
                        }
                        if(Continue)
                            FIRST[A].insert("@");
                    }
                }

            }
            j++;
        }
    }

    //获得FOLLOW集合
    void getFollow()
    {
        getFirst();

        //将界符加入开始符号的follow集
        FOLLOW[S].insert("$");

        int j=0;
        while(j<4)
        {
            //遍历非终结符号
            for(string A:NT)
            {
                for(string right:production[A])
                {
                    for(string B:NT)
                    {
                        //A->Bb
                        if(right.find(B)!=string::npos)
                        {
                            /*找B后的字符b*/
                            string b;
							int flag = 0;
                            //识别到E'
                            if(right[right.find(B)+B.size()]!=' '&&right[right.find(B)+B.size()]!='\0')
                            {
                                string s=right.substr(right.find(B));//E'b
                                string temp=right.substr(right.find(B)+B.size());//' b

                                //A->E'
                                if(temp.find(" ")==string::npos)
                                {
                                    B=s;//B:E->E'
                                    FOLLOW[B].insert(FOLLOW[A].begin(),FOLLOW[A].end());//左部的FOLLOW赋给B
                                    flag=1;
                                }
                                //A->E'b
                                else
                                {
                                    B=s.substr(0,s.find(" "));
                                    temp=temp.substr(temp.find(" ")+1);//b

                                    //b后无字符
                                    if(temp.find(" ")==string::npos)
                                        b=temp;
                                    //b后有字符
                                    else
                                        b=temp.substr(0,temp.find(" "));
                                }
                            }

                            //A->aEb
                            else if(right[right.find(B)+B.size()]==' ')
                            {
                                string temp=right.substr(right.find(B)+B.size()+1);//B后的子串

                                //b后无字符
                                if(temp.find(" ")==string::npos)
                                    b=temp;
                                //b后有字符
                                else
                                    b=temp.substr(0,temp.find(" "));
                            }
                            //A->aE
                            else
                            {
                                FOLLOW[B].insert(FOLLOW[A].begin(),FOLLOW[A].end());
                                flag=1;
                            }

                            //FOLLOW[B]还没求到
                            if(flag==0)
                            {
                                //FIRST[b]中不包含@
                                if(FIRST[b].find("@")==FIRST[b].end())
                                {
                                    FOLLOW[B].insert(FIRST[b].begin(),FIRST[b].end());
                                }
                                else
                                {
                                    for(string follow:FIRST[b])
                                    {
                                        if(follow=="@")
                                            continue;
                                        else
                                            FOLLOW[B].insert(follow);
                                    }
                                    FOLLOW[B].insert(FOLLOW[A].begin(),FOLLOW[A].end());
                                }
                            }
                        }
                    }
                }
            }
            j++;
        }
        //printFOLLOW();
    }

    //项目闭包
    set<Item> closure(Item item)
    {
        set<Item>C;//项目闭包
        C.insert(item);

        queue<Item>bfs;//bfs求所有闭包项
        bfs.push(item);

        while(!bfs.empty())
        {
            Item now=bfs.front();
            bfs.pop();

            vector<string>buffer=split(now.getRight(),".");
            if(buffer.size()>1)
            {
                string first=first_Word(buffer[1].erase(0,1));

                if(isNT(first))//如果"."后面第一个字符是NT
                {
                    for(auto it2=production[first].begin(); it2!=production[first].end(); it2++)
                    {
                        Item temp(first,*it2);

                        if(!isIn(temp,C))
                        {
                            C.insert(temp);
                            bfs.push(temp);
                        }
                    }
                }
            }
        }
        return C;
    }

    //构造DFA
    void dfa()
    {
        State s0;//初始项目集
        s0.id=number++;//状态序号

        //初始项目集
        string firstRight=*(production[S].begin());
        Item start(S,firstRight);
        s0.items=closure(start);//加到状态中
        States.insert(s0);

        //构建DFA
        State temp;
        for(State s:States)
        {
            map<string,int>Paths;//路径
            for(Item now:s.items)
            {
                now.getItem();
                if(now.hasNextDot())
                {
                    string path=now.getPath();//path
                    Item nextD(now.getLeft(),now.nextDot());
                    set<Item>next=closure(nextD);//to


                    //该状态已经有这条路径了，则将新产生的闭包添加到原有目的状态中
                    int oldDes;
                    if(Paths.find(path)!=Paths.end())
                    {
                        oldDes=Paths.find(path)->second;

                        for(State dest:States)
                        {
                            if(dest.id==oldDes)
                            {
                                dest.items.insert(next.begin(),next.end());
                                next=dest.items;

                                //更新状态集中状态
                                //set不允许重复插入，因此只能删除再插
                                States.erase(dest);
                                States.insert(dest);


                                int tID=hasState(next);
                                if(tID!=-1)
                                {
                                    //temp=dest;
                                    for(int i=0; i<GO.size(); i++)
                                    {
                                        if(GO[i].to==oldDes)
                                        {
                                            GO[i].to=tID;
                                            //cout<<tID<<endl;
                                        }
                                    }
                                }
                            }

                        }
                    }

                    //如果该目的状态在状态集里没有出现过，就加入状态集
                    int tID=hasState(next);
                    if(tID==-1)
                    {
                        State t;
                        t.id=number++;
                        t.items=next;
                        States.insert(t);
                        Paths.insert(pair<string,int>(path,t.id));
                        GO.push_back(GOTO(s.id,path,t.id));
                    }
                    //该目的状态已经在状态集中了
                    else
                    {
                        Paths.insert(pair<string,int>(path,tID));
                        GO.push_back(GOTO(s.id,path,tID));
                    }
                }
            }
        }
        //删除重复GOTO
        sort(GO.begin(),GO.end());
        GO.erase(unique(GO.begin(), GO.end()), GO.end());

        //处理重复状态
        for(State s1:States)
        {
            for(State s2:States)
            {
                //发现重复状态集
                if(s2.id>s1.id&&s1.items==s2.items)
                {
                    int erase_id=s2.id;
                    States.erase(s2);

                    //重复状态后面的所有状态序号-1
                    for(State s:States)
                    {
                        if(s.id>erase_id)
                        {
                            //原地修改set！
                            State &newS=const_cast<State&>(*States.find(s));
                            newS.id--;
                        }
                    }
                    //状态转移函数
                    for(int i=0; i<GO.size(); i++)
                    {
                        if(GO[i].from==erase_id||GO[i].to==erase_id)
                            GO.erase(find(GO.begin(),GO.end(),GO[i]));
                        if(GO[i].from>erase_id)
                            GO[i].from--;
                        if(GO[i].to>erase_id)
                            GO[i].to--;
                    }
                }
            }
        }
        //printS();
        //printGO();
    }

    //构造SLR(1)分析表
    void getTable()
    {
        addNumP();
        string s=S;
        s=s.erase(s.find("'"));

        pair<int,string>title(1,"$");
        actionTable[title]="acc";

        for(GOTO go:GO)
        {
            //目的地是NT
            if(isNT(go.path))
            {
                pair<int,string>title(go.from,go.path);
                gotoTable[title]=go.to;

            }
            //加入action表
            else
            {
                //shift
                pair<int,string>title(go.from,go.path);
                actionTable[title]="s"+to_string(go.to);
            }

            //reduce
            string rNT=tableReduce(go.to);
            if(rNT!="")
            {
                if(go.path!=s)
                {
                    vector<string>x=T;
                    x.push_back("$");

                    for(string p:x)
                    {
                        set<string>follow=FOLLOW[rNT];
                        if(follow.find(p)!=follow.end())
                        {
                            pair<int,string>title(go.to,p);
                            actionTable[title]="r"+to_string(findReduce(go.to));
                        }
                    }
                }
            }
        }
        //printTable();
    }

    //语法分析过程
    int parsing(string input, string& str)
    {
        stack<string>Analysis;
        input+=" $";

        //0状态入栈
        Analysis.push("$");
        Analysis.push("0");

        vector<string>w=split(input," ");//将输入串分成一个个词
        int ip=0;//输入串的指针

        str += input + "\n\n";

        while(true)
        {
            pair<int,string>title(stoi(Analysis.top()),w[ip]); //stoi函数用于string to int
            string res=actionTable[title];
            if(stoi(Analysis.top()) < 10){
                str += " s" + Analysis.top() + "  当前符号: "+ w[ip] + "    ";
            }
            else
                str += "s" + Analysis.top() + "  当前符号: "+ w[ip] + "    ";

            //shift
            if(res[0]=='s')
            {
                str +=" 移进\n";
                int state=stoi(res.substr(1));
                Analysis.push(w[ip]);
                Analysis.push(to_string(state));
                ip++;
            }

            //reduce
            else if(res[0]=='r')
            {
                str += " 归约\n";
                int pro=stoi(res.substr(1));
                string left;//产生式左部
                int b=2*rightCount(left,pro);//2倍的产生式右部符号数量

                while(b>0)
                {
                    Analysis.pop();
                    b--;
                }

                int s1=stoi(Analysis.top());
                Analysis.push(left);

                pair<int,string>t(s1,left);
                Analysis.push(to_string(gotoTable[t]));
            }

            else if(res[0]=='a')
            {
               str += " 接受\n\n";
                return 1;
            }

            else
            {
                str += "语法错误！\n\n";
                return 0;
            }
        }
    }

    string parser(string sentence)
    {
        if(sentence == ""){
            return "error";
        }

        string program = sentence;
        string str;

        if(parsing(program, str))
            str += program + "属于该文法";
        else
            str += program + "不属于该文法";

        return str;
    }

    /*=====================打印===========================*/
    //打印NT和T
    void printV()
    {
        cout<<"非终结符号集合："<<endl;
        for(int i=0; i<NT.size(); i++)
        {
            cout<<NT[i]<<" ";
        }
        cout<<endl;
        cout<<"终结符号集合："<<endl;
        for(int i=0; i<T.size(); i++)
        {
            cout<<T[i]<<" ";
        }
        cout<<endl;
    }

    //打印FOLLOW集
    void printFOLLOW()
    {
        cout<<"FOLLOW:"<<endl;
        cout.setf(ios::left);
        for(string non_terminal:NT)
        {
            cout<<setw(20)<<non_terminal;
            for(string follow:FOLLOW[non_terminal])
                cout<<follow<<" ";
            cout<<endl;
        }
        cout<<endl;
    }

    //打印分析表
    string printTable()
    {
        string str;
        //str += "*****SLR(1)分析表*****\n";

        vector<string>x=T;//含界符的终结符号集合
        x.push_back("$");

        //输出表格横轴
        str += "---------------输入---------------\n\n";
        //cout.setf(ios::left);

        for (auto it1 = x.begin(); it1 != x.end(); it1++)
        {
            if (it1==x.begin())
                str += "     ";
            str += "      " + *it1;
        }
        str += "\n";

        for(int i=0; i<States.size(); i++)
        {
            if(i < 10){
                str += " "+ to_string(i) + "   ";
            }
            else
                str +=to_string(i) + "   ";

            for(string t:x)
            {      
                if(actionTable.empty() == false)
                {
                    pair<int,string>title(i,t);
                    if(actionTable[title] != ""){
                        string temp = actionTable[title];
                        if(strlen(temp.c_str()) >= 3){
                            str += "  | " + actionTable[title];
                        }
                        else
                            str += "  |  " + actionTable[title];
                    }
                    else{
                        str += "  |    " ;
                        //cout<<str<<endl;
                    }

                }


            }
            str += "\n";
        }
        str += "\n";

        /*打印GOTO表*/
        vector<string>y=NT;//不含S’的非终结符号集合
        y.erase(y.begin());

        str += "---------------goto---------------\n\n";
        //cout.setf(ios::left);

        for (auto it1 = y.begin(); it1 != y.end(); it1++)
        {
            if(it1==y.begin())
                str += "     ";

            str += "     " + *it1;
        }
        str += "\n";

        for(int i=0; i<States.size(); i++)
        {
            if(i < 10){
                str += " "+ to_string(i) + "   ";
            }
            else
                str += to_string(i) + "   ";

            for(string t:y)
            {
                pair<int,string>title(i,t);

                if(gotoTable[title]!=0)
                {
                    int num = gotoTable[title];
                    if(num >= 10){
                        str += "  | " + to_string(num);
                    }
                    else
                        str += "  |  " + to_string(num);

                }
                else
                    str += "  |   ";
            }
            str += "\n";
        }

        return str;
        //cout<<endl<<"LR分析表构建完成"<<endl<<endl;
    }


    //生成产生式
    string genprintP()
    {
        string garstr;
        for(string left:NT)
        {
            garstr += left + "->";
            for(auto it=production[left].begin(); it!=production[left].end(); it++)
            {
                if(it!=production[left].end()-1)
                    garstr += *it + "|";
                else
                    garstr += *it + "\n";
            }
        }
        garstr += "\n";

        return garstr;
    }

    //状态表
    string printS()
    {
        string str;
        str += "------LR(0)DFA图状态------\n\n";
        for(State s:States)
        {
            str += to_string(s.id) + "\n";
            str += printI(s.items);
        }
        str += "\n";

        return str;
    }

    //状态转移函数
    string printGO()
    {
        string str;
        str += "-----转移条件及下一状态-----\n\n";
        for(GOTO go:GO)
        {
            str += to_string(go.from) + "---" + go.path+ "-->" + to_string(go.to) + "\n";
        }
        str += "\n";

        return str;
    }

    //项目集
    string printI(set<Item>I)
    {
        //cout<<"The project set of SLR(1)"<<endl;
        string str;
        str += "---------------\n";
        for(Item i:I)
        {
            str += i.getItem() + "\n";
        }
        str += "---------------\n";
        str += "\n";

        return str;
    }
};


