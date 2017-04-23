#pragma once
#include "CppUnitTest.h"
#define BEGIN_TEST_GROUP(groupName) TEST_CLASS(groupName) { public:
#define END_TEST_GROUP };
#define RUN_TESTS
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#define ASSERT_TRUE(v) Assert::IsTrue(v,nullptr,LINE_INFO())
#define ASSERT_FALSE(v) Assert::IsFalse(v,nullptr,LINE_INFO())
#define ASSERT_EQ(a,b) Assert::AreEqual(a , b , nullptr , LINE_INFO())
#define ASSERT_NE(a,b) Assert::AreNotEqual(a , b , nullptr , LINE_INFO())
#define FAIL() Assert::Fail(nullptr,LINE_INFO())


