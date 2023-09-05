#include <fstream>
#include <utility>
#include <iostream>
#include "FileStack.h"

using namespace std;

void FileStack::process(int h) {
    for (int i=0; i<stack[h].lines.size(); i++) {
        stack[h].currentLine = i;
        auto directive = stack[h].directive();
        auto dt = directive.dt;
        switch (dt) {
            case DirectiveType::dtInclude: {
                std::filesystem::path path = stack[h].canonicalPath.parent_path() / directive.include;
                path = std::filesystem::canonical(path);
                for (int j = 0; j <= h; j++) {
                    if (stack[j].canonicalPath == path)
                        throw runtime_error("include recursive " + path.string());
                }
                if (regionVisible()) {
                    stack.emplace_back();
                    stack.back().read(path);
                    process(h + 1);
                }
            }
                break;
            case DirectiveType::dtDefine:
                if (regionVisible())
                    variables[directive.variable] = directive.variable2;
                break;
            case DirectiveType::dtUndef:
                if (regionVisible())
                    variables.erase(directive.variable);
                break;
            case DirectiveType::dtIfdef:
                stackRegions.push_back(variables.find(directive.variable) != variables.end());
                break;
            case DirectiveType::dtIfndef:
                stackRegions.push_back(variables.find(directive.variable) == variables.end());
                break;
            case DirectiveType::dtElse:
                stackRegions.back() = !stackRegions.back();
                break;
            case DirectiveType::dtEndif:
                if (stackRegions.empty())
                    throw runtime_error("too much #endif");
                stackRegions.pop_back();
                break;
            case DirectiveType::dtIf:
                stackRegions.push_back(condition(directive));
                break;
            case DirectiveType::dtElif:
                stackRegions.back() = condition(directive);
                break;
            case DirectiveType::dtNone:
                if (regionVisible())
                    cout << stack[h].currLine() << endl;
                break;
        }
    }
}

void FileStack::preprocess(std::string filename) {
    stack.emplace_back();
    stack[0].read(std::move(filename));
    process(0);
}

bool FileStack::regionVisible() {
    for (bool b: stackRegions)
        if (!b)
            return false;
    return true;
}

bool FileStack::condition(Directive &directive) {
    if (variables.find(directive.variable) == variables.end())
        throw runtime_error("variable " + directive.variable + " undefined");
    int n1,n2;
    string value = variables[directive.variable];
    if (value.empty())
        n1 = -1;
    else
        n1 = stoi(value);

    if (directive.var2Type == WordType::wtEmpty)
        n2 = -1;
    else
        n2 = stoi(directive.variable2);
    switch(directive.relOp) {
        case RelOp::opEq: return n1 == n2;
        case RelOp::opNe: return n1 != n2;
        case RelOp::opGt: return n1 > n2;
        case RelOp::opGe: return n1 >= n2;
        case RelOp::opLt: return n1 < n2;
        case RelOp::opLe: return n1 <= n2;
    }
    return false;
}


std::string File::currLine() {
    return lines[currentLine];
}

Directive File::directive() {
    std::string line = currLine();
    Directive result;
    result.parse(line);
    return result;
}

string trimLeft(const string& str)
{
    const auto strBegin = str.find_first_not_of(" \t");
    if (strBegin == string::npos)
        return ""; // no content
    return str.substr(strBegin, str.length() - strBegin);
}

string trimRight(const string& str)
{
    const auto strEnd = str.find_last_not_of(" \t\r");
    if (strEnd == string::npos)
        return ""; // no content
    return str.substr(0, strEnd + 1);
}

string trim(const string& str) {
    return trimLeft(trimRight(str));
}

void File::read(std::string filename) {
    canonicalPath = std::filesystem::canonical(filename);
    std::ifstream infile(canonicalPath);
    lines.clear();
    for( std::string line; getline( infile, line ); )
        lines.push_back(trimRight(line));
}


size_t Directive::skipBlank(std::string line, size_t start) {
    while (iswblank(line[start])) start++;
    return start;
}

size_t Directive::nextIdent(std::string line, size_t start) {
    while (isalnum(line[start])) start++;
    return start;
}

size_t Directive::nextNumber(std::string line, size_t start) {
    while (isdigit(line[start])) start++;
    return start;
}

size_t Directive::nextQuote(std::string line, size_t start) {
    while (line[start] != '\"') start++;
    return start + 1;
}

size_t Directive::nextOther(std::string line, size_t start) {
    while (!isalnum(line[start]) && !iswblank(line[start]) && line[start] != '\"') start++;
    return start;
}

