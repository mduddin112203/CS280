// Md Uddin
// NJIT ID: 31569771
// UCID: Msu7
// CS280-012
#include "parserInterp.h"
#include <stack>

// Global variables
map<string, bool> defVar; // Map to store defined variables
map<string, Token> SymTable; // Symbol table to store variable names and their corresponding holderens
map<string, Value> TempsResults; // Container for temporary locations of Value objects for results of expressions, variables values, and constants
queue<Value>* ValQue; // Pointer variable to a queue of Value objects
string strLen = "1"; // Variable to store the length of strings
map<string, Value>::iterator it; // Iterator for iterating through the TempsResults map

// Namespace for parser utilities
namespace Parser {
    bool backP = false; // Flag to indicate if a holderen has been pushed back
    LexItem TP; // Variable to store the pushed holderen

    // Function to get the next holderen
    static LexItem GetNextToken(istream& in, int& line) {
        if (backP) { // Check if a holderen has been pushed back
            backP = false;
            return TP;
        }
        return getNextToken(in, line); // Get the next holderen from the input stream
    }

    // Function to push back a holderen
    static void backPTok(LexItem& t) {
        if (backP) { // Check if a holderen has already been pushed back
            abort(); // Abort if a holderen has been pushed back twice
        }
        backP = true;
        TP = t; // Store the holderen to be pushed back
    }
}

static int error_count = 0; // Global variable to count parsing errors

// Function to get the error count
int ErrCount() {
    return error_count;
}

// Function to report a parsing error
void ParseError(int line, string msg) {
    ++error_count; // Increment the error count
    cout << line << ": " << msg << endl; // Output the error message with line number
}

// Function declaration to parse a list of identifiers
bool IdentList(istream& in, int& line);

// Function to parse the program structure
bool Prog(istream& in, int& line) {
    // Initialize flags for declaration and statement parsing
    bool dl = false, sl = false;
    // Get the next holderen
    LexItem holder = Parser::GetNextToken(in, line);

    // Check if the holderen is PROGRAM
    if (holder.GetToken() == PROGRAM) {
        // Get the next holderen
        holder = Parser::GetNextToken(in, line);
        // Check if the holderen is IDENT
        if (holder.GetToken() == IDENT) {
            // Parse declarations
            dl = Decl(in, line);
            // Check if declaration parsing was successful
            if (!dl) {
                ParseError(line, "wrong1");
                return false;
            }
            // Parse statements
            sl = Stmt(in, line);
            // Check if statement parsing was successful
            if (!sl) {
                ParseError(line, "wrong2");
                return false;
            }
            // Get the next holderen
            holder = Parser::GetNextToken(in, line);
            // Check if the holderen is END
            if (holder.GetToken() == END) {
                // Get the next holderen
                holder = Parser::GetNextToken(in, line);
                // Check if the holderen is PROGRAM
                if (holder.GetToken() == PROGRAM) {
                    // Get the next holderen
                    holder = Parser::GetNextToken(in, line);
                    // Check if the holderen is IDENT
                    if (holder.GetToken() == IDENT) {
                        // Print a message indicating successful parsing
                        cout << "(DONE)" << endl;
                        return true; // Return true indicating successful parsing
                    }
                    else {
                        ParseError(line, "Wrong3");
                        return false;
                    }
                }
                else {
                    ParseError(line, "Wrong4");
                    return false;
                }
            }
            else {
                ParseError(line, "Wrong5");
                return false;
            }
        }
        else {
            ParseError(line, "Wrong6");
            return false;
        }
    }
    // Check if the holderen is an error holderen
    else if (holder.GetToken() == ERR) {
        ParseError(line, "Wrong7");
        cout << "(" << holder.GetLexeme() << ")" << endl;
        return false;
    }
    else {
        ParseError(line, "Wrong8");
        return false;
    }
}

