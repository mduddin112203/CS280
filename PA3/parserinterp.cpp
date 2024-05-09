#include "parserInterp.h"
#include <stack>

map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects
string strLen = "1";
map<string, Value>::iterator it;

namespace Parser {
    bool pushed_back = false;
    LexItem	pushed_token;

    static LexItem GetNextToken(istream& in, int& line) {
        if( pushed_back ) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem & t) {
        if( pushed_back ) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }
}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

//Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line) {
    bool dl = false, sl = false;
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok.GetToken() == PROGRAM) {
        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == IDENT) {
            dl = Decl(in, line);
            if(!dl) {
                ParseError(line, "Incorrect Declaration in Program");
                return false;
            }
            sl = Stmt(in, line);
            if(!sl) {
                ParseError(line, "Incorrect Statement in program");
                return false;
            }
            tok = Parser::GetNextToken(in, line);
            if (tok.GetToken() == END) {
                tok = Parser::GetNextToken(in, line);
                if (tok.GetToken() == PROGRAM) {
                    tok = Parser::GetNextToken(in, line);
                    if (tok.GetToken() == IDENT) {
//                        for (it = TempsResults.begin(); it != TempsResults.end(); it++) {
//                            cout << it->first << " " << it->second << endl;
//                        }
                        cout << "(DONE)" << endl;
                        return true;
                    }
                    else {
                        ParseError(line, "Missing Program Name");
                        return false;
                    }
                }
                else {
                    ParseError(line, "Missing PROGRAM at the End");
                    return false;
                }
            }
            else {
                ParseError(line, "Missing END of Program");
                return false;
            }
        }
        else {
            ParseError(line, "Missing Program name");
            return false;
        }
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else {
        ParseError(line, "Missing Program keyword");
        return false;
    }
}

//Decl ::= Type :: VarList
//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
bool Decl(istream& in, int& line) {
    bool status = false;
    LexItem tok;
    //string strLen;

    LexItem t = Parser::GetNextToken(in, line);
    if(t == INTEGER || t == REAL || t == CHARACTER ) {
        tok = t;
        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == DCOLON) {
            status = VarList(in, line, t);
            if (status) {
                status = Decl(in, line);
                if(!status) {
                    ParseError(line, "Declaration Syntactic Error.");
                    return false;
                }
                return status;
            }
            else {
                ParseError(line, "Missing Variables List.");
                return false;
            }
        }
        else if(t == CHARACTER && tok.GetToken() == LPAREN) {
            tok = Parser::GetNextToken(in, line);
            if(tok.GetToken() == LEN) {
                tok = Parser::GetNextToken(in, line);
                if(tok.GetToken() == ASSOP) {
                    tok = Parser::GetNextToken(in, line);
                    if(tok.GetToken() == ICONST) {
                        strLen = tok.GetLexeme();
                        tok = Parser::GetNextToken(in, line);
                        if(tok.GetToken() == RPAREN) {
                            tok = Parser::GetNextToken(in, line);
                            if(tok.GetToken() == DCOLON) {
                                status = VarList(in, line, t);
                                if (status) {
                                    //cout << "Definition of Strings with length of " << strLen << " in declaration statement." << endl;
                                    status = Decl(in, line);
                                    if(!status) {
                                        ParseError(line, "Declaration Syntactic Error.");
                                        return false;
                                    }
                                    return status;
                                }
                                else {
                                    ParseError(line, "Missing Variables List.");
                                    return false;
                                }
                            }
                        }
                        else {
                            ParseError(line, "Missing Right Parenthesis for String Length definition.");
                            return false;
                        }

                    }
                    else {
                        ParseError(line, "Incorrect Initialization of a String Length");
                        return false;
                    }
                }
            }
        }
        else {
            ParseError(line, "Missing Double Colons");
            return false;
        }
    }
    Parser::PushBackToken(t);
    return true;
}

//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line) {
    bool status;
    LexItem t = Parser::GetNextToken(in, line);

    switch(t.GetToken()) {
        case PRINT:
            status = PrintStmt(in, line);
            if(status)
                status = Stmt(in, line);
            break;
        case IF:
            status = BlockIfStmt(in, line);
            if(status)
                status = Stmt(in, line);
            break;
        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);
            if(status)
                status = Stmt(in, line);
            break;
        default:
            Parser::PushBackToken(t);
            return true;
    }
    return status;
}

