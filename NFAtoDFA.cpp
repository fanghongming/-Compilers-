#include<bits/stdc++.h>
#include<random>
using namespace std;
//S ->
using namespace std;

const char EPSILON = '\0';

// 词法单元类型枚举
enum TokenType {
    KEYWORD = 1,
    //关键字
    IDENTIFIER = 2,
    //标识符
    NUMBER = 3,
    //数字,这里只有整数
    OPERATOR = 4,
    //运算符号
    DELIMITER = 5,
    //边界符号。
    ERROR = -1
    //错误
};
struct State
{
    TokenType type;
    int num;
    State()
    {
        type=ERROR;
    }
    State(TokenType _type,int _num):type(_type),num(_num)
    {
        
    }
    bool operator<(const State& b)const 
    {
        return num<b.num;
    }
    bool operator==(const State&b)const {
        return num==b.num;
    }
    bool operator!=(const State&b)const{
        return !(*this==b);
    }
};
ostream&operator<<(ostream&b,State a)
{
    cout<<a.num;
    return b;
}
struct node
    {
        set<State>S;
        int num;
        bool operator<(const node&b)const 
        {
          return S<b.S;
        }
        bool operator==(const node&b)const{
            return S==b.S;
        }
        bool operator!=(const node&b)const 
        {
            return !(*this==b);
        }
    };
    ostream &operator<<(ostream&b,node a)
    {
        //cout<<a.num<< " ";
        for(auto t:a.S)
        {
            cout<<t<<" ";
        }
        return b;
    }
node operator+(node a,node b)
{
    a.S.insert(b.S.begin(),b.S.end());
    return a;
}

class NFA {
public:
    int addState(TokenType _type = ERROR) {
        int state = nextStateId++;
        states[state].type = _type;
        states[state].num=state;
        transitions[states[state]]; // 初始化转移表
        return state;
    }

    void addTransition(int from, char input, int to) {
        transitions[states[from]][input].insert(states[to]);
        C.insert(input);
    }

    void setInitialState(int state) { initial.num = state; }

    // 检查输入字符串并返回匹配的词法单元类型
    TokenType match(const string& input) {
        set<State> currentStates = epsilonClosure({initial});
        TokenType matchedType = ERROR;
        size_t lastAcceptPos = 0;

        for (size_t pos = 0; pos <=input.size(); pos++) {
            // 检查当前状态是否为接受状态
            for (auto s : currentStates) {
                if (s.type != ERROR) {
                    matchedType = s.type;
                    lastAcceptPos = pos;
                }
            }

            if (pos == input.size()) break;

            // 转移至下一个字符
            currentStates = move(currentStates, input[pos]);
            currentStates = epsilonClosure(currentStates);
            if(currentStates.size()==0)return ERROR;
        }
        for(auto t:currentStates)
        {
                if(t.type==KEYWORD)
                return KEYWORD;
        }
        return (lastAcceptPos > 0) ? matchedType : ERROR;
    }
    int nextStateId = 0;
    State initial;
    map<int, State> states;
    map<State, map<char, set<State>>> transitions;
    set<char>C;
    set<State> epsilonClosure(const set<State>& states) //使用迭代法，而不是dfs
    {
        set<State> closure;
        stack<State> stack;
        for (auto s : states) {
            closure.insert(s);
            stack.push(s);
        }
        while (!stack.empty()) {
            auto s = stack.top();
            stack.pop();
            if (transitions[s].count(EPSILON)) {
                for (auto next : transitions[s][EPSILON]) {
                    if (!closure.count(next)) {
                        closure.insert(next);
                        stack.push(next);
                    }
                }
            }
        }
        return closure;
    }

    set<State> move(const set<State>& states, char c) {
        set<State> result;
        for (auto  s : states) {
            if (transitions[s].count(c)) {
                result.insert(transitions[s][c].begin(), transitions[s][c].end());
            }
        }
        return result;
    }
};

struct DFA{
    
