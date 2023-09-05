#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "FileStack.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc<2) {
        cout << "give main file as parameter" << endl;
        return 0;
    }
    FileStack fis;
    fis.preprocess(argv[1]);
    return 0;
}
