#pragma once
#include <gtest/gtest.h>
#define BEGIN_TEST_GROUP
#define END_TEST_GROUP
#define TEST_METHOD(testName) TEST(ETH_TEST_SUITE, testName)
#define RUN_TESTS \
int main(int argc, char**argv) {\
    testing::InitGoogleTest(&argc, argv);\
    return RUN_ALL_TESTS();\
}