    map<node,map<char,node>>G;
    node start;
    set<node>vis;
    int idx;
    set<char>C;
    DFA()
    {

    }
    DFA(NFA nfa)
    {
        C=nfa.C;
        C.erase(EPSILON);
        idx=0;
        set<State>tmp={nfa.initial};
        start.S=nfa.epsilonClosure(tmp);
        start.num=++idx;
        queue<node>q;
        q.push(start);
        while(q.size())
        {
            auto t=q.front();
            q.pop();
            vis.insert(t);
            for(auto c:C)
            {
                node next;
                next.S=nfa.epsilonClosure(nfa.move(t.S,c));
                if(vis.count(next))
                {
                    G[t][c]=(*vis.find(next));
                }
                else {
                    if(next.S.size()!=0)
                    {
                        next.num=++idx;
                        G[t][c]=(next);
                        q.push(next);
                    }
                 
                    
                }
            }
        }
        C.erase(EPSILON);
    }
    TokenType match(const string&input)
    {
        node current=start;
        TokenType matchedType=ERROR;
        size_t lastAcceptPos=0;
        for(size_t pos=0;pos<=input.size();pos++)
        {
            for(auto s:current.S)
            {
                if(s.type!=ERROR)
                {
                    matchedType=s.type;
                    lastAcceptPos=pos;
                }
            }if(pos==input.size())break;

            current=G[current][input[pos]];
            if(current.S.size()==0)return ERROR;
        }
        for(auto t:current.S)
        {
            if(t.type==KEYWORD)return KEYWORD;
        }
        return (lastAcceptPos>0)?matchedType:ERROR;
    }
};
DFA simpy(DFA dfa)
{
    set<set<node>>group;
    // map<set<node>,int>to_id;
    // map<vector<int>,set<node>>div;

    set<node>F;
    set<node>S;
    for(auto t:dfa.G)
    {
        int ed=false;

        for(auto w:t.first.S)
        {
            if(w.type!=ERROR )
            ed=true;
        }
        if(ed)
        {
            F.insert(t.first);
        }
        else 
        {
            S.insert(t.first);
        }
    }

    group.insert(S);
    group.insert(F);


    while(1)
    {
            set<set<node>>pre=group;
            map<node,int>to_id;
            map<vector<int>,set<node>>div;
            int idx=0;
            for(auto t:pre)
            {
                idx++;
                for(auto i:t)
                {
                    to_id[i]=idx;
                }
            }
            for(auto t:pre)
            {
                for(auto st:t)
                {
                    vector<int>now;
                    for(auto c:dfa.C){
                        if(dfa.G[st].count(c))
                        {
                            now.push_back(to_id[dfa.G[st][c]]);
                        }
                        else {
                            now.push_back(0);
                        }
                    }
                    div[now].insert(st);
                }
            }
            group.clear();
            for(auto t:div)
            {
                group.insert(t.second);
            }

            if(pre==group)
            {
                break;
            }
    }
    cout<<group.size()<<"\n";
     map<node,int>to_id;
     map<int,set<node>>to_set;
     map<int,node>newnode;
     int idx=0;
     for(auto t:group)
     {
        idx++;
        set<node>same;
        node cup;
        for(auto w:t)
        {
            to_id[w]=idx;
            same.insert(w);
            cup=cup+w;
        }
        cup.S.insert(State(ERROR,-idx));
        to_set[idx]=same;
        newnode[idx]=cup;
     }

     DFA res;
     res.C=dfa.C;
     res.start=newnode[to_id[dfa.start]];
     //1 2 是一类 3 4 是一类，然后我们简单认为是相同的。
     //1 3       2 4 最后导出来的是一样的。
     //但是，实际上不是一样的。
     //{1 2}{3,4}是一组
     //{13}{2,4}是一组
     //如果不做区分，我们会认为这两个组是同一个节点。
     //做区分的方法就是添加负号，这是一个技巧。

     //想到了一种偷懒的方法，就是转换为NFA
     //或者在每个新的节点上，添加一个-然后是第几组。
     for(auto t:to_set)
     {
        if(t.second.size()==0)continue;
        node cup;
        for(auto tt:t.second)
        cup=cup+tt;
        node it=*t.second.begin();
        for(auto c:res.C)
        {
                if(dfa.G[it].count(c))
                {
                    res.G[cup][c]=newnode[to_id[dfa.G[it][c]]];
                }
                else 
                {
                    //否则，都是空的。
                }
        }
     }
     return res;
}
NFA buildPL0NFA() {
    NFA nfa;
    int initial = nfa.addState();
    nfa.setInitialState(initial);

    // ------------------------- 0. 空白符处理 -------------------------
    int whitespaceState = nfa.addState();
    nfa.addTransition(initial, EPSILON, whitespaceState);
    for (char c : {' ', '\t', '\n', '\r'}) {
        nfa.addTransition(whitespaceState, c, whitespaceState);
    }

    // ------------------------- 1. 关键字 -------------------------
       std::vector<std::string> keywords = {
        "begin", "call", "const", "do", "end", "if", "odd",
        "procedure", "read", "then", "var", "while", "write"
    };//关键字不能被当成标识符号。


    for (const auto& keyword : keywords) {
        int currentState = initial;
        for (size_t i = 0; i < keyword.size(); ++i) {
            int nextState = (i == keyword.size() - 1) ? 
                nfa.addState(KEYWORD) : nfa.addState();
            nfa.addTransition(currentState, keyword[i], nextState);
            currentState = nextState;
        }
        // 关键字后必须是非字母数字字符（通过ε转移实现边界检查）
    }

    // ------------------------- 2. 标识符 -------------------------
    int one=nfa.addState();
    int moreone=nfa.addState();
    int idAccept=nfa.addState(IDENTIFIER);
    for(char c='a';c<='z';c++)
    {
        nfa.addTransition(initial,c,one);
    }
    for(char c='a';c<='z';c++)
    {
        nfa.addTransition(one,c,moreone);
        nfa.addTransition(one,c-'a'+'A',moreone);
        nfa.addTransition(moreone,c,moreone);
        nfa.addTransition(moreone,c-'a'+'A',moreone);
    }

    for(char c='0';c<='9';c++)
    {
        nfa.addTransition(one,c,moreone);
        nfa.addTransition(moreone,c,moreone);
    }
    nfa.addTransition(moreone,EPSILON,idAccept);
    nfa.addTransition(one,EPSILON,idAccept);

    int num=nfa.addState();
    for(char c='0';c<='9';c++)
    {
        nfa.addTransition(initial,c,num);
        nfa.addTransition(num,c,num);
    }
    int numberAccept=nfa.addState(NUMBER);
    nfa.addTransition(num,EPSILON,numberAccept);

    //两个字符的
     std::vector<std::pair<std::string, TokenType>> multiOps = {
        {":=", OPERATOR}, {"<=", OPERATOR}, {">=", OPERATOR}, {"#", OPERATOR}
    };

    for (const auto& op : multiOps) {
        int opStart = nfa.addState();
        nfa.addTransition(initial, EPSILON, opStart);
        int s1 = nfa.addState();
        nfa.addTransition(opStart, op.first[0], s1);
        int opAccept = nfa.addState(OPERATOR);
        nfa.addTransition(s1, op.first[1], opAccept);
    }

    // 单字符运算符（+, -, *, /, <, >, =）
    std::vector<char> singleOps = {'+', '-', '*', '/', '<', '>', '='};
    for (char op : singleOps) {
        int opStart = nfa.addState();
        nfa.addTransition(initial, EPSILON, opStart);
        int opAccept = nfa.addState(OPERATOR);
        nfa.addTransition(opStart, op, opAccept);
    }

    // ------------------------- 5. 界符 -------------------------
    std::vector<char> delimiters = {',', ';', '{', '}', '(', ')', '.'};
    for (char delim : delimiters) {
        int delimStart = nfa.addState();
        nfa.addTransition(initial, EPSILON, delimStart);
        int delimAccept = nfa.addState(DELIMITER);
        nfa.addTransition(delimStart, delim, delimAccept);
    }
    return nfa;
}
vector<pair<TokenType, string>> tokenize(const string& filename, DFA& dfa) {
    vector<pair<TokenType, string>> tokens;
    ifstream file(filename);
    string buffer;
    char c;

    while (file.get(c)) {
        if (isspace(c)) {
            if (!buffer.empty()) {
                // 缓冲区非空时立即结束当前词法单元
                TokenType type = dfa.match(buffer);
                if (type != ERROR) {
                    tokens.emplace_back(type, buffer);
                    buffer.clear();
                } else {
                    cerr << "错误：无法识别的符号 '" << buffer << "'" << endl;
                    buffer.clear();
                }
            }
            continue; // 跳过空白符
        }

        // 处理注释 /* ... */
        if (c == '/' && file.peek() == '*') {
            file.get();
            bool closed = false;
            while (file.get(c)) {
                if (c == '*' && file.peek() == '/') {
                    file.get();
                    closed = true;
                    break;
                }
            }
            if (!closed) cerr << "错误：注释未闭合" << endl;
            continue;
        }

        buffer += c;
        TokenType type = dfa.match(buffer);

        // 检查下一个字符是否为空格，若是则立即结束匹配
        if (isspace(file.peek())) {
            if (type != ERROR) {
                tokens.emplace_back(type, buffer);
                buffer.clear();
            }
        } else {
            // 继续扩展匹配
            while (file.peek() != EOF) {
                char next = file.peek();
               
                string newBuffer = buffer + next;
                TokenType newType = dfa.match(newBuffer);

                if (newType == ERROR) break;

                buffer = newBuffer;
                type = newType;
                file.get();
            }

            if (type != ERROR) {
                tokens.emplace_back(type, buffer);
                buffer.clear();
            } else {
                cerr << "错误：无法识别的符号 '" << buffer << "'" << endl;
                buffer.clear();
            }
        }
    }

    // 处理文件末尾剩余字符
    if (!buffer.empty()) {
        TokenType type = dfa.match(buffer);
        if (type != ERROR) {
            tokens.emplace_back(type, buffer);
        } else {
            cerr << "错误：无法识别的符号 '" << buffer << "'" << endl;
        }
    }
    return tokens;
}
// NFA build()
// {
//     NFA nfa;
//     int q0 = nfa.addState();
//     nfa.setInitialState(q0);
//     int q1=nfa.addState();
//     int q2=nfa.addState(IDENTIFIER);
//     int q3=nfa.addState(IDENTIFIER);
//     nfa.addTransition(q0,EPSILON,q1);
//     nfa.addTransition(q1,'b',q2);
//     nfa.addTransition(q2,EPSILON,q1);
//     nfa.addTransition(q0,'a',q2);
//     nfa.addTransition(q1,'b',q3);
//     nfa.addTransition(q3,EPSILON,q2);
//     return nfa;
// }   
signed main()
{
    // auto t=build();
    // DFA dfa(t);
    // for(auto t:dfa.G)
    // {
    //     cout<<t.first<<"\n";
    //     for(auto c:dfa.C)
    //     {
    //         if(dfa.G[t.first].count(c))
    //         {
    //             cout<<c<<" "<<dfa.G[t.first][c]<<"\n";

    //         }
            
    //     }
    //     cout<<"\n";
    // }
    // dfa=simpy(dfa);
    // for(auto t:dfa.G)
    // {
    //     cout<<t.first<<"\n";
    //     for(auto c:dfa.C)
    //     {
    //         if(dfa.G[t.first].count(c))
    //         {
    //             cout<<c<<" "<<dfa.G[t.first][c]<<"\n";

    //         }
            
    //     }
    //     cout<<"\n";
    // }
    NFA pl0NFA = buildPL0NFA();
    DFA dfa(pl0NFA);
    //dfa=simpy(dfa);
    auto tokens = tokenize("1.txt", dfa);

    freopen("1.out","w+",stdout);
    for (const auto& token : tokens) {
        cout << "(" << token.first << ", '" << token.second << "')" << endl;
    }
    return 0;
}