// Function to parse a term expression
bool TermExpr(istream& in, int& line, Value& retVal) {
    // Stack to hold values for exponentiation
    stack<Value> powerStack;
    Value val2;
    Value power;
    Value base;
    LexItem holder;

    // Parse the first simple factor
    bool t1 = SFactor(in, line, retVal);
    if (!t1) {
        return false;
    }

    // Get the next holderen
    holder = Parser::GetNextToken(in, line);

    // Check if the holderen indicates an error
    if (holder.GetToken() == ERR) {
        // Handle unrecognized input patterns
        ParseError(line, "Wrong9");
        cout << "(" << holder.GetLexeme() << ")" << endl;
        return false;
    }

    // Push the first value onto the power stack
    powerStack.push(retVal);

    // Loop through while there are still exponentiation operators
    while (holder == POW) {
        // Parse the next simple factor
        t1 = SFactor(in, line, val2);
        if (!t1) {
            // Handle missing exponent operand
            ParseError(line, "Wrong10");
            return false;
        }

        // Push the exponent onto the power stack
        powerStack.push(val2);

        // Get the next holderen
        holder = Parser::GetNextToken(in, line);

        // Check if the holderen indicates an error
        if (holder.GetToken() == ERR) {
            // Handle unrecognized input patterns
            ParseError(line, "Wrong11");
            cout << "space" << holder.GetLexeme() << "space" << endl;
            return false;
        }
    }

    // Perform exponentiation using the values on the power stack
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

    // Push back the last holderen if it's not an exponentiation operator
    Parser::backPTok(holder);
    return true; // Return true to indicate successful parsing
}
// End of TermExpr



// Function to parse a statement
bool Stmt(istream& in, int& line) {
    bool status;
    // Get the next holderen
    LexItem t = Parser::GetNextToken(in, line);

    // Switch statement to handle different types of statements
    switch(t.GetToken()) {
        case PRINT:
            // Parse a print statement
            status = PrintStmt(in, line);
            // If parsing is successful, recursively parse the next statement
            if(status)
                status = Stmt(in, line);
            break;
        case IF:
            // Parse a block if statement
            status = BlockIfStmt(in, line);
            // If parsing is successful, recursively parse the next statement
            if(status)
                status = Stmt(in, line);
            break;
        case IDENT:
            // Push back the holderen and parse an assignment statement
            Parser::backPTok(t);
            status = AssignStmt(in, line);
            // If parsing is successful, recursively parse the next statement
            if(status)
                status = Stmt(in, line);
            break;
        default:
            // If the holderen doesn't match any statement type, return true
            Parser::backPTok(t);
            return true;
    }
    return status;
}

// Function to parse a simple statement
bool SimpleStmt(istream& in, int& line) {
    bool status;
    // Get the next holderen
    LexItem t = Parser::GetNextToken(in, line);

    // Switch statement to handle different types of simple statements
    switch(t.GetToken()) {
        case PRINT:
            // Parse a print statement
            status = PrintStmt(in, line);
            // If parsing fails, report an error
            if (!status) {
                ParseError(line, "Wrong12");
                return false;
            }
            cout << "Wrong13" << endl;
            break;
        case IDENT:
            // Push back the holderen and parse an assignment statement
            Parser::backPTok(t);
            status = AssignStmt(in, line);
            // If parsing fails, report an error
            if (!status) {
                ParseError(line, "Wrong14");
                return false;
            }
            cout << "Wrong15" << endl;
            break;
        default:
            // If the holderen doesn't match any simple statement type, return true
            Parser::backPTok(t);
            return true;
    }
    return status;
}
// Function to parse a multiplication expression
bool MultExpr(istream& in, int& line, Value& retVal) {
    // Parse the first term expression
    bool t1 = TermExpr(in, line, retVal);
    Value val2;
    LexItem holder;

    // Check if the first term expression is parsed successfully
    if (!t1) {
        return false;
    }

    // Get the next holderen
    holder = Parser::GetNextToken(in, line);

    // Check if the holderen indicates an error
    if (holder.GetToken() == ERR) {
        // Handle unrecognized input patterns
        ParseError(line, "Wrong16");
        cout << "(" << holder.GetLexeme() << ")" << endl;
        return false;
    }

    // Loop through while there are still multiplication or division operators
    while (holder == MULT || holder == DIV) {
        // Parse the next term expression
        t1 = TermExpr(in, line, val2);
        if (!t1) {
            // Handle missing operand after operator
            ParseError(line, "Wrong17");
            return false;
        }

        // Perform the operation based on the operator
        if (holder == DIV) {
            if (val2.GetReal() == 0) {
                // Handle division by zero
                ParseError(line - 1, "Wrong18");
                return false;
            }
            retVal = retVal / val2;
        } else if (holder == MULT) {
            retVal = retVal * val2;
        }

        // Get the next holderen
        holder = Parser::GetNextToken(in, line);

        // Check if the holderen indicates an error
        if (holder.GetToken() == ERR) {
            // Handle unrecognized input patterns
            ParseError(line, "Wrong19");
            cout << "space" << holder.GetLexeme() << "space" << endl;
            return false;
        }
    }

    // Push back the last holderen if it's not a multiplication or division operator
    Parser::backPTok(holder);
    return true; // Return true to indicate successful parsing
}
// End of MultExpr