//SimpleStmt ::= AssigStmt | PrintStmt
bool SimpleStmt(istream& in, int& line) {
    bool status;
    LexItem t = Parser::GetNextToken(in, line);

    switch(t.GetToken()) {
        case PRINT:
            status = PrintStmt(in, line);
            if(!status) {
                ParseError(line, "Incorrect Print Statement");
                return false;
            }
            cout << "Print statement in a Simple If statement." << endl;
            break;
        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);
            if(!status) {
                ParseError(line, "Incorrect Assignent Statement");
                return false;
            }
            cout << "Assignment statement in a Simple If statement." << endl;
            break;
        default:
            Parser::PushBackToken(t);
            return true;
    }
    return status;
}

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem& idtok, int strlen) {
    bool status = false, exprstatus = false;
    string identstr;
    Value retVal;
    strlen = stoi(strLen);
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok == IDENT) {
        identstr = tok.GetLexeme();
        if (!(defVar.find(identstr)->second)) {
            defVar[identstr] = true;
        }
        else {
            ParseError(line, "Variable Redefinition");
            return false;
        }
        SymTable[tok.GetLexeme()] = idtok.GetToken();
        if (SymTable[tok.GetLexeme()] == CHARACTER) {
            string s(strlen, ' ');
            retVal = Value(s);
            TempsResults[tok.GetLexeme()] = retVal;
        }
    }
    else {
        ParseError(line, "Missing Variable Name");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok == ASSOP) {
        exprstatus = Expr(in, line, retVal);
        if(!exprstatus) {
            ParseError(line, "Incorrect initialization for a variable.");
            return false;
        }
        if (idtok == REAL) {
            if (retVal.GetType() == VINT) {
                retVal = double(retVal.GetInt());
            }
        }
        if (idtok == CHARACTER) {
            string ogString = retVal.GetString();
            int extraSpaces = strlen - ogString.length();
            if (ogString.length() > strlen) {
                retVal = Value(ogString.substr(0, strlen));
            }
            else {
                retVal = Value(ogString + string(extraSpaces, ' '));
            }
        }
        TempsResults[identstr] = retVal;
        //cout << identstr << " " << retVal.GetType() << endl;
        //cout<< "Initialization of the variable " << identstr <<" in the declaration statement." << endl;

        tok = Parser::GetNextToken(in, line);
        if (tok == COMMA) {
            status = VarList(in, line, idtok, strlen);
        }
        else {
            Parser::PushBackToken(tok);
            return true;
        }
    }
    else if (tok == COMMA) {
        status = VarList(in, line, idtok, strlen);
    }
    else if(tok == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        return false;
    }
    else {
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}

//PrintStmt:= PRINT *, ExprList
bool PrintStmt(istream& in, int& line) {
    LexItem t;
    t = Parser::GetNextToken(in, line);
    ValQue = new queue<Value>;

    if(t != DEF) {
        ParseError(line, "Print statement syntax error.");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != COMMA) {
        ParseError(line, "Missing Comma.");
        return false;
    }

    bool ex = ExprList(in, line);
    if( !ex ) {
        ParseError(line, "Missing expression after Print Statement");
        return false;
    }
    while (!(*ValQue).empty()) {
        Value nextVal = (*ValQue).front();
        cout << nextVal;
        ValQue->pop();
    }
    cout << endl;
    return ex;
}//End of PrintStmt

