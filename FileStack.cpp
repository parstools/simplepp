#include <fstream>
#include <utility>
#include "FileStack.h"

using namespace std;

void FileStack::preprocess(std::string filename) {
    stack.emplace_back();
    stack[0].read(std::move(filename));
    while(stack.size()>0) {
        auto & top = stack.back();
        top.currentLine++;
        if (top.eof())
            stack.pop_back();
    }
}

bool FileStack::File::isDirective(std::string line) {
    line = trimLeft(line);
    return !line.empty() and line[0]=='#';
}

string FileStack::File::trimLeft(const string& str)
{
    const auto strBegin = str.find_first_not_of(" \t");
    if (strBegin == string::npos)
        return ""; // no content
    return str.substr(strBegin, str.length() - strBegin);
}

string FileStack::File::trimRight(const string& str)
{
    const auto strEnd = str.find_last_not_of(" \t\r");
    if (strEnd == string::npos)
        return ""; // no content
    return str.substr(0, strEnd + 1);
}

string FileStack::File::trim(const string& str) {
    return trimLeft(trimRight(str));
}

void FileStack::File::read(std::string filename) {
    canonicalPath = std::filesystem::canonical(filename);
    std::ifstream infile(canonicalPath);
    lines.clear();
    for( std::string line; getline( infile, line ); )
        lines.push_back(trimRight(line));
}

bool FileStack::File::eof() {
    return currentLine>=lines.size();
}
