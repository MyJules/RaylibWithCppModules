#include <gtest/gtest.h>

import std;
import args;

TEST(ArgsTest, ToSpanView) {
    const char* argv[3] = {"program", "arg1", "arg2"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    auto args = args::to_span(argc, argv);

    EXPECT_EQ(args.size(), 3);
    ASSERT_TRUE(args[0] == std::string_view("program"));
    ASSERT_TRUE(args[1] == std::string_view("arg1"));
    ASSERT_TRUE(args[2] == std::string_view("arg2"));
}

TEST(ArgsTest, EmptyArgs) {
    const char* argv[1] = {"program"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    auto args = args::to_span(argc, argv);

    EXPECT_EQ(args.size(), 1);
    ASSERT_TRUE(args[0] == std::string_view("program"));
}