//BlockIfStmt:= if (Expr) then {Stmt} [Else Stmt]
//SimpleIfStatement := if (Expr) Stmt
bool BlockIfStmt(istream& in, int& line) {
    bool ex=false, status ;
    //static int nestlevel = 0;
    //int level;
    LexItem t;

    t = Parser::GetNextToken(in, line);
    if(t != LPAREN) {
        ParseError(line, "Missing Left Parenthesis");
        return false;
    }
    Value retVal;
    ex = RelExpr(in, line, retVal);
    if(!ex) {
        ParseError(line, "Missing if statement condition");
        return false;
    }
    if (retVal.GetType() == VERR) {
        ParseError(line, "Illegal operand types for a Relational operation.");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if(t != RPAREN) {
        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    if (retVal.GetBool() == 0) {
        while (t != ELSE && t != END) {
            t = Parser::GetNextToken(in, line);
        }
        status = Stmt(in, line);
        if(!status) {
            ParseError(line, "Missing Statement for If-Stmt Else-Part");
            return false;
        }
        else
            t = Parser::GetNextToken(in, line);
        if(t != END) {
            ParseError(line, "Missing END of IF");
            return false;
        }

        t = Parser::GetNextToken(in, line);
        if(t == IF ) {
            //cout << "End of Block If statement at nesting level " << level << endl;
            return true;
        }
        Parser::PushBackToken(t);
        ParseError(line, "Missing IF at End of IF statement");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if(t != THEN) {
        Parser::PushBackToken(t);
        status = SimpleStmt(in, line);
        if(status) {
            return true;
        }
        else {
            ParseError(line, "If-Stmt Syntax Error");
            return false;
        }
    }
    //nestlevel++;
    //level = nestlevel;
    status = Stmt(in, line);
    if(!status) {
        ParseError(line, "Missing Statement for If-Stmt Then-Part");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if( t == ELSE ) {
        status = Stmt(in, line);
        if(!status) {
            ParseError(line, "Missing Statement for If-Stmt Else-Part");
            return false;
        }
        else
            t = Parser::GetNextToken(in, line);
    }
    if(t != END) {
        ParseError(line, "Missing END of IF");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t == IF ) {
        //cout << "End of Block If statement at nesting level " << level << endl;
        return true;
    }
    Parser::PushBackToken(t);
    ParseError(line, "Missing IF at End of IF statement");
    return false;
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem& idtok) {
    string identstr;
    LexItem tok = Parser::GetNextToken(in, line);
    idtok = tok;

    if (tok == IDENT){
        identstr = tok.GetLexeme();
        if (!(defVar.find(identstr)->second)) {
            ParseError(line, "Undeclared Variable");
            return false;
        }
        return true;
    }
    else if(tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
    bool varstatus = false, status = false;
    LexItem t;
    int assignLine = line;
    varstatus = Var(in, line, t);

    if (varstatus){
        LexItem tok = Parser::GetNextToken(in, line);
        if (tok == ASSOP){
            Value retVal;
            status = Expr(in, line, retVal);
            if(!status) {
                ParseError(line, "Missing Expression in Assignment Statment");
                return status;
            }
            if (SymTable[t.GetLexeme()] == CHARACTER) {
                if (retVal.GetType() != VSTRING) {
                    ParseError(assignLine, "Illegal mixed-mode assignment operation");
                    return false;
                }
            }
            if (SymTable[t.GetLexeme()] == CHARACTER) {
                string ogString = retVal.GetString();
                int extraSpaces = stoi(strLen) - ogString.length();
                if (ogString.length() > stoi(strLen)) {
                    retVal = Value(ogString.substr(0, stoi(strLen)));
                }
                else {
                    retVal = Value(ogString + string(extraSpaces, ' '));
                }
            }
            TempsResults[t.GetLexeme()] = retVal;
            if (TempsResults[t.GetLexeme()].GetType() == VERR) {
                ParseError(assignLine, "Illegal operand type for the operation.");
                return false;
            }
            //cout << t.GetLexeme() << TempsResults[t.GetLexeme()] << endl;
        }
        else if(tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        }
        else {
            ParseError(line, "Missing Assignment Operator");
            return false;
        }
    }
    else {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    return status;
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status = false;
    Value retVal;
    status = Expr(in, line, retVal);

    if(!status) {
        ParseError(line, "Missing Expression");
        return false;
    }
    ValQue->push(retVal);
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == COMMA) {
        status = ExprList(in, line);
    }
    else if(tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else {
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}//End of ExprList

//RelExpr ::= Expr  [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line, Value& retVal) {
    bool t1 = Expr(in, line, retVal);
    LexItem tok;
    if(!t1) {
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    if ( tok == EQ || tok == LTHAN || tok == GTHAN) {
        string op = tok.GetLexeme();
        Value val2;
        t1 = Expr(in, line, val2);
        if( !t1 ) {
            ParseError(line, "Missing operand after operator");
            return false;
        }
        if (tok == EQ) {
            retVal = retVal == val2;
        }
        else if (tok == LTHAN) {
            retVal = retVal < val2;
        }
        else if (tok == GTHAN) {
            retVal = retVal > val2;
        }
    }
    return true;
}//End of RelExpr

//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line, Value& retVal) {
    bool t1 = MultExpr(in, line, retVal);
    LexItem tok;
    if(!t1) {
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while ( tok == PLUS || tok == MINUS || tok == CAT) {
        string op = tok.GetLexeme();
        Value val2;
        t1 = MultExpr(in, line, val2);
        if( !t1 ) {
            ParseError(line, "Missing operand after operator");
            return false;
        }
        if (tok == PLUS) {
            retVal = retVal + val2;
        }
        else if (tok == MINUS) {
            retVal = retVal - val2;
        }
        else if (tok == CAT) {
            retVal = retVal.Catenate(val2);
        }
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    Parser::PushBackToken(tok);
    return true;
}//End of Expr

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line, Value& retVal) {
    bool t1 = TermExpr(in, line, retVal);
    Value val2;
    LexItem tok;
    if(!t1) {
        return false;
    }
    tok	= Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while ( tok == MULT || tok == DIV  ) {
        t1 = TermExpr(in, line, val2);
        if(!t1) {
            ParseError(line, "Missing operand after operator");
            return false;
        }
        if (tok == DIV) {
            if (val2.GetReal() == 0) { //HARD CODED
                ParseError(line -1, "Run-Time Error-Illegal division by Zero");
                return false;
            }
            retVal = retVal / val2;
        }
        else if (tok == MULT) {
            retVal = retVal * val2;
        }

        tok	= Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    return true;
}//End of MultExpr

//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line, Value& retVal) {
    stack<Value> powerStack;
    Value val2;
    Value power;
    Value base;
    LexItem tok;
    bool t1 = SFactor(in, line, retVal);
    if(!t1) {
        return false;
    }
    tok	= Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    powerStack.push(retVal);
    while ( tok == POW  ) {
        t1 = SFactor(in, line, val2);
        if(!t1) {
            ParseError(line, "Missing exponent operand");
            return false;
        }
        powerStack.push(val2);
        tok	= Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    while (!powerStack.empty()) {
        if (powerStack.size() == 1) {
            break;
        }
        power = powerStack.top();
        powerStack.pop();
        base = powerStack.top();
        powerStack.pop();
        retVal = base.Power(power);
        retVal = powerStack.top().Power(retVal);
    }
    Parser::PushBackToken(tok);
    return true;
}//End of TermExpr

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value& retVal) {
    LexItem t = Parser::GetNextToken(in, line);
    bool status;
    int sign = 0;

    if(t == MINUS) {
        sign = -1;
    }
    else if(t == PLUS) {
        sign = 1;
    }
    else {
        Parser::PushBackToken(t);
    }
    status = Factor(in, line, sign, retVal);
    return status;
}//End of SFactor

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value& retVal) {
    LexItem tok = Parser::GetNextToken(in, line);
    //cout << tok.GetLexeme() << endl;

    if(tok == IDENT) {
        string lexeme = tok.GetLexeme();
        if (!(defVar.find(lexeme)->second)) {
            ParseError(line, "Using Undefined Variable");
            return false;
        }
        retVal = TempsResults[lexeme];
        if (TempsResults[lexeme].GetType() == VERR) {
            ParseError(line, "Using uninitialized Variable");
            return false;
        }
        if (sign == -1) {
            if (retVal.IsInt()) {
                retVal = -retVal.GetInt();
            }
            else if (retVal.IsReal()) {
                retVal = -retVal.GetReal();
            }
            if (retVal.IsString()) {
                ParseError(line, "Illegal Operand Type for Sign Operator");
                return false;
            }
        }
        return true;
    }
    else if( tok == ICONST ) {
        retVal.SetType(VINT);
        if (sign == -1) {
            retVal.SetInt(-stoi(tok.GetLexeme()));
        }
        else {
            retVal.SetInt(stoi(tok.GetLexeme()));
        }
        return true;
    }
    else if( tok == SCONST ) {
        retVal.SetType(VSTRING);
        retVal.SetString(tok.GetLexeme());
        return true;
    }
    else if( tok == RCONST ) {
        retVal.SetType(VREAL);
        if (sign == -1) {
            retVal.SetReal(-stod(tok.GetLexeme()));
        }
        else {
            retVal.SetReal(stod(tok.GetLexeme()));
        }
        return true;
    }
    else if( tok == LPAREN ) {
        bool ex = Expr(in, line, retVal);
        if(!ex) {
            ParseError(line, "Missing expression after (");
            return false;
        }
        if( Parser::GetNextToken(in, line) == RPAREN ){
            return ex;
        }
        else {
            Parser::PushBackToken(tok);
            ParseError(line, "Missing ) after expression");
            return false;
        }
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return false;
}