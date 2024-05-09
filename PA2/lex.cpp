// Md Uddin
// NJIT ID: 31569771
// UCID: Msu7
// CS280-012
#include <algorithm>
#include "lex.h"

LexItem id_or_kw(const string& lexeme, int linenum) {
    string upper = lexeme;
    string lower = lexeme;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    static map<string, Token> tokenKeywords_upper {
            {"PROGRAM", PROGRAM}, {"IF", IF}, {"ELSE", ELSE}, {"PRINT", PRINT},
            {"INTEGER", INTEGER}, {"REAL", REAL}, {"CHARACTER", CHARACTER},
            {"END", END}, {"THEN", THEN}, {"LEN", LEN}
    };
    static map<string, Token> tokenKeywords_lower {
            {"program", PROGRAM}, {"if", IF}, {"else", ELSE}, {"print", PRINT},
            {"integer", INTEGER}, {"real", REAL}, {"character", CHARACTER},
            {"end", END}, {"then", THEN}, {"len", LEN}
    };

    auto it = tokenKeywords_upper.find(upper);
    if (it != tokenKeywords_upper.end()) {
        return LexItem(it->second, lexeme, linenum);
    } else if ((it = tokenKeywords_lower.find(lower)) != tokenKeywords_lower.end()) {
        return LexItem(it->second, lexeme, linenum);
    } else {
        return LexItem(IDENT, lexeme, linenum);
    }
}

ostream& operator<<(ostream& out, const LexItem& tok) {
    switch (tok.GetToken()) {
        case IF:
            out << "IF";
            break;
        case ELSE:
            out << "ELSE";
            break;
        case PRINT:
            out << "PRINT";
            break;
        case INTEGER:
            out << "INTEGER";
            break;
        case REAL:
            out << "REAL";
            break;
        case CHARACTER:
            out << "CHARACTER";
            break;
        case END:
            out << "END";
            break;
        case THEN:
            out << "THEN";
            break;
        case PROGRAM:
            out << "PROGRAM";
            break;
        case LEN:
            out << "LEN";
            break;
        case IDENT:
            out << "IDENT";
            break;
        case ICONST:
            out << "ICONST";
            break;
        case RCONST:
            out << "RCONST";
            break;
        case SCONST:
            out << "SCONST";
            break;
        case BCONST:
            out << "BCONST";
            break;
        case PLUS:
            out << "PLUS";
            break;
        case MINUS:
            out << "MINUS";
            break;
        case MULT:
            out << "MULT";
            break;
        case DIV:
            out << "DIV";
            break;
        case ASSOP:
            out << "ASSOP";
            break;
        case EQ:
            out << "EQ";
            break;
        case POW:
            out << "POW";
            break;
        case GTHAN:
            out << "GTHAN";
            break;
        case LTHAN:
            out << "LTHAN";
            break;
        case CAT:
            out << "CAT";
            break;
        case COMMA:
            out << "COMMA";
            break;
        case LPAREN:
            out << "LPAREN";
            break;
        case RPAREN:
            out << "RPAREN";
            break;
        case DOT:
            out << "DOT";
            break;
        case DCOLON:
            out << "DCOLON";
            break;
        case DEF:
            out << "DEF";
            break;
        case ERR:
            out << "ERR";
            break;
        case DONE:
            out << "DONE";
            break;
    }
    if (tok.GetToken() == ICONST || tok.GetToken() == RCONST || tok.GetToken() == BCONST) {
        out << ": (" << tok.GetLexeme() << ")";
    }
    if (tok.GetToken() == IDENT) {
        out << ": '" << tok.GetLexeme() << "'";
    }
    if (tok.GetToken() == SCONST) {
        out << ": \"" << tok.GetLexeme() << "\"";
    }
    out << endl;
    return out;
}