size_t Directive::nextWord(std::string line, size_t start) {
    if (isalpha(line[start]))
        return nextIdent(line, start + 1);
    else if (isdigit(line[start]))
        return nextNumber(line, start + 1);
    else if (line[start] == '"')
        return nextQuote(line, start + 1);
    else
        return nextOther(line, start + 1);
}

vector<pair<string, WordType>> Directive::parseRaw(string line) {
    vector<pair<string, WordType>> result;
    line = trimLeft(line);
    if (line.empty() || line[0] != '#')
        return result;
    size_t pos = 1;
    pos = skipBlank(line, pos);
    while (pos < line.size()) {
        size_t pos0 = pos;
        pos = nextWord(line, pos);
        string word = line.substr(pos0, pos - pos0);
        WordType wt;
        if (isalpha(word[0]))
            wt = WordType::wtIdent;
        else if (isdigit(word[0]))
            wt = WordType::wtNumber;
        else if (word[0] == '\"')
            wt = WordType::wtQuote;
        else
            wt = WordType::wtOther;
        pos = skipBlank(line, pos);
        result.emplace_back(word, wt);
    }
    return result;
}



void Directive::parse(string line) {
    auto wordList = parseRaw(line);
    if (wordList.empty()) {
        dt = DirectiveType::dtNone;
        return;
    }
    if (wordList[0].second != WordType::wtIdent)
        throw runtime_error("bad directive " + line);
    string word = wordList[0].first;
    if (word == "include")
        dt = DirectiveType::dtInclude;
    else if (word == "define") dt = DirectiveType::dtDefine;
    else if (word == "undef") dt = DirectiveType::dtUndef;
    else if (word == "ifdef") dt = DirectiveType::dtIfdef;
    else if (word == "ifndef") dt = DirectiveType::dtIfndef;
    else if (word == "endif") dt = DirectiveType::dtEndif;
    else if (word == "else") dt = DirectiveType::dtElse;
    else if (word == "elif") dt = DirectiveType::dtElif;
    else if (word == "if") dt = DirectiveType::dtIf;
    else {
        throw runtime_error("bad directive: " + line);
    }
    switch (dt) {
        case DirectiveType::dtEndif:
        case DirectiveType::dtElse:
            if (wordList.size() != 1)
                throw runtime_error("bad directive: " + line);
            break;
        case DirectiveType::dtDefine:
            if (wordList.size() != 2 && wordList.size() != 3)
                throw runtime_error("bad directive: " + line);
            if (wordList[1].second != WordType::wtIdent)
                throw runtime_error("bad directive: " + line);
            variable = wordList[1].first;
            varType = WordType::wtIdent;
            if (wordList.size() == 2) {
                variable2 = "";
                var2Type = WordType::wtEmpty;
            } else {
                if (wordList[2].second != WordType::wtNumber)
                    throw runtime_error("in this version preprocessor variables can be only numbers without sign or empty: " + line);
                variable2 = wordList[2].first;
                var2Type = WordType::wtNumber;
            }
            break;
        case DirectiveType::dtUndef:
        case DirectiveType::dtIfdef:
        case DirectiveType::dtIfndef:
            if (wordList.size() != 2)
                throw runtime_error("bad directive: " + line);
            if (wordList[1].second != WordType::wtIdent)
                throw runtime_error("bad directive: " + line);
            variable = wordList[1].first;
            break;
        case DirectiveType::dtInclude:
            if (wordList.size() != 2)
                throw runtime_error("bad directive: " + line);
            if (wordList[1].second != WordType::wtQuote)
                throw runtime_error("bad directive: " + line);
            include = unquote(wordList[1].first);
            break;
        case DirectiveType::dtIf:
        case DirectiveType::dtElif:
            if (wordList.size() != 4)
                throw runtime_error("bad directive: " + line);
            if (wordList[1].second == WordType::wtOther)
                throw runtime_error("bad directive: " + line);
            variable = wordList[1].first;
            varType = wordList[1].second;

            if (wordList[2].second != WordType::wtOther)
                throw runtime_error("bad directive: " + line);
            string op = wordList[2].first;
            if (op=="==")
                relOp = RelOp::opEq;
            else if (op=="!=")
                relOp = RelOp::opNe;
            else if (op==">")
                relOp = RelOp::opGt;
            else if (op==">=")
                relOp = RelOp::opGe;
            else if (op=="<")
                relOp = RelOp::opLt;
            else if (op=="<=")
                relOp = RelOp::opLe;
            else
                throw runtime_error("bad directive: " + line);

            if (wordList[3].second == WordType::wtOther)
                throw runtime_error("bad directive: " + line);
            variable2 = wordList[3].first;
            var2Type = wordList[3].second;
            break;
    }
}

std::string Directive::unquote(std::string quoted) {
    return quoted.substr(1, quoted.length()-2);
}


