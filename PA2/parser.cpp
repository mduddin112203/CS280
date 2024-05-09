// Md Uddin
// NJIT ID: 31569771
// UCID: Msu7
// CS280-012
#include "parser.h"

map<string, bool> varDefn;
map<string, Token> SymTable;
static int nestingLevel = 0;
static string value = "0";

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

void ParseError(int line, string msg) {
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

//Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != PROGRAM) {
        ParseError(line, "Missing PROGRAM at the beginning");
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok != IDENT) {
        ParseError(line, "Missing Program Name");
        return false;
    }
    while (true) {
        if (!Decl(in, line)) {
            break;
        }
    }
    while (true) {
        if (!Stmt(in, line)) {
            break;
        }
    }
    tok = Parser::GetNextToken(in, line);
    if (tok != END) {
        ParseError(line, "Missing END at the end of program");
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok != PROGRAM) {
        ParseError(line, "Missing PROGRAM at end");
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok != IDENT) {
        ParseError(line, "Missing name of program at the end");
        return false;
    }
    cout << "(DONE)" << endl;
    return true;
}//end of Prog

//Decl ::= Type :: VarList
bool Decl(istream& in, int& line) {
    if (!Type(in, line)) {
        return false;
    }
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != DCOLON) {
        if (tok == ASSOP) {
            return false;
        }
        ParseError(line, "Missing DCOLON for decleration");
        return false;
    }
    if (!VarList(in, line)) {
        ParseError(line, "Missing VarList");
        return false;
    }
    if (stoi(value) > 0) {
        cout << "Definition of Strings with length of " << value << " in declaration statement." << endl;
    }
    return true;
}//end of Decl

//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
bool Type(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == INTEGER || tok == REAL) {
        return true;
    }
    else if (tok == CHARACTER) {
        tok = Parser::GetNextToken(in, line);
        if (tok == LPAREN) {
            tok = Parser::GetNextToken(in, line);
            if (tok != LEN) {
                return false;
            }
            tok = Parser::GetNextToken(in, line);
            if (tok != ASSOP) {
                return false;
            }
            tok = Parser::GetNextToken(in, line);
            if (tok != ICONST) {
                ParseError(line, "Incorrect Initialization of a String Length");
                return false;
            }
            value = tok.GetLexeme();
            tok = Parser::GetNextToken(in, line);
            if (tok != RPAREN) {
                return false;
            }
        }
        else {
            Parser::PushBackToken(tok);
        }
        return true;
    }
    else {
        Parser::PushBackToken(tok);
        return false;
    }
}//end of Type

//VarList ::= Var [= Expr] {, Var [= Expr]
bool VarList(istream& in, int& line) {
    bool status;
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == IDENT) {
        auto it = varDefn.find(tok.GetLexeme());
        if (it == varDefn.end()) {
            varDefn[tok.GetLexeme()] = true;
        }
        else {
            if (it->second) {
                it->second = false;
                ParseError(line, "Variable Redefinition");
                return false;
            }
        }
    }
    Parser::PushBackToken(tok);
    status = Var(in, line);
    if (!status) {
        return false;
    }
    string varName = tok.GetLexeme();
    tok = Parser::GetNextToken(in, line);
    if (tok == ASSOP) {
        status = Expr(in, line);
        if (!status) {
            return false;
        }
        else {
            cout << "Initialization of the variable " << varName << " in the declaration statement." << endl;
            tok = Parser::GetNextToken(in, line);
        }
    }
    if (tok == COMMA) {
        status = VarList(in, line);
    }
    else {
        Parser::PushBackToken(tok);
    }
    return status;
}//end of VarList

//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line) {
    bool status;
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == END) {
        Parser::PushBackToken(tok);
        return false;
    }
    if (tok == IDENT) {
        //Parser::PushBackToken(tok);
        status = AssignStmt(in, line);
    }
    else if (tok == IF) {
        status = SimpleIfStmt(in, line);
    }
    else if (tok == PRINT) {
        status = PrintStmt(in, line);
    }
    else {
        //ParseError(line, "invalid stmt");
        Parser::PushBackToken(tok);
        return false;
    }
    return status;
}//end of Stmt

//SimpleStmt ::= AssigStmt | PrintStmt
bool SimpleStmt(istream& in, int& line) {
    bool flag;
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == IDENT) { //AssignStmt
        //Parser::PushBackToken(tok);
        flag = AssignStmt(in, line);
        if (flag) {
            cout << "Assignment statement in a Simple If statement" << endl;
        }
    }
    else if (tok == PRINT) {
        flag = PrintStmt(in, line);
        if (flag) {
            cout << "Print statement in a Simple If statement." << endl;
        }
    }
    else {
        Parser::PushBackToken(tok);
        return false;
    }
    return flag;
}//end of SimpleStmt