LexItem getNextToken(istream& in, int& linenum) {
    enum tokenState {START, INID, INSTRING, ININT, INREAL, INCOMMENT} lexState = START;
    string lexeme;
    char character, nextChar, quoteMark;
    bool checker = false;
    while (in.get(character)) {
        switch (lexState) {
            case START:
                if (character == '\n') {
                    if (in.peek() != EOF) {
                        linenum++;
                    }
                    continue;
                }
                if(character == '\r'){
                    continue;
                }
                if (isspace(character)) { if(in.peek() == EOF){linenum--;} continue; }
                lexeme = character;
                if (isdigit(character)) { lexState = ININT; continue;}
                else if (isalpha(character)) { lexState = INID; continue;}
                else if (character == '\'' || character == '\"') { lexState = INSTRING; lexeme += (char)in.peek(); continue;}
                else if (character == '!') {
                    lexState = INCOMMENT;
                }
                else if (character == ':') {
                    if (in.peek() == ':') {
                        in.get(character);
                        return LexItem(DCOLON, lexeme, linenum);
                    }
                    return LexItem(ERR, lexeme, linenum);
                }
                else if (character == '.') {
                    lexState = INREAL;
                    checker = true;
                    continue;
                }
                else {
                    if (character == '+') { return LexItem(PLUS, lexeme, linenum); }
                    if (character == '-') { return LexItem(MINUS, lexeme, linenum); }
                    if (character == '*') {
                        if (in.peek() == '*') {
                            in.get(character);
                            return LexItem(POW, lexeme, linenum);
                        }
                        else if (in.peek() == ',') {
                            return LexItem(DEF, lexeme, linenum);
                        }
                        else {
                            return LexItem(MULT, lexeme, linenum);
                        }
                    }
                    if (character == '/') {
                        if (in.peek() == '/') {
                            in.get(character);
                            return LexItem(CAT, lexeme, linenum);
                        } else {
                            return LexItem(DIV, lexeme, linenum);
                        }
                    }
                    if (character == '=') {
                        if (in.peek() == '=') {
                            in.get(character);
                            return LexItem(EQ, lexeme, linenum);
                        } else {
                            return LexItem(ASSOP, lexeme, linenum);
                        }
                    }
                    if (character == '>') { return LexItem(GTHAN, lexeme, linenum); }
                    if (character == '<') { return LexItem(LTHAN, lexeme, linenum); }
                    if (character == '(') { return LexItem(LPAREN, lexeme, linenum); }
                    if (character == ')') { return LexItem(RPAREN, lexeme, linenum); }
                    if (character == ',') {
                        return LexItem(COMMA, lexeme, linenum);
                    }
                    else {
                        return LexItem(ERR, lexeme, linenum);
                    }
                }
                break;
            case INCOMMENT:
                if (character == '\n') {
                    linenum++;
                    lexState = START;
                }
                break;
            case INID:
                if (isalnum(character) || character == '_') {
                    lexeme += character;
                    if (isalnum(in.peek()) || in.peek() == '_') {
                        continue;
                    }
                    else {
                        LexItem result = id_or_kw(lexeme, linenum);
                        return result;
                    }
                }
                else {
                    in.putback(character);
                    return id_or_kw(lexeme, linenum);
                }
                continue;
            case INSTRING:
                quoteMark = lexeme[0];
                while (in.get(nextChar)) {
                    if (nextChar == '\n') {
                        return LexItem(ERR, lexeme , linenum);
                    }
                    lexeme += nextChar;
                    if (nextChar == quoteMark) {
                        return LexItem(SCONST, lexeme.substr(1,lexeme.length()-2), linenum);
                    }
                }
                return LexItem(ERR, lexeme, linenum);
            case ININT:
                if (isdigit(character)) {
                    lexeme += character;
                }
                else if (character == '.') {
                    checker = true;
                    lexeme += character;
                    lexState = INREAL;
                    continue;
                }
                else {
                    in.putback(character);
                    return LexItem(ICONST, lexeme, linenum);
                }
                continue;
            case INREAL:
                while (isdigit(character) || (character == '.' && checker)) {
                    lexeme += character;
                    in.get(character);
                    if (character == '.') {
                        lexeme += character;
                        return LexItem(ERR, lexeme, linenum);
                    }
                    checker = isdigit(in.peek());
                }
                if (lexeme.back() == '.') {
                    in.putback(character);
                    return LexItem(DOT, ".", linenum);
                }
                in.putback(character);
                return LexItem(RCONST, lexeme, linenum);
        }
    }
    if( in.eof() )
        return LexItem(DONE, "", linenum);
    return LexItem(ERR, "some strange I/O error", linenum);
}