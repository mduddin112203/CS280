// Md Uddin
// NJIT ID: 31569771
// UCID: Msu7
// CS280-012
#include "parser.h"

// Global variables
map<string, bool> varDefn; // Map to store defined variables
static string placeholder = "0"; // Static string placeholder initialized to "0"

// Namespace for Parser functions
namespace Parser {
    bool pushed_back = false; // Flag to track if token is pushed back
    LexItem pushed_token; // Last pushed token

    // Function to get the next token
    static LexItem GetNextToken(istream& in, int& line) {
        if (pushed_back) { // If a token is pushed back, return it
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line); // Otherwise, get the next token from input
    }

  // Function to push back a token
    static void PushBackToken(LexItem & t) {
        if (pushed_back) { // If a token is already pushed back, abort
            abort();
        }
        pushed_back = true; // Set pushed_back flag to true
        pushed_token = t; // Store the token to be pushed back
    }

}
// Static variable initialization
static int error_count = 0; // Static variable to count parse errors

// Function to print parse error message
void ParseError(int line, string msg) {
    ++error_count; // Increment error count
    cout << line << ": " << msg << endl; // Print error message with line number
}
// Function to return the total number of parse errors
int ErrCount() {
    return error_count; // Return the error count
}

// Function to parse Multiplicative Expression
bool MultExpr(istream& in, int& line) {
    bool status;
    status = TermExpr(in, line);
    if (!status) {
        return false;
    }
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == MULT || tokenPlaceholder == DIV) {
        status = MultExpr(in, line);
    }
    else if(tokenPlaceholder.GetToken() == ERR){
        ParseError(line, "Error message 1");
        cout << "placeholder1" << tokenPlaceholder.GetLexeme() << "placeholder2" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tokenPlaceholder);
    }
    return status;
}//end of Multiplicative Expression

// Function to parse Variable List
bool VarList(istream& in, int& line) {
    bool status;
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == IDENT) {
        auto it = varDefn.find(tokenPlaceholder.GetLexeme());
        if (it == varDefn.end()) {
            varDefn[tokenPlaceholder.GetLexeme()] = true;
        }
        else {
            if (it->second) {
                it->second = false;
                ParseError(line, "Error message 2");
                return false;
            }
        }
    }
    Parser::PushBackToken(tokenPlaceholder);
    status = Var(in, line);
    if (!status) {
        return false;
    }
    string varName = tokenPlaceholder.GetLexeme();
    tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == ASSOP) {
        status = Expr(in, line);
        if (!status) {
            return false;
        }
        else {
cout << "Initialization of the variable " << varName << " in the declaration statement." << endl;           
            tokenPlaceholder = Parser::GetNextToken(in, line);
        }
    }
    if (tokenPlaceholder == COMMA) {
        status = VarList(in, line);
    }
    else {
        Parser::PushBackToken(tokenPlaceholder);
    }
    return status;
}//end of Variable List

// Function to parse Term Expression
bool TermExpr(istream& in, int& line) {
    bool status;
    status = SFactor(in, line);
    if (!status) {
        return false;
    }
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == POW) {
        status = TermExpr(in, line);
    }
    else if(tokenPlaceholder.GetToken() == ERR){
        ParseError(line, "Error message 3");
        cout << "placeholder5" << tokenPlaceholder.GetLexeme() << "placeholder6" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tokenPlaceholder);
    }
    return status;
}//end of Term Expression


// Function to parse Simple Statement
bool SimpleStmt(istream& in, int& line) {
    bool flag;
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == IDENT) { // Check if token represents an assignment statement
         // Execute assignment statement parsing
        flag = AssignStmt(in, line);
        if (flag) {
            cout << "Error message 4" << endl;
        }
    }
    else if (tokenPlaceholder == PRINT) { // Check if token denotes a print statement
         // Execute print statement parsing
        flag = PrintStmt(in, line);
        if (flag) {
            cout << "Print statement in a Simple If statement." << endl;
        }
    }
    else { // Push back token if it doesn't represent an assignment or print statement
        Parser::PushBackToken(tokenPlaceholder);
        return false;
    }
    return flag; // Return the status of parsing
}//end of Simple Statement

// Function to parse Print Statement
bool PrintStmt(istream& in, int& line) {
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if(tokenPlaceholder != DEF) {
        ParseError(line, "Error message 5");
        return false;
    }
    tokenPlaceholder = Parser::GetNextToken(in, line);
    if(tokenPlaceholder != COMMA) {
        ParseError(line, "Error message 6");
        return false;
    }
    bool status = ExprList(in, line);
    if(!status) {
        ParseError(line, "Error message 7");
        return false;
    }
    return status;
}//End of Print Statement

