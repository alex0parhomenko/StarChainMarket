#include "linear_function.h"
#include <gtest/gtest.h>

TEST(linear_function, shift) {
    for (auto it = 0; it < 100; it++) {
        auto func = LinearFunction::GenerateNonDecreasingLinearFunction();
        auto shift = GenerateRandomValue(-1000, 1000);
        auto x = GenerateRandomValue(-10000, 10000);
        auto x_new = x + shift;
        auto y_new = func.GetValueAtPoint(x_new);
        func.ShiftXAxis(shift);
        EXPECT_TRUE(fabs(func.GetValueAtPoint(x) - y_new) < 0.000001);
    }

    for (auto it = 0; it < 100; it++) {
        auto func = LinearFunction::GenerateNonIncreasingLinearFunction();
        auto shift = GenerateRandomValue(-1000, 1000);
        auto x = GenerateRandomValue(-10000, 10000);
        auto x_new = x + shift;
        auto y_new = func.GetValueAtPoint(x_new);
        func.ShiftXAxis(shift);
        EXPECT_TRUE(fabs(func.GetValueAtPoint(x) - y_new) < 0.000001);
    }
}