// Function to parse a list of variables with optional initializations
bool VarList(istream& in, int& line, LexItem& idholder, int strlen) {
    bool status = false, exprstatus = false;
    string identstr;
    Value retVal;

    // Convert the string length to an integer
    strlen = stoi(strLen);

    // Get the next holderen
    LexItem holder = Parser::GetNextToken(in, line);

    // Check if the holderen is an identifier
    if (holder == IDENT) {
        identstr = holder.GetLexeme();
        // Check if the variable is already defined
        if (!(defVar.find(identstr)->second)) {
            defVar[identstr] = true;
        } else {
            ParseError(line, "Wrong20");
            return false;
        }
        // Add the variable to the symbol table
        SymTable[holder.GetLexeme()] = idholder.GetToken();
        // Handle CHARACTER type variables
        if (SymTable[holder.GetLexeme()] == CHARACTER) {
            string s(strlen, ' ');
            retVal = Value(s);
            TempsResults[holder.GetLexeme()] = retVal;
        }
    } else {
        ParseError(line, "Wrong21");
        return false;
    }

    // Get the next holderen
    holder = Parser::GetNextToken(in, line);

    // Check if the holderen is an assignment operator
    if (holder == ASSOP) {
        // Parse the expression
        exprstatus = Expr(in, line, retVal);
        if (!exprstatus) {
            ParseError(line, "Wrong22");
            return false;
        }
        // Convert integer value to double if the variable type is REAL
        if (idholder == REAL) {
            if (retVal.GetType() == VINT) {
                retVal = double(retVal.GetInt());
            }
        }
        // Handle CHARACTER type variables' length
        if (idholder == CHARACTER) {
            string ogString = retVal.GetString();
            int extraSpaces = strlen - ogString.length();
            if (ogString.length() > strlen) {
                retVal = Value(ogString.substr(0, strlen));
            } else {
                retVal = Value(ogString + string(extraSpaces, ' '));
            }
        }
        // Store the variable and its value in the temporary results container
        TempsResults[identstr] = retVal;

        // Get the next holderen
        holder = Parser::GetNextToken(in, line);
        // Check if the holderen is a comma, indicating more variables in the list
        if (holder == COMMA) {
            // Recursively parse the next variable in the list
            status = VarList(in, line, idholder, strlen);
        } else {
            // Push back the holderen if it's not a comma
            Parser::backPTok(holder);
            return true;
        }
    }
    // Handle the case where there's no assignment operator
    else if (holder == COMMA) {
        // Recursively parse the next variable in the list
        status = VarList(in, line, idholder, strlen);
    }
    // Handle errors
    else if (holder == ERR) {
        ParseError(line, "Wrong23");
        return false;
    } else {
        // If the holderen is neither comma nor assignment operator, push it back and return true
        Parser::backPTok(holder);
        return true;
    }
    return status;
}


