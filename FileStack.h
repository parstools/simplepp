//
// Created by andrzej on 9/4/23.
//

#ifndef PP_FILESTACK_H
#define PP_FILESTACK_H

#include <unordered_map>
#include <string>
#include <filesystem>
#include <vector>

enum class DirectiveType {dtNone, dtInclude, dtDefine, dtIfdef, dtIfndef, dtEndif,
    dtElse, dtElif, dtIf};

enum class RelOp {opEq, opNe, opGt, opGe, opLt, opLe};

struct Directive {
    DirectiveType dt = DirectiveType::dtNone;
    std::string include;
    RelOp relOp = RelOp::opEq;
    std::string variable;
    std::string variable2;
};

class File {
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::string trim(const std::string& str);
public:
    Directive directive();
    std::string currLine();
    std::filesystem::path canonicalPath;
    std::vector<std::string> lines;
    int currentLine = 0;
    void read(std::string filename);
    bool eof();
};

class FileStack {
    std::unordered_map<std::string,std::string> variables;
    std::vector<File> stack;
public:
    void preprocess(std::string filename);
};


#endif //PP_FILESTACK_H