// Function to handle Block If Statement
bool BlockIfStmt(istream& in, int& line) {
    static int nestingLevel = 0;
    nestingLevel++;

    // Loop to process statements within the block
    while (true) {
        if (!Stmt(in, line)) {
            break;
        }
    }

    // Check for ELSE clause and process its statements if present
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == ELSE) {
        while (true) {
            if (!Stmt(in, line)) {
                break;
            }
        }
    }
    // Output end of block message for each nesting level
    tokenPlaceholder = Parser::GetNextToken(in, line);
    while (nestingLevel > 0) {
        cout << "End of Block If statement at nesting level " << nestingLevel << endl;
        nestingLevel--;
    }
    // Check for END IF tokens
    if (tokenPlaceholder != END) {
        //ParseError(line, "Error finding END");
        return false;
    }
    tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder != IF) {
        //ParseError(line, "Error finding IF");
        return false;
    }

    return true;
}//end of Block If Statement


// Function to parse Simple If Statement
bool SimpleIfStmt(istream& in, int& line) {
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder != LPAREN) {
        return false;
    }
    if (!RelExpr(in, line)) {
        return false;
    }
    tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder != RPAREN) {
        return false;
    }
    tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == THEN) {
        return BlockIfStmt(in, line);
    }
    else {
        Parser::PushBackToken(tokenPlaceholder);
    }
    if (!SimpleStmt(in, line)) {
        return false;
    }
    return true;
}//end of imple If Statement

// Function to parse Signed Factor
bool SFactor(istream& in, int& line) {
    int sign = 1;
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == PLUS || tokenPlaceholder == MINUS) {
        if (tokenPlaceholder == PLUS) {
            sign = 1;
        }
        if (tokenPlaceholder == MINUS) {
            sign = 0;
        }
        return Factor(in, line, sign);
    }
    else {
        Parser::PushBackToken(tokenPlaceholder);
        return Factor(in, line, sign);
    }
    return true;
}//end of Signed Factor

// Function to parse Declaration
bool Decl(istream& in, int& line) {
    if (!Type(in, line)) {
        return false;
    }
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder != DCOLON) {
        if (tokenPlaceholder == ASSOP) {
            return false;
        }
        ParseError(line, "Error message 8");
        return false;
    }
    if (!VarList(in, line)) {
        ParseError(line, "Error message 9");
        return false;
    }
    if (stoi(placeholder) > 0) {
        cout << "Definition of Strings with length of " << placeholder << " in declaration statement." << endl;
    }
    return true;
}//end of Declaration

// Function to parse Expression List
bool ExprList(istream& in, int& line) {
    bool status;
    status = Expr(in, line);
    if(!status){
        return false;
    }
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == COMMA) {
        status = ExprList(in, line);
    }
    else if(tokenPlaceholder.GetToken() == ERR){
        ParseError(line, "Error message 10");
        cout << "placeholder 11" << tokenPlaceholder.GetLexeme() << "placeholder 12" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tokenPlaceholder);
    }
    return status;
}//End of Expression List

// Function to parse Assignment Statement
bool AssignStmt(istream& in, int& line) {
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder != ASSOP) {
        return false;
    }
    if (!Expr(in, line)) {
        return false;
    }
    if (varDefn[tokenPlaceholder.GetLexeme()]) {
        cout << "placeholder 13" << tokenPlaceholder.GetLexeme() << "placeholder 14" << endl;
    }
    return true;
}//end of Assignment Statement

// Function to parse Relational Expression
bool RelExpr(istream& in, int& line) {
    if (!Expr(in, line)) {
        ParseError(line, "Error message 11");
        return false;
    }
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);
    if (tokenPlaceholder == EQ || tokenPlaceholder == LTHAN || tokenPlaceholder == GTHAN) {
        if (!Expr(in, line)) {
            return false;
        }
    }
//    else {
//        Parser::PushBackToken(tok);
//    }
    return true;
}// End of Relational Expression

// Function to handle Statement parsing
bool Stmt(istream& in, int& line) {
    bool status;

    // Get the next token
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check for END token
    if (tokenPlaceholder == END) {
        Parser::PushBackToken(tokenPlaceholder);
        return false;
    }

    // Check for IDENT, IF, or PRINT tokens
    if (tokenPlaceholder == IDENT) {
        // Parse Assignment Statement
        //Parser::PushBackToken(tok);
        status = AssignStmt(in, line);
    }
    else if (tokenPlaceholder == IF) {
        // Parse Simple If Statement
        status = SimpleIfStmt(in, line);
    }
    else if (tokenPlaceholder == PRINT) {
        // Parse Print Statement
        status = PrintStmt(in, line);
    }
    else {
        // Invalid statement
        //ParseError(line, "invalid stmt");
        Parser::PushBackToken(tokenPlaceholder);
        return false;
    }

    // Return status of statement parsing
    return status;
}//end of Statement


// Function to handle Expression parsing
bool Expr(istream& in, int& line) {
    bool status;

    // Parse Multiplicative Expression
    status = MultExpr(in, line);
    if (!status) {
        return false;
    }

    // Get the next token
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check for addition, subtraction, or concatenation tokens
    if (tokenPlaceholder == PLUS || tokenPlaceholder == MINUS || tokenPlaceholder == CAT) {
        // Parse additional Expression
        status = Expr(in, line);
    }
    else if(tokenPlaceholder.GetToken() == ERR){
        // Error handling for unrecognized input pattern
        cout << "(" << tokenPlaceholder.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        // Push back token for future processing
        Parser::PushBackToken(tokenPlaceholder);
    }

    // Return status of Expression parsing
    return status;
}//End of Expression

