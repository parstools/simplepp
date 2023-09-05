#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "FileStack.h"

using namespace std;

int main() {
    FileStack fis;
    fis.preprocess("../testsuite/a.txt");
    return 0;
}
