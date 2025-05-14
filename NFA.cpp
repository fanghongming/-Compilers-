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

class NFA {
public:
    struct State {
        TokenType tokenType = ERROR; // 状态的词法单元类型（默认为错误）
        State()
        {
            tokenType=ERROR;
        }
    };

    int addState(TokenType type = ERROR) {
        int state = nextStateId++;
        states[state].tokenType = type;
        transitions[state]; // 初始化转移表
        return state;
    }

    void addTransition(int from, char input, int to) {
        transitions[from][input].insert(to);
    }

    void setInitialState(int state) { initial = state; }

    // 检查输入字符串并返回匹配的词法单元类型
    TokenType match(const string& input) {
        set<int> currentStates = epsilonClosure({initial});
        TokenType matchedType = ERROR;
        size_t lastAcceptPos = 0;

        for (size_t pos = 0; pos <=input.size(); pos++) {
            // 检查当前状态是否为接受状态
            for (int s : currentStates) {
                if (states[s].tokenType != ERROR) {
                    matchedType = states[s].tokenType;
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
                if(states[t].tokenType==KEYWORD)
                return KEYWORD;
        }
        return (lastAcceptPos > 0) ? matchedType : ERROR;
    }

private:
    int nextStateId = 0;
    int initial = -1;
    unordered_map<int, State> states;
    unordered_map<int, map<char, set<int>>> transitions;

    set<int> epsilonClosure(const set<int>& states) //使用迭代法，而不是dfs
    {
        set<int> closure;
        stack<int> stack;
        for (int s : states) {
            closure.insert(s);
            stack.push(s);
        }
        while (!stack.empty()) {
            int s = stack.top();
            stack.pop();
            if (transitions[s].count(EPSILON)) {
                for (int next : transitions[s][EPSILON]) {
                    if (!closure.count(next)) {
                        closure.insert(next);
                        stack.push(next);
                    }
                }
            }
        }
        return closure;
    }

    set<int> move(const set<int>& states, char c) {
        set<int> result;
        for (int s : states) {
            if (transitions[s].count(c)) {
                result.insert(transitions[s][c].begin(), transitions[s][c].end());
            }
        }
        return result;
    }
};
vector<pair<TokenType, string>> tokenize(const string& filename, NFA& nfa) {
    vector<pair<TokenType, string>> tokens;
    ifstream file(filename);
    string buffer;
    char c;

    while (file.get(c)) {
        if (isspace(c)) {
            if (!buffer.empty()) {
                // 缓冲区非空时立即结束当前词法单元
                TokenType type = nfa.match(buffer);
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
        TokenType type = nfa.match(buffer);

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
                TokenType newType = nfa.match(newBuffer);

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
        TokenType type = nfa.match(buffer);
        if (type != ERROR) {
            tokens.emplace_back(type, buffer);
        } else {
            cerr << "错误：无法识别的符号 '" << buffer << "'" << endl;
        }
    }

    return tokens;
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
// 主函数
int main() {
    NFA pl0NFA = buildPL0NFA();
    auto tokens = tokenize("1.txt", pl0NFA);

    freopen("1.out","w+",stdout);
    for (const auto& token : tokens) {
        cout << "(" << token.first << ", '" << token.second << "')" << endl;
    }
    
    return 0;
}