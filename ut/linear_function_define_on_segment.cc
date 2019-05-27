#include "linear_function.h"
#include "linear_function_define_on_segment.h"
#include "helpers.h"
#include <gtest/gtest.h>


TEST(linear_func_def_on_segment, integrate) {
    LinearFunction f1(5, 3, 2);
    LinearFunctionDefineOnSegment f2(f1, 2, 9);
    auto val = f2.Integrate();
    EXPECT_FLOAT_EQ(val, -59.5);
    LinearFunctionDefineOnSegment f3(f1, -1, 11);
    val = f3.Integrate();
    EXPECT_FLOAT_EQ(val, -92);
}

TEST(linear_func_def_on_segment, inverse) {
    for (auto it = 0; it < 100; it++) {
        auto function_domain = GenerateFunctionDomain();
        auto func = LinearFunctionDefineOnSegment::GenerateNonIncreasingLinearFunction(
                function_domain.GetStart(), function_domain.GetEnd());
        auto func_inv = func.GetInverse();
        for (auto it = 0; it < 5000; it++) {
            auto x = GenerateRandomValue(function_domain.GetStart(), function_domain.GetEnd());
            auto val = func.GetValueAtPoint(x).GetSinglePoint();
            auto val1 = func_inv.GetValueAtPoint(val);
            if (func.GetLinearFunction().GetACoeff() == 0) {
                EXPECT_EQ(val1, Segment(func.GetXStartCoordinate(), func.GetXEndCoordinate()));
            } else {
                auto vv = val1.GetSinglePoint();
                EXPECT_TRUE(fabs(vv - x) < kEPS);
            }
        }
    }
}


