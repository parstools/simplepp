#include "gtest/gtest.h"
#include "../FileStack.h"

using namespace std;

TEST(Directive, nextWord) {
    string line = "#abc12 1234 \"s 3 d\" <= k ";
    Directive directive;
    EXPECT_EQ(6, directive.nextWord(line,1));
    EXPECT_EQ(11, directive.nextWord(line,7));
    EXPECT_EQ(19, directive.nextWord(line,12));
    EXPECT_EQ(22, directive.nextWord(line,20));
    EXPECT_EQ(24, directive.nextWord(line,23));
}
