// Md Uddin
// NJIT ID: 31569771
// UCID: Msu7
// CS280-012
#include <fstream>
#include "lex.h"

using namespace std;

map<string, Token> tokenKeywords_upper_modified {
        {"PROGRAM", PROGRAM}, {"IF", IF}, {"ELSE", ELSE}, {"PRINT", PRINT},
        {"INTEGER", INTEGER}, {"REAL", REAL}, {"CHARACTER", CHARACTER},
        {"END", END}, {"THEN", THEN}, {"LEN", LEN}
};
map<string, Token> tokenKeywords_lower_modified {
        {"program", PROGRAM}, {"if", IF}, {"else", ELSE}, {"print", PRINT},
        {"integer", INTEGER}, {"real", REAL}, {"character", CHARACTER},
        {"end", END}, {"then", THEN}, {"len", LEN}
};

void getFileStatus(istream& file, string& filename) {
    if (!file) {
        cerr << "CANNOT OPEN THE FILE " << filename << endl;
        exit(1);
    }
    if (file.peek() == EOF) {
        cerr << "Empty File." << endl;
        exit (1);
    }
}

int main(int argc, char* argv[]) {
    int nonFlagArg = 0;
    int totalTokens = 0;
    int lineNumber = 1;
    LexItem token;
    string filename;

    map<int,int> integerConstants;
    map<string,int> realConstants;
    map<string,int>::iterator reals;
    map<string,int> stringLiterals;
    map<string,int>::iterator strings;
    map<string,int>::iterator key;
    map<string,int> keywords_modified;
    map<string,int> keywords_upper_modified;
    map<string,int> keywords_lower_modified;
    map<string,int> identifiers;
    map<int,int>::iterator it;
    map<string,bool>  flag_arguments{
            {"-all", false},
            {"-int", false},
            {"-real", false},
            {"-str", false},
            {"-id", false},
            {"-kw", false}
    };
    if (argc == 1) {
        cerr << "NO SPECIFIED INPUT FILE." << endl;
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        string argument = argv[i];
        map<string, bool>::iterator it = flag_arguments.find(argument);
        if (it != flag_arguments.end()) {
            it->second = true;
            continue;
        }
        else if(argument[0] == '-'){
            cerr << "UNRECOGNIZED FLAG " << "{" << argument << "}" << endl;
            return 0;
        }
        nonFlagArg++;
    }
    if (nonFlagArg != 1) {
        cerr << "ONLY ONE FILE NAME IS ALLOWED." << endl;
        exit (1);
    }
    filename = argv[1];
    ifstream file(filename.c_str());
    getFileStatus(file, filename);

    while (true) {
        token = getNextToken(file, lineNumber);
        if (token.GetToken() == ERR || token.GetToken() == DONE) {
            break;
        }
        totalTokens++;
        if (flag_arguments["-all"]) {
            cout << token;
        }
        if (token.GetToken() == ICONST) {
            int value = stoi(token.GetLexeme());
            integerConstants[value]++;
        }
        else if (token.GetToken() == IDENT) {
            identifiers[token.GetLexeme()]++;
        }
        else if (token.GetToken() == RCONST) {
            realConstants[token.GetLexeme()]++;
        }
        else if (token.GetToken() == SCONST) {
            stringLiterals[token.GetLexeme()]++;
        }
        else if (token.GetToken() == tokenKeywords_upper_modified[token.GetLexeme()]) {
            keywords_modified[token.GetLexeme()]++;
        }
        else if (token.GetToken() == tokenKeywords_lower_modified[token.GetLexeme()]) {
            keywords_modified[token.GetLexeme()]++;
        }
    }
    if (token.GetToken() ==  ERR) {
        cerr << "Error in line " << lineNumber << ": Unrecognized Lexeme {" << token.GetLexeme() << "}" << endl;
        exit(1);
    }
    cout << endl;
    cout << "Lines: " << lineNumber << endl;
    cout << "Total Tokens: " << totalTokens << endl;
    cout << "Identifiers: " << identifiers.size() << endl;
    cout << "Integers: " << integerConstants.size() << endl;
    cout << "Reals: " << realConstants.size() << endl;
    cout << "Strings: " << stringLiterals.size() << endl;

    map<string,int>::iterator id;
    if (flag_arguments["-id"]) {
        cout << "IDENTIFIERS:" << endl;
        for (id = identifiers.begin(); id != identifiers.end(); id++) {
            cout << id->first << " (" << id->second << ")";
            if (next(id) != identifiers.end()) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    if (flag_arguments["-kw"]) {
        cout << "KEYWORDS:" << endl;
        if (keywords_modified.size() > 1) {
            for (auto key_up = keywords_modified.begin(); key_up != keywords_modified.end(); key_up++) {
                cout << key_up->first << " (" << key_up->second << ")";
                if (next(key_up) != keywords_modified.end()) {
                    cout << ", ";
                }
            }
        }
        if (keywords_lower_modified.size() > 1) {
            for (auto key_low = keywords_modified.begin(); key_low != keywords_modified.end(); key_low++) {
                cout << key_low->first << " (" << key_low->second << ")";
                if (next(key_low) != keywords_modified.end()) {
                    cout << ", ";
                }
            }
        }
        cout << endl;
    }
    if (flag_arguments["-int"]) {
        cout << "INTEGERS:" << endl;
        for (it = integerConstants.begin(); it != integerConstants.end(); it++) {
            cout << it->first;
            if (next(it) != integerConstants.end()) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    if (flag_arguments["-real"]) {
        cout << "REALS:" << endl;
        for (reals = realConstants.begin(); reals != realConstants.end(); reals++) {
            if (reals->first[0] == '.') {
                cout << "0" << reals->first;
            }
            else {
                cout << reals->first;
            }
            if (next(reals) != realConstants.end()) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    if (flag_arguments["-str"]) {
        cout << "STRINGS:" << endl;
        for (strings = stringLiterals.begin(); strings != stringLiterals.end(); strings++) {
            cout << "\"" << strings->first << "\"";
            if (next(strings) != stringLiterals.end()) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    return 0;
}