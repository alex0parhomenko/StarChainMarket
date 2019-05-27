#include "utils.h"
#include <gtest/gtest.h>

TEST(gen_random_value, simple_gen_int) {
    auto min = -13991, max = 29328;
    for (auto i = 0; i < 1000; i++) {
        auto val = GenerateRandomValue(min, max);
        EXPECT_LE(val, max);
        EXPECT_GE(val, min);
    }
    min = 138, max = 1399191;
    for (auto i = 0; i < 1000; i++) {
        auto val = GenerateRandomValue(min, max);
        EXPECT_LE(val, max);
        EXPECT_GE(val, min);
    }
}

TEST(gen_random_value, simple_gen_double) {
    auto min = -2138.399, max = 11.223;
    for (auto i = 0; i < 1000; i++) {
        auto val = GenerateRandomValue(min, max);
        EXPECT_LE(val, max);
        EXPECT_GE(val, min);
    }
    min = 0.138, max = 481.3;
    for (auto i = 0; i < 1000; i++) {
        auto val = GenerateRandomValue(min, max);
        EXPECT_LE(val, max);
        EXPECT_GE(val, min);
    }
}

TEST(gen_random_vector, gen_int_vec) {
    auto min = 12, max = 3919, vec_size = 398;
    auto res = GenerateVectorOfValues(min, max, vec_size);
    EXPECT_EQ(res.size(), vec_size);
    for (size_t i = 0; i < res.size(); i++) {
        if (i > 0) {
            EXPECT_LT(res[i - 1], res[i]);
        }
        auto elem = res[i];
        EXPECT_LE(elem, max);
        EXPECT_GE(elem, min);
    }
}

TEST(gen_random_vector, gen_double_vec) {
    double min = -183.4, max = 239892.92;
    size_t vec_size = 8384;
    auto res = GenerateVectorOfValues(min, max, vec_size);
    EXPECT_EQ(res.size(), vec_size);
    for (size_t i = 0; i < res.size(); i++) {
        if (i > 0) {
            EXPECT_LT(res[i - 1], res[i]);
        }
        auto elem = res[i];
        EXPECT_LE(elem, max);
        EXPECT_GE(elem, min);
    }
}

