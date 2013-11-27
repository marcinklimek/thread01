#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "async_map.h"



TEST(async_map_test, int_int_map_four_elements)
{
    AsyncMap<int, int> map;
    std::vector<int> input{};
    std::vector<int> output{};
    for (int i = 1  ; i < 5 ; ++i)
        input.push_back(i);

    output = map(input, [](int a) { return a+1; });
    ASSERT_THAT(output, ::testing::ElementsAre(2,3,4,5));
}

TEST(async_map_test, int_int_map_two_elements)
{
    AsyncMap<int, int> map;
    std::vector<int> input{};
    std::vector<int> output{};
    for (int i = 1  ; i < 3 ; ++i)
        input.push_back(i);
    output = map(input, [](int a) { return a+1; });
    ASSERT_THAT(output, ::testing::ElementsAre(2,3));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