//PrintStmt ::= PRINT *, ExprList
bool PrintStmt(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if(tok != DEF) {
        ParseError(line, "Print statement syntax error.");
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if(tok != COMMA) {
        ParseError(line, "Missing Comma.");
        return false;
    }
    bool status = ExprList(in, line);
    if(!status) {
        ParseError(line, "Missing expression after Print Statement");
        return false;
    }
    return status;
}//End of PrintStmt

//BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
bool BlockIfStmt(istream& in, int& line) {
    nestingLevel++;
    while (true) {
        if (!Stmt(in, line)) {
            break;
        }
    }
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == ELSE) {
        while (true) {
            if (!Stmt(in, line)) {
                break;
            }
        }
    }
    tok = Parser::GetNextToken(in, line);
    while (nestingLevel > 0) {
        cout << "End of Block If statement at nesting level " << nestingLevel << endl;
        nestingLevel--;
    }
    if (tok != END) {
        //ParseError(line, "Error finding END");
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok != IF) {
        //ParseError(line, "Error finding IF");
        return false;
    }
//    while (nestingLevel > 0) {
//        cout << "End of Block If statement at nesting level " << nestingLevel << endl;
//        nestingLevel--;
//    }
    return true;
}//end of BlockIfStmt

//SimpleIfStmt ::= IF (RelExpr) SimpleStmt
bool SimpleIfStmt(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != LPAREN) {
        return false;
    }
    if (!RelExpr(in, line)) {
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok != RPAREN) {
        return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok == THEN) {
        return BlockIfStmt(in, line);
    }
    else {
        Parser::PushBackToken(tok);
    }
    if (!SimpleStmt(in, line)) {
        return false;
    }
    return true;
}//end of SimpleIfStmt

//AssignStmt ::= Var = Expr
bool AssignStmt(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != ASSOP) {
        return false;
    }
    if (!Expr(in, line)) {
        return false;
    }
    if (varDefn[tok.GetLexeme()]) {
        cout << "Initialization of the variable " << tok.GetLexeme() << " in the declaration statement." << endl;
    }
    return true;
}//end of AssignStmt

//Var ::= IDENT
bool Var(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != IDENT) {
        return false;
    }
    return true;
}//end of Var

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status;
    status = Expr(in, line);
    if(!status){
        return false;
    }
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == COMMA) {
        status = ExprList(in, line);
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
    }
    return status;
}//End of ExprList

//RelExpr ::= Expr [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line) {
    if (!Expr(in, line)) {
        ParseError(line, "Expected Expr");
        return false;
    }
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == EQ || tok == LTHAN || tok == GTHAN) {
        if (!Expr(in, line)) {
            return false;
        }
    }
//    else {
//        Parser::PushBackToken(tok);
//    }
    return true;
}//RelExpr

//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line) {
    bool status;
    status = MultExpr(in, line);
    if (!status) {
        return false;
    }
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == PLUS || tok == MINUS || tok == CAT) {
        status = Expr(in, line);
    }
    else if(tok.GetToken() == ERR){
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
    }
    return status;
}//Expr

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line) {
    bool status;
    status = TermExpr(in, line);
    if (!status) {
        return false;
    }
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == MULT || tok == DIV) {
        status = MultExpr(in, line);
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
    }
    return status;
}//end of MultExpr

//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line) {
    bool status;
    status = SFactor(in, line);
    if (!status) {
        return false;
    }
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == POW) {
        status = TermExpr(in, line);
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
    }
    return status;
}//end of TermExpr

//SFactor ::= [+ | -] Factor
bool SFactor(istream& in, int& line) {
    int sign = 1;
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == PLUS || tok == MINUS) {
        if (tok == PLUS) {
            sign = 1;
        }
        if (tok == MINUS) {
            sign = 0;
        }
        return Factor(in, line, sign);
    }
    else {
        Parser::PushBackToken(tok);
        return Factor(in, line, sign);
    }
    return true;
}//end of SFactor

//Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == IDENT) {
        if (!varDefn[tok.GetLexeme()]) {
            ParseError(line, "Using Undefined Variable");
            return false;
        }
        return true;
    }
    else if (tok == ICONST || tok == RCONST || tok == SCONST) {
        return true;
    }
    else if (tok == LPAREN) {
        if (!Expr(in, line)) {
            return false;
        }
        tok = Parser::GetNextToken(in, line);
        if (tok != RPAREN) {
            ParseError(line, "Missing closing parenthesis in Factor.");
            return false;
        }
        return true;
    }
    ParseError(line, "Invalid factor token.");
    return false;
}//end of Factor

int ErrCount() {
    return error_count;
}//end of ErrCount