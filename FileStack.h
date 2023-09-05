//
// Created by andrzej on 9/4/23.
//

#ifndef PP_FILESTACK_H
#define PP_FILESTACK_H

#include <unordered_map>
#include <string>
#include <filesystem>
#include <vector>

class FileStack {
    std::unordered_map<std::string,std::string> variables;
    struct File {
        static bool isDirective(std::string line);
        std::filesystem::path canonicalPath;
        std::vector<std::string> lines;
        int currentLine = 0;
        void read(std::string filename);
        static std::string trimLeft(const std::string& str);
        static std::string trimRight(const std::string& str);
        static std::string trim(const std::string& str);
        bool eof();
    };
    std::vector<File> stack;
public:
    void preprocess(std::string filename);
};


#endif //PP_FILESTACK_H
