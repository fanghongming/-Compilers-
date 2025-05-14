#include<bits/stdc++.h>
using namespace std;
string epsilon="\t";

struct node
{
    string name;
    bool ed;
    node()
    {

    }
    node(string _name,bool _ed)
    {
        name=_name;
        ed=_ed; 
    }
    bool operator<(const node&b)const 
    {
        return name<b.name;
    }
    bool operator==(const node&b)const 
    {
        return name==b.name&&ed==b.ed;
    }
    bool operator!=(const node&b)const 
    {
        return !(*this==b);
    }
};
node empty("#",1);
set<node>V;
map<node,set<vector<node>>>G;
map<node,set<node>>FIRST;
map<node,set<node>>FOLLOW;
map<pair<node,vector<node>>,set<node>>SELECT;
map<pair<node,node>,pair<node,vector<node>>>M;

set<node>operator-(set<node>a,node b)
{
    a.erase(b);
    return a;
}
set<node>operator+(set<node>a,set<node>b)
{
    a.insert(b.begin(),b.end());
    return a;
}
set<node>operator+(set<node>a,node  b)
{
    a.insert(b);
    return a;
}
ostream &operator<<(ostream&w,node b)
{
    w<<b.name;
    return w;
}
ostream&operator<<(ostream&w,vector<node>b)
{
        for(auto t:b)
        w<<t<<" ";
        return w;
}
ostream&operator<<(ostream&w,pair<node,vector<node>>t)
{
        w<<t.first<<"->"<<t.second<<" ";
        return w;
}
void First()
{
    for(auto t:V)
    {
        if(t.ed)//对于每个终结符a
        {
            FIRST[t].insert(t);
        }
    }
    map<node,set<node>>pre;//之前的first集合
    map<node,set<node>>now;

    while(1)
    {
        pre=FIRST;

        for(auto g:G)
        {
            node A=g.first;
            for(auto X:g.second)
            {   
                //X是一个数组
                bool has_empty=true;
                
                for(int i=0;i<X.size();i++)
                {
                    FIRST[A]=FIRST[A]+(FIRST[X[i]]-empty);
                    if(!FIRST[X[i]].count(empty))
                    {
                        has_empty=false;
                        break;
                    }
                }
                if(has_empty)
                {
                    FIRST[A]=FIRST[A]+empty;
                }
            }
        }
        if(pre==FIRST)
        {
            break;
        }
    }
}
void Follow(node start)
{
    FOLLOW[start].insert(empty);
    map<node,set<node>>pre;
    while(1)
    {
        pre=FOLLOW;
        for(auto g:G)
        {
            node A=g.first;
            for(auto X:g.second)
            {
                for(int i=0;i<X.size();i++)
                {
                    if(X[i].ed==true)
                    {

                    }
                    else{
                        int has_empty=true;
                        for(int j=i+1;j<X.size();j++)
                        {
                            FOLLOW[X[i]]=FOLLOW[X[i]]+FIRST[X[j]];
                            if(!FIRST[X[j]].count(empty))
                            {
                                has_empty=false;
                                break;
                            }
                        }
                        if(has_empty)
                        {
                            FOLLOW[X[i]]=FOLLOW[X[i]]+empty+FOLLOW[A];
                        }
                    }
                }
            }
        }
        if(pre==FOLLOW)break;
    }
}
void add(node A,vector<node>b)
{
    V.insert(A);
    for(auto t:b)
    V.insert(t);

    G[A].insert(b);
}
set<node>select(node A,vector<node>X)
{
    set<node>res;
    int has_empty=true;
    for(int i=0;i<X.size();i++)
    {
        res=(res+FIRST[X[i]]);
        if(!FIRST[X[i]].count(empty))
        {
            has_empty=false;
            break;
        }
    }
    if(has_empty)
    {
        res=res-empty+FOLLOW[A];
        return res;
    }
    else{
        return res-empty;
    }
}
set<node>cap(set<node>A,set<node>B)
{
    set<node>res;
    for(auto t:A){
        if(B.count(t))
        {
            res.insert(t);
        }
    }
    return res;
}
void LL(node start,vector<node>w)
{
    stack<node>stk;
    stk.push(start);
    int ip=0;
    while(stk.size())
    {
        node X=stk.top();
        node a;
        if(ip>=w.size())
        {

            a=empty;
        }
        else a=w[ip];
     
        if(stk.top()==a)
        {
            stk.pop();
            ip++;
            cout<<"匹配"<<"\n";
        }
        else if(stk.top().ed)
        {
            cout<<"error\n";
            return;
        }
        else if(!M.count(pair<node,node>{stk.top(),a}))
        {
             cout<<"error\n";
            return;
        }
        else {
            auto Y=M[{X,a}].second;
            cout<<"输出产生式"<<X<<"->"<<Y<<"\n";
            stk.pop();
            for(int i=Y.size()-1;i>=0;i--)
            {
                if(Y[i]!=empty)
                stk.push(Y[i]);
            }
        }
    }
}
signed main()
{
    node E("E",false);
    node T("T",false);
    node E_("E'",false);
    node plus("+",true);
    node F("F",false);
    node T_("T'",false);
    node mul("*",true);
    node L("(",true);
    node R(")",true);
    node id("id",true);

    add(E,{T,E_});//这里出现了命名冲突
    add(E_,{plus,T,E_});
    add(E_,{empty});
    add(T,{F,T_});
    add(T_,{mul,F,T_});
    add(T_,{empty});
    add(F,{L,E,R});
    add(F,{id});

    First();
    for(auto w:FIRST)
    {
        auto A=w.first;
        cout<<A<<":";
        for(auto f:w.second)
        {
            cout<<f<<" ";
        }
        cout<<"\n";
    }
    cout<<"\n";
    Follow(E);
     for(auto w:FOLLOW)
    {
        auto A=w.first;
        cout<<A<<":";
        for(auto f:w.second)
        {
            cout<<f<<" ";
        }
        cout<<"\n";
    }
    cout<<"\n";
    cout<<"\n";
    for(auto g:G)
    {
        node A=g.first;
        for(auto X:g.second)
        {
            SELECT[{A,X}]=select(A,X);
            cout<<A<<"->"<<X<<"    ";
            for(auto t:SELECT[{A,X}])
            cout<<t<<" ";
            cout<<"\n";
        }
    }
    for(auto g:G)
    {
        node A=g.first;
        for(auto X1:g.second)
        {
            for(auto X2:g.second)
            {
                if(X1==X2)
                continue;
                auto w=cap(SELECT[{A,X1}],SELECT[{A,X2}]);
                if(w.size()==0)
                {

                }
                else {
                    cout<<"NO\n";
                    return 0;
                }
            }
        }
    }
    for(auto g:G)
    {
        node A=g.first;
        for(auto X:g.second)
        {
            for(auto t:SELECT[{A,X}])
            {
                 M[pair<node,node>{A,t}]=pair<node,vector<node>>{A,X};
            }
        }
    }
    vector<node>p={id,plus,id,mul,id};
    LL(E,p);
}   