// Function to handle Type parsing
bool Type(istream& in, int& line) {
    // Get the next token
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check if the token represents INTEGER or REAL type
    if (tokenPlaceholder == INTEGER || tokenPlaceholder == REAL) {
        // Return true if it's INTEGER or REAL
        return true;
    }
    // Check if the token represents CHARACTER type
    else if (tokenPlaceholder == CHARACTER) {
        // Check for optional string length specification
        tokenPlaceholder = Parser::GetNextToken(in, line);
        if (tokenPlaceholder == LPAREN) {
            // Read string length
            tokenPlaceholder = Parser::GetNextToken(in, line);
            if (tokenPlaceholder != LEN) {
                return false;
            }
            // Read assignment operator
            tokenPlaceholder = Parser::GetNextToken(in, line);
            if (tokenPlaceholder != ASSOP) {
                return false;
            }
            // Read string length value
            tokenPlaceholder = Parser::GetNextToken(in, line);
            if (tokenPlaceholder != ICONST) {
                // Error handling for incorrect initialization of string length
                ParseError(line, "Error message 12");
                return false;
            }
            // Store string length value
          placeholder = tokenPlaceholder.GetLexeme();

            // Check for closing parenthesis
            tokenPlaceholder = Parser::GetNextToken(in, line);
            if (tokenPlaceholder != RPAREN) {
                return false;
            }
        }
        else {
            // Push back token for future processing
            Parser::PushBackToken(tokenPlaceholder);
        }
        // Return true indicating successful parsing of CHARACTER type
        return true;
    }
    else {
        // Push back token for future processing
        Parser::PushBackToken(tokenPlaceholder);
        // Return false for unrecognized type
        return false;
    }
}//end of Type


// Function to handle Variable parsing
bool Var(istream& in, int& line) {
    // Get the next token
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check if the token represents an identifier
    if (tokenPlaceholder != IDENT) {
        // Return false if the token is not an identifier
        return false;
    }
    // Return true indicating successful parsing of a variable
    return true;
}//end of Variable

// Function to parse Factor
bool Factor(istream& in, int& line, int sign) {
    // Get the next token
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check if the token is an identifier
    if (tokenPlaceholder == IDENT) {
        // Check if the identifier is defined
        if (!varDefn[tokenPlaceholder.GetLexeme()]) {
            // Emit an error if the identifier is undefined
            ParseError(line, "Error message 13");
            return false;
        }
        // Return true indicating successful parsing of an identifier
        return true;
    }
    // Check if the token is an integer, real, or string constant
    else if (tokenPlaceholder == ICONST || tokenPlaceholder == RCONST || tokenPlaceholder == SCONST) {
        // Return true indicating successful parsing of a constant
        return true;
    }
    // Check if the token is a left parenthesis
    else if (tokenPlaceholder == LPAREN) {
        // Parse the expression within the parentheses
        if (!Expr(in, line)) {
            return false;
        }
        // Get the next token after the expression
        tokenPlaceholder = Parser::GetNextToken(in, line);
        // Check if the next token is a right parenthesis
        if (tokenPlaceholder != RPAREN) {
            // Emit an error if the right parenthesis is missing
            ParseError(line, "Error message 14");
            return false;
        }
        // Return true indicating successful parsing of an expression within parentheses
        return true;
    }
    // Emit an error if none of the above cases match
    ParseError(line, "Error message 15");
    return false;
}//end of Factor


bool IdentList(istream& in, int& line);

// Function to parse Program
bool Prog(istream& in, int& line) {
    // Get the next token
    LexItem tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check if the token is PROGRAM
    if (tokenPlaceholder != PROGRAM) {
        // Emit an error if PROGRAM is missing at the beginning
        ParseError(line, "Error message 16");
        return false;
    }

    // Get the next token
    tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check if the token is an identifier
    if (tokenPlaceholder != IDENT) {
        // Emit an error if the program name is missing
        ParseError(line, "Error message 17");
        return false;
    }

    // Parse declarations
    while (true) {
        if (!Decl(in, line)) {
            break;
        }
    }

    // Parse statements
    while (true) {
        if (!Stmt(in, line)) {
            break;
        }
    }

    // Get the next token
    tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check if the token is END
    if (tokenPlaceholder != END) {
        // Emit an error if END is missing at the end of the program
        ParseError(line, "Error message 18");
        return false;
    }

    // Get the next token
    tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check if the token is PROGRAM
    if (tokenPlaceholder != PROGRAM) {
        // Emit an error if PROGRAM is missing at the end
        ParseError(line, "Error message 19");
        return false;
    }

    // Get the next token
    tokenPlaceholder = Parser::GetNextToken(in, line);

    // Check if the token is an identifier
    if (tokenPlaceholder != IDENT) {
        // Emit an error if the program name is missing
        ParseError(line, "");
        return false;
    }
    // Output a message indicating successful program parsing
    cout << "(DONE)" << endl;

    // Return true indicating successful parsing of the program
    return true; // end of Program
}//end of Parser.cpp