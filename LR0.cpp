#include<bits/stdc++.h>
using namespace std;
//set中一定要写好排序顺序。
struct node
{
    string name;
    bool ed;
    node(){
        name="";
        ed=0;
    }
    node(string _name,bool _ed)
    {
        name=_name;
        ed=_ed;
    }
    bool operator<(const node&b)const{
        //比较顺序是有问题的，不能这么任性。

       if(name==b.name)
       {
        return ed<b.ed;
       }
       return name<b.name;
    }
    bool operator==(const node &b)const 
    {
        return name==b.name&&ed==b.ed;
    }
    bool operator!=(const node&b)const 
    {
        return !(*this==b);
    }
};
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
set<node>operator+(set<node>a,node b)
{
    a.insert(b);
    return a;
}
set<node>cap(set<node>A,set<node>B)
{
    set<node>res;
    for(auto t:A)
    {
        if(B.count(t))
        res.insert(t);
    }
    return res;
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
node empty("#",1);
//node ed("$",1);
struct project
{
    node A;
    vector<node>X;
    int id;//在X的某个下标前出现这个位置.

    project()
    {
        A=empty;
        id=0;
    }

    project(node _A,vector<node>_X,int _id)
    {
        A=_A;
        X=_X;
        id=_id;
    }
    bool operator<(const project&T)const 
    {
        if(A==T.A)
        {
            if(id==T.id)
            {
                return X<T.X;
            }
            else return id<T.id;
        }
        else 
        {
            return A<T.A;
        }
    }
    bool operator==(const project&T)const 
    {
        return (A==T.A)&&(X==T.X)&&(id==T.id);
    }
    bool operator!=(const project&T)const 
    {
        return !(T==*this);
    }
};
project start;
ostream&operator<<(ostream&b,project t)
{
    b<<t.A<<"->";
    for(int i=0;i<t.id;i++)
    {
        b<<t.X[i];
    }
    if(t.id!=t.X.size())
    {
        cout<<"*";
    }
    for(int i=t.id;i<t.X.size();i++)
    b<<t.X[i];
    if(t.id==t.X.size())
    {
        b<<"*";
    }
    return b;
}
set<node>V;
map<node,set<vector<node>>>G;
map<node,set<project>>P;
map<node,set<node>>FIRST;
map<node,set<node>>FOLLOW;
void add(node A,vector<node>b)
{
    V.insert(A);
    for(auto t:b)V.insert(t);

    G[A].insert(b);
    for(int i=0;i<=b.size();i++)
    {
        P[A].insert(project(A,b,i));
    }
    //cout<<P[A].size()<<"\n";
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
set<project>closure(set<project>I)
{
    // if(I.begin()->X[0].name=="E")
    // {
    //     cout<<1<<"\n";
    // }
    set<project>res;
    res.insert(I.begin(),I.end());
    queue<project>q;
    for(auto t:I)
    q.push(t);

    while(q.size())
    {
        auto t=q.front();
        q.pop();
        node A=t.A;
        auto  X=t.X;
        int id=t.id;
        if(id==X.size())continue;
        node Next=X[id];
        //cout<<Next.name<<"\n";
        //cout<<P[Next].size()<<"\n";
        for(auto w:P[Next])
        {
            //cout<<Next<<" "<<w<<"\n";
            if(w.id==0)
            {
                //cout<<res.count(w)<<"\n";
            if(!res.count(w))
                {
                     res.insert(w);
                    q.push(w);
                }
              
            }
        }
    }
    return res;
}
set<project>move(set<project>I,node X)
{
    set<project>res;
    set<project>J;
    for(auto t:I)
    {
        if(t.id==t.X.size())
        {
            continue;
        }
        if(t.X[t.id]==X)
        {
            J.insert(project(t.A,t.X,t.id+1));
        }
    }
    res=closure(J);
    return res;
}
map<set<project>,set<set<project>>>C;
void Item()
{
    C[closure({start})];
   map<set<project>,set<set<project>>>pre;
    while(1)
    {
        pre=C;
        for(auto [I,tmp]:C)
        {
            for(auto A:V)
            {
                if(A==empty)continue;
                //  if(A.name=="E")
                // {
                //     cout<<1<<"\n";
                // }
                auto w=move(I,A);
               
                if(C.count(w))continue;
                else 
                {
                    C[I].insert(w);
                    C[w];
                }
            }
        }
        if(pre==C)break;
    }
}
struct  Action{
    bool accept=false;
    bool error=false;
    
    bool shift=false;
    set<project>to;

    bool reduce=false;
    project r;
};

map<pair<set<project>,node>,Action>action;

void init()
{
    for(auto [state,tmp]:C)
    {
        for(auto v:V)
        {
            if(v.ed)
            {
                Action q;
                for(auto p:state)
                {
                    if(p.id==p.X.size())
                    {
                        // if(v=)//如果是最后一位
                        // {
                        //     if(!FOLLOW[p.A].count(empty))
                        //     {
                        //         continue;
                        //     }
                        // }
                        // else
                            if(!FOLLOW[p.A].count(v))
                                continue;
                        if(q.reduce)
                        {
                            cout<<"归约冲突\n";
                            return ;
                        }
                        q.reduce=true;
                        q.r=p;
                    }
                    else 
                    {
                        if(p.X[p.id]==v)
                        {
                            q.shift=true;
                            q.to=move(state,v);
                        }
                    }
                    project tmp=start;
                    tmp.id++;
                    if(p==tmp&&v==empty)
                    {
                        q.accept=true;
                    }
                   
                }
                 if(q.reduce&&q.shift&&(!q.accept))
                    {
                        for(auto t:state)
                        cout<<t<<"\n";
                        cout<<v<<"\n";
                
                        cout<<"移入-规约冲突\n";
                        q.reduce=false;
                       // return ;
                    }
                    if(!(q.accept||q.reduce||q.shift))
                    {
                        q.error=true;
                    }
                    if(q.accept)
                    {
                        q.reduce=false;
                    }
                    action[{state,v}]=q;
            }
            else 
            {
                        Action q;
                        q.to=move(state,v);
                       
                        if(q.to.size()==0)
                        {
                            q.error=true;
                        }
                        action[{state,v}]=q;
            }
        }
    }
}
void LR(vector<node>w)
{
    stack<set<project>>s;
    s.push(closure({start}));
    w.push_back(empty);

    while(1)
    {
        cout<<s.size()<<"\n";
        auto state=s.top();
        //s.pop();
        auto current=w.front();
        Action act=action[{state,current}];
        if(!(act.accept||act.error||act.reduce||act.shift))
        {
            cout<<"出错\n";
            return ;
        }
        if(act.shift)
        {
            s.push(act.to);
            w.erase(w.begin());
        }
        else if(act.reduce) 
        {
            int sz=act.r.X.size();
            while(sz--)s.pop();
            set<project> t=s.top();
            s.push(move(t,act.r.A));
            cout<<act.r<<"\n";
        }
        else if(act.accept)
        {
            cout<<"YES\n";
            return;
        }
        else 
        {
            cout<<"ERROR\n";
            return;
        }
    }

}
signed main()
{
    // node S_("S'",0);
    // node S("S",0);
    // node B("B",0);
    // node b("b",1);
    // node CC("C",0);
    // node a("a",1);
    // node c("c",1);
    // node d("d",1);

    // add(S_,{S});
    // add(S,{a,B});
    // add(B,{b,CC});
    // add(CC,{c});
    // add(S,{d});
    V.insert(empty);
     node E("E",false);
    node T("T",false);
    node E_("E'",false);
    node plus("+",true);
    node F("F",false);
    node T_("T'",false);
    node mul(".",true);
    node L("(",true);
    node R(")",true);
    node id("id",true);

    add(E,{E,plus,T});
    add(T,{F});
    add(E,{T});
    add(F,{L,E,R});
    add(T,{T,mul,F});
    add(F,{id});
    add(E_,{E});
    start=project(E_,{E},0);
    //closure({start});
    First();
    Follow(E_);
     Item();
    for(auto t:C)
    {
        for(auto w:t.first)cout<<w<<"\n";
        cout<<"\n\n";
    }
    cout<<C.size()<<"\n";
    init();
    vector<node>w={id,mul,id,plus,id};


    LR(w);
    for(auto t:FOLLOW)
    {
        cout<<t.first<<"\n";
        for(auto tt:t.second)cout<<tt<<" ";
        cout<<"\n";
    }
}