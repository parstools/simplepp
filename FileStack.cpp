#include <fstream>
#include <utility>
#include <iostream>
#include "FileStack.h"

using namespace std;

void FileStack::preprocess(std::string filename) {
    stack.emplace_back();
    stack[0].read(std::move(filename));
    while(stack.size()>0) {
        auto & top = stack.back();
        if (top.isDirective()) {
            auto dt = top.directiveType();
            switch(dt) {
                case File::dtInclude:
                    stack.emplace_back();
                    //stack.back().read(top.askInclude());
                    break;
               }
        }
        else
           cout << top.currLine() << endl;
        top.currentLine++;
        if (stack.back().eof())
            stack.pop_back();
    }
}


std::string File::currLine() {
    return lines[currentLine];
}

bool File::isDirective() {
    std::string line = currLine();
    line = trimLeft(line);
    return !line.empty() and line[0] == '#';
}

File::DirectiveType File::directiveType() {
    std::string line = currLine();
    line = trimLeft(line);
    if (line.empty() || line[0] != '#')
        return dtNone;
    size_t pos = 1;
    while (isalnum(line[pos])) pos++;
    string word = line.substr(1,pos-1);
    if (word=="include") return dtInclude;
    else if (word=="define") return dtDefine;
    else if (word=="ifdef") return dtIfdef;
    else if (word=="ifndef") return dtIfndef;
    else if (word=="endif") return dtEndif;
    else if (word=="else") return dtElse;
    else if (word=="if") return dtIf;
    else {
        throw runtime_error("bad directive: "+line);
    }
}

string File::trimLeft(const string& str)
{
    const auto strBegin = str.find_first_not_of(" \t");
    if (strBegin == string::npos)
        return ""; // no content
    return str.substr(strBegin, str.length() - strBegin);
}

string File::trimRight(const string& str)
{
    const auto strEnd = str.find_last_not_of(" \t\r");
    if (strEnd == string::npos)
        return ""; // no content
    return str.substr(0, strEnd + 1);
}

string File::trim(const string& str) {
    return trimLeft(trimRight(str));
}

void File::read(std::string filename) {
    canonicalPath = std::filesystem::canonical(filename);
    std::ifstream infile(canonicalPath);
    lines.clear();
    for( std::string line; getline( infile, line ); )
        lines.push_back(trimRight(line));
}

bool File::eof() {
    return currentLine>=lines.size();
}