// PrintStmt:= PRINT *, ExprList
bool PrintStmt(istream& in, int& line) {
    LexItem t;
    t = Parser::GetNextToken(in, line);
    ValQue = new queue<Value>;

    // Check if the holderen is DEF (Print statement)
    if(t != DEF) {
        ParseError(line, "Wrong24");
        return false;
    }

    // Check if the next holderen is COMMA (Print separator)
    t = Parser::GetNextToken(in, line);
    if(t != COMMA) {
        ParseError(line, "Wrong25");
        return false;
    }

    // Parse the expression list
    bool ex = ExprList(in, line);
    if( !ex ) {
        ParseError(line, "Wrong26");
        return false;
    }

    // Output the expressions in the queue
    while (!(*ValQue).empty()) {
        Value nextVal = (*ValQue).front();
        cout << nextVal;
        ValQue->pop();
    }
    cout << endl;
    return ex;
}// End of PrintStmt


// BlockIfStmt:= if (Expr) then {Stmt} [Else Stmt]
// SimpleIfStatement := if (Expr) Stmt
bool BlockIfStmt(istream& in, int& line) {
    bool ex=false, status ;
    LexItem t;

    // Check for left parenthesis indicating the start of the if condition
    t = Parser::GetNextToken(in, line);
    if(t != LPAREN) {
        ParseError(line, "Wrong27");
        return false;
    }

    Value retVal;
    // Parse the relational expression
    ex = RelExpr(in, line, retVal);
    if(!ex) {
        ParseError(line, "Wrong28");
        return false;
    }
    // Check for illegal operand types
    if (retVal.GetType() == VERR) {
        ParseError(line, "Wrong29");
        return false;
    }

    // Check for right parenthesis
    t = Parser::GetNextToken(in, line);
    if(t != RPAREN) {
        ParseError(line, "Wrong30");
        return false;
    }

    // If the condition evaluates to false, skip the if block
    if (retVal.GetBool() == 0) {
        while (t != ELSE && t != END) {
            t = Parser::GetNextToken(in, line);
        }
        status = Stmt(in, line);
        if(!status) {
            ParseError(line, "Wrong31");
            return false;
        }
        else
            t = Parser::GetNextToken(in, line);
        if(t != END) {
            ParseError(line, "Wrong32");
            return false;
        }

        t = Parser::GetNextToken(in, line);
        if(t == IF ) {
            return true;
        }
        Parser::backPTok(t);
        ParseError(line, "Wrong33");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != THEN) {
        Parser::backPTok(t);
        status = SimpleStmt(in, line);
        if(status) {
            return true;
        }
        else {
            ParseError(line, "Wrong34");
            return false;
        }
    }

    status = Stmt(in, line);
    if(!status) {
        ParseError(line, "Wrong35");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if( t == ELSE ) {
        status = Stmt(in, line);
        if(!status) {
            ParseError(line, "Wrong36");
            return false;
        }
        else
            t = Parser::GetNextToken(in, line);
    }
    if(t != END) {
        ParseError(line, "Wrong37");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t == IF ) {
        return true;
    }
    Parser::backPTok(t);
    ParseError(line, "Wrong38");
    return false;
}//End of IfStmt function


// Function to parse a factor
bool Factor(istream& in, int& line, int sign, Value& retVal) {
    // Get the next holderen
    LexItem holder = Parser::GetNextToken(in, line);

    // Check the type of the holderen
    if (holder == IDENT) {
        // If the holderen is an identifier
        string lexeme = holder.GetLexeme();
        // Check if the identifier is defined
        if (!(defVar.find(lexeme)->second)) {
            ParseError(line, "Wrong39");
            return false;
        }
        // Get the value associated with the identifier from the temporary results
        retVal = TempsResults[lexeme];
        // Check if the variable is initialized
        if (TempsResults[lexeme].GetType() == VERR) {
            ParseError(line, "Wrong40");
            return false;
        }
        // Apply sign if necessary
        if (sign == -1) {
            if (retVal.IsInt()) {
                retVal = -retVal.GetInt();
            } else if (retVal.IsReal()) {
                retVal = -retVal.GetReal();
            }
            if (retVal.IsString()) {
                ParseError(line, "Wrong41");
                return false;
            }
        }
        return true;
    } else if (holder == ICONST) {
        // If the holderen is an integer constant
        retVal.SetType(VINT);
        // Convert the holderen value to an integer and apply sign if necessary
        if (sign == -1) {
            retVal.SetInt(-stoi(holder.GetLexeme()));
        } else {
            retVal.SetInt(stoi(holder.GetLexeme()));
        }
        return true;
    } else if (holder == SCONST) {
        // If the holderen is a string constant
        retVal.SetType(VSTRING);
        // Set the string value to the holderen value
        retVal.SetString(holder.GetLexeme());
        return true;
    } else if (holder == RCONST) {
        // If the holderen is a real constant
        retVal.SetType(VREAL);
        // Convert the holderen value to a real number and apply sign if necessary
        if (sign == -1) {
            retVal.SetReal(-stod(holder.GetLexeme()));
        } else {
            retVal.SetReal(stod(holder.GetLexeme()));
        }
        return true;
    } else if (holder == LPAREN) {
        // If the holderen is a left parenthesis, parse an expression inside the parenthesis
        bool ex = Expr(in, line, retVal);
        if (!ex) {
            ParseError(line, "Wrong42");
            return false;
        }
        // Check if the expression is followed by a right parenthesis
        if (Parser::GetNextToken(in, line) == RPAREN) {
            return ex;
        } else {
            // If there's no right parenthesis, push back the left parenthesis holderen and return false
            Parser::backPTok(holder);
            ParseError(line, "Wrong43");
            return false;
        }
    } else if (holder.GetToken() == ERR) {
        // If the holderen is an error holderen, print an error message and return false
        ParseError(line, "Wrong44");
        cout << "space" << holder.GetLexeme() << "space" << endl;
        return false;
    }
    // If none of the above conditions are met, return false
    return false;
}


// Function to parse a variable identifier
bool Var(istream& in, int& line, LexItem& idholder) {
    // Declare variables
    string identstr;
    LexItem holder = Parser::GetNextToken(in, line);
    idholder = holder; // Assign the holderen to idholder

    // Check if the holderen is an identifier
    if (holder == IDENT) {
        identstr = holder.GetLexeme(); // Get the lexeme of the identifier
        // Check if the variable is declared
        if (!(defVar.find(identstr)->second)) {
            // Raise an error if the variable is undeclared
            ParseError(line, "Wrong45");
            return false; // Return false indicating parsing failure
        }
        return true; // Return true indicating successful parsing
    } else if (holder.GetToken() == ERR) {
        // Raise an error for unrecognized input pattern
        ParseError(line, "Wrong46");
        // Print the lexeme causing the error
        cout << "space" << holder.GetLexeme() << "space" << endl;
        return false; // Return false indicating parsing failure
    }
    return false; // Return false indicating parsing failure
}
// End of Var function

// Function to parse an assignment statement
bool AssignStmt(istream& in, int& line) {
    // Declare variables
    bool varstatus = false, status = false;
    LexItem t;
    int assignLine = line; // Store the line number of the assignment statement

    // Parse the left-hand side variable
    varstatus = Var(in, line, t);

    // Check if the left-hand side variable is parsed successfully
    if (varstatus) {
        LexItem holder = Parser::GetNextToken(in, line);

        // Check if the next holderen is an assignment operator
        if (holder == ASSOP) {
            // Parse the expression on the right-hand side of the assignment
            Value retVal;
            status = Expr(in, line, retVal);
            if (!status) {
                ParseError(line, "Wrong48");
                return status;
            }
            // Handle special case for CHARACTER type variables
            if (SymTable[t.GetLexeme()] == CHARACTER) {
                // Check if the expression is of type string
                if (retVal.GetType() != VSTRING) {
                    ParseError(assignLine, "Wrong49");
                    return false;
                }
                // Adjust the length of the string if necessary
                string ogString = retVal.GetString();
                int extraSpaces = stoi(strLen) - ogString.length();
                if (ogString.length() > stoi(strLen)) {
                    retVal = Value(ogString.substr(0, stoi(strLen)));
                } else {
                    retVal = Value(ogString + string(extraSpaces, ' '));
                }
            }
            // Store the result in the temporary results container
            TempsResults[t.GetLexeme()] = retVal;
            // Check if the result type is valid
            if (TempsResults[t.GetLexeme()].GetType() == VERR) {
                ParseError(assignLine, "Wrong50");
                return false;
            }
        } else if (holder.GetToken() == ERR) {
            ParseError(line, "Wrong51");
            cout << "space" << t.GetLexeme() << "space" << endl;
            return false;
        } else {
            ParseError(line, "Wrong51");
            return false;
        }
    } else {
        ParseError(line, "Wrong52");
        return false;
    }
    return status; // Return the parsing status
}
// End of AssignStmt


// Function to parse a list of expressions
bool ExprList(istream& in, int& line) {
    // Declare variables
    bool status = false;
    Value retVal;

    // Parse the first expression in the list
    status = Expr(in, line, retVal);

    // Check if the first expression is parsed successfully
    if (!status) {
        ParseError(line, "Wrong53");
        return false;
    }
    // Push the parsed expression onto the value queue
    ValQue->push(retVal);

    // Get the next holderen
    LexItem holder = Parser::GetNextToken(in, line);
    if (holder == COMMA) {
        // If the next holderen is a comma, recursively parse the next expression in the list
        status = ExprList(in, line);
    } else if (holder.GetToken() == ERR) {
        // Handle unrecognized input patterns
        ParseError(line, "Wrong54");
        cout << "space" << holder.GetLexeme() << "space" << endl;
        return false;
    } else {
        // If the next holderen is not a comma, push it back and return true
        Parser::backPTok(holder);
        return true;
    }
    return status; // Return the parsing status
}
// End of ExprList


// Function to parse a relational expression
bool RelExpr(istream& in, int& line, Value& retVal) {
    // Parse the first expression
    bool t1 = Expr(in, line, retVal);
    LexItem holder;

    // Check if the first expression is parsed successfully
    if (!t1) {
        return false;
    }

    // Get the next holderen
    holder = Parser::GetNextToken(in, line);

    // Check if the holderen indicates a relational operator or an error
    if (holder.GetToken() == ERR) {
        // Handle unrecognized input patterns
        ParseError(line, "Wrong55");
        cout << "(" << holder.GetLexeme() << ")" << endl;
        return false;
    }

    // If the holderen is a relational operator
    if (holder == EQ || holder == LTHAN || holder == GTHAN) {
        // Store the operator
        string op = holder.GetLexeme();

        // Parse the second expression
        Value val2;
        t1 = Expr(in, line, val2);
        if (!t1) {
            // Handle missing operand after operator
            ParseError(line, "Wrong56");
            return false;
        }

        // Perform the relational operation based on the operator
        if (holder == EQ) {
            retVal = retVal == val2;
        } else if (holder == LTHAN) {
            retVal = retVal < val2;
        } else if (holder == GTHAN) {
            retVal = retVal > val2;
        }
    }
    return true; // Return true to indicate successful parsing
}
// End of RelExpr


// Function to parse an expression
bool Expr(istream& in, int& line, Value& retVal) {
    // Parse the first multiplication expression
    bool t1 = MultExpr(in, line, retVal);
    LexItem holder;

    // Check if the first multiplication expression is parsed successfully
    if (!t1) {
        return false;
    }

    // Get the next holderen
    holder = Parser::GetNextToken(in, line);

    // Check if the holderen indicates an error
    if (holder.GetToken() == ERR) {
        // Handle unrecognized input patterns
        ParseError(line, "Wrong57");
        cout << "space" << holder.GetLexeme() << "space" << endl;
        return false;
    }

    // Loop through while there are still addition, subtraction, or concatenation operators
    while (holder == PLUS || holder == MINUS || holder == CAT) {
        // Store the operator
        string op = holder.GetLexeme();

        // Parse the next multiplication expression
        Value val2;
        t1 = MultExpr(in, line, val2);
        if (!t1) {
            // Handle missing operand after operator
            ParseError(line, "Wrong58");
            return false;
        }

        // Perform the operation based on the operator
        if (holder == PLUS) {
            retVal = retVal + val2;
        } else if (holder == MINUS) {
            retVal = retVal - val2;
        } else if (holder == CAT) {
            retVal = retVal.Catenate(val2);
        }

        // Get the next holderen
        holder = Parser::GetNextToken(in, line);

        // Check if the holderen indicates an error
        if (holder.GetToken() == ERR) {
            // Handle unrecognized input patterns
            ParseError(line, "Wrong59");
            cout << "space" << holder.GetLexeme() << "space" << endl;
            return false;
        }
    }

    // Push back the last holderen if it's not an operator
    Parser::backPTok(holder);
    return true; // Return true to indicate successful parsing
}
// End of Expr

// Function to parse a declaration statement
bool Decl(istream& in, int& line) {
    bool status = false;
    LexItem holder;

    // Get the next holderen
    LexItem t = Parser::GetNextToken(in, line);

    // Check if the holderen represents a valid type (INTEGER, REAL, CHARACTER)
    if (t == INTEGER || t == REAL || t == CHARACTER) {
        holder = t;
        holder = Parser::GetNextToken(in, line);

        // Check if the holderen after the type is a double colon (::)
        if (holder.GetToken() == DCOLON) {
            // Parse the list of variables
            status = VarList(in, line, t);
            if (status) {
                // Recursive call to parse subsequent declarations
                status = Decl(in, line);
                if (!status) {
                    ParseError(line, "Wrong60");
                    return false;
                }
                return status;
            } else {
                ParseError(line, "Wrong61");
                return false;
            }
        }
        // Handle the case for CHARACTER type with length specified
        else if (t == CHARACTER && holder.GetToken() == LPAREN) {
            holder = Parser::GetNextToken(in, line);
            // Check if the next holderen is LEN
            if (holder.GetToken() == LEN) {
                holder = Parser::GetNextToken(in, line);
                // Check if the next holderen is ASSOP
                if (holder.GetToken() == ASSOP) {
                    holder = Parser::GetNextToken(in, line);
                    // Check if the next holderen is an integer constant
                    if (holder.GetToken() == ICONST) {
                        // Store the length of the string
                        strLen = holder.GetLexeme();
                        holder = Parser::GetNextToken(in, line);
                        // Check if the next holderen is a right parenthesis
                        if (holder.GetToken() == RPAREN) {
                            holder = Parser::GetNextToken(in, line);
                            // Check if the next holderen is a double colon (::)
                            if (holder.GetToken() == DCOLON) {
                                // Parse the list of variables
                                status = VarList(in, line, t);
                                if (status) {
                                    // Recursive call to parse subsequent declarations
                                    status = Decl(in, line);
                                    if (!status) {
                                        ParseError(line, "Wrong62");
                                        return false;
                                    }
                                    return status;
                                } else {
                                    ParseError(line, "Wrong63");
                                    return false;
                                }
                            }
                        } else {
                            ParseError(line, "Wrong64");
                            return false;
                        }
                    } else {
                        ParseError(line, "Wrong65");
                        return false;
                    }
                }
            }
        } else {
            ParseError(line, "Wrong66");
            return false;
        }
    }

    // Push back the holderen if it doesn't represent a valid type
    Parser::backPTok(t);
    return true;
}
// Function to parse a signed factor
bool SFactor(istream& in, int& line, Value& retVal) {
    // Get the next holderen
    LexItem t = Parser::GetNextToken(in, line);

    // Variable to track the sign of the factor
    int sign = 0;

    // Check if the holderen represents a sign (+ or -)
    if (t == MINUS) {
        // If the holderen is a minus sign, set the sign variable to -1
        sign = -1;
    } else if (t == PLUS) {
        // If the holderen is a plus sign, set the sign variable to 1
        sign = 1;
    } else {
        // If the holderen is not a sign, push it back to the input stream
        Parser::backPTok(t);
    }

    // Parse the factor with the determined sign
    bool status = Factor(in, line, sign, retVal);

    // Return the parsing status
    return status;
}
// End of SFactor