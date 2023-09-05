//
// Created by andrzej on 9/4/23.
//

#ifndef PP_FILESTACK_H
#define PP_FILESTACK_H

#include <unordered_map>
#include <string>
#include <filesystem>
#include <vector>

std::string trimLeft(const std::string& str);
std::string trimRight(const std::string& str);
std::string trim(const std::string& str);

enum class DirectiveType {
    dtNone, dtInclude, dtDefine, dtUndef, dtIfdef, dtIfndef, dtEndif,
    dtElse, dtElif, dtIf};

enum class RelOp {
    opEq, opNe, opGt, opGe, opLt, opLe
};
enum class WordType {
    wtIdent, wtNumber, wtQuote, wtOther
};

struct Directive {
    DirectiveType dt = DirectiveType::dtNone;
    std::string include;
    RelOp relOp = RelOp::opEq;
    std::string variable;
    std::string variable2;
    WordType varType;
    WordType var2Type;
    static size_t skipBlank(std::string line, size_t start);
    static size_t nextIdent(std::string line, size_t start);
    static size_t nextNumber(std::string line, size_t start);
    static size_t nextQuote(std::string line, size_t start);
    static size_t nextOther(std::string line, size_t start);
    static size_t nextWord(std::string line, size_t start);
    static std::vector<std::pair<std::string, WordType>> parseRaw(std::string line);
    static std::string unquote(std::string quoted);
    void parse(std::string line);
};

struct File {
    Directive directive();
    std::string currLine();
    std::filesystem::path canonicalPath;
    std::vector<std::string> lines;
    int currentLine = 0;
    void read(std::string filename);
};

class FileStack {
    std::unordered_map<std::string,std::string> variables;
    std::vector<File> stack;
    void process(int h);
public:
    void preprocess(std::string filename);
};


#endif //PP_FILESTACK_H
