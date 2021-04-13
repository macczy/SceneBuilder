#include "pch.h"
#include <iostream>

#include "../SceneBuilder/Scanner.cpp"

TEST(ScannerUnitTest, DefaultConstructorTest) {
	Scanner scanner(std::cin);
  EXPECT_TRUE(true);
}

