#include <fstream>
#include <utility>
#include <iostream>
#include "FileStack.h"

using namespace std;

void FileStack::process(int h) {
    for (int i=0; i<stack[h].lines.size(); i++) {
        stack[h].currentLine = i;
        auto directive = stack[h].directive();
        if (directive.dt != DirectiveType::dtNone) {
            auto dt = directive.dt;
            switch(dt) {
                case DirectiveType::dtInclude:
                    std::filesystem::path path = stack[h].canonicalPath.parent_path() / directive.include;
                    path = std::filesystem::canonical(path);
                    for (int j=0; j<=h; j++) {
                        if (stack[j].canonicalPath == path)
                            throw runtime_error("include recursive "+ path.string());
                    }
                    stack.emplace_back();
                    stack.back().read(path);
                    process(h+1);
            }
        } else
            cout << stack[h].currLine() << endl;
    }
}

void FileStack::preprocess(std::string filename) {
    stack.emplace_back();
    stack[0].read(std::move(filename));
    process(0);
}


std::string File::currLine() {
    return lines[currentLine];
}

Directive File::directive() {
    std::string line = currLine();
    line = trimLeft(line);
    Directive result;
    if (line.empty() || line[0] != '#')
        return result;
    size_t pos = 1;
    while (isalnum(line[pos])) pos++;
    string word = line.substr(1,pos-1);
    if (word == "include") {
        result.dt = DirectiveType::dtInclude;
        pos = line.find('\"');
        if (pos == line.npos)
            throw runtime_error("no open quote include: " + line);
        int pos2 = line.find('\"', pos + 1);
        if (pos2 == line.npos)
            throw runtime_error("no close quote include: " + line);
        result.include = line.substr(pos+1, pos2 - pos-1);
    } else if (word == "define") result.dt = DirectiveType::dtDefine;
    else if (word == "undef") result.dt = DirectiveType::dtUndef;
    else if (word == "ifdef") result.dt = DirectiveType::dtIfdef;
    else if (word == "ifndef") result.dt = DirectiveType::dtIfndef;
    else if (word == "endif") result.dt = DirectiveType::dtEndif;
    else if (word == "else") result.dt = DirectiveType::dtElse;
    else if (word == "elif") result.dt = DirectiveType::dtElif;
    else if (word == "if") result.dt = DirectiveType::dtIf;
    else {
        throw runtime_error("bad directive: "+line);
    }
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


vector<pair<string, WordType>> Directive::parseRaw(string line) {
    vector<pair<string, WordType>> result;
    line = trimLeft(line);
    if (line.empty() || line[0] != '#')
        return result;
    return result;
}
