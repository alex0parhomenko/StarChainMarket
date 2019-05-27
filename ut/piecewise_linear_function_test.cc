#include "linear_function.h"
#include "piecewise_linear_function.h"
#include "helpers.h"
#include <gtest/gtest.h>

TEST(piecewise_linear_function, base_test) {
    LinearFunctionDefineOnSegment func1(LinearFunction(1, -3, -2), 1, 4);
    LinearFunctionDefineOnSegment func2(2, 5, 4);
    LinearFunctionDefineOnSegment func3(LinearFunction(0, 1, 5), 4, 6);
    LinearFunctionDefineOnSegment func4(LinearFunction(3, -1, 13), 6, 7);
    LinearFunctionDefineOnSegment func5(LinearFunction(1, -2, -9), 7, 9);
    LinearFunctionDefineOnSegment func6(LinearFunction(2, -1, 9), 9, 10);

    PiecewiseLinearFunction piecewise_lin_func1({func1, func2, func3, func4, func5, func6}, Segment(1, 10));

    auto val = piecewise_lin_func1.GetValueAtPoint(3).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 1.6666666);

    auto val1 = piecewise_lin_func1.GetValueAtPoint(4);
    EXPECT_FLOAT_EQ(val1.GetStart(), 2);
    EXPECT_FLOAT_EQ(val1.GetEnd(), 5);

    val = piecewise_lin_func1.GetValueAtPoint(5.5).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 5);
    val = piecewise_lin_func1.GetValueAtPoint(6).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 5);
    val = piecewise_lin_func1.GetValueAtPoint(7).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 8);
    val = piecewise_lin_func1.GetValueAtPoint(8).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 8.5);
    val = piecewise_lin_func1.GetValueAtPoint(10).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 11);
    val = piecewise_lin_func1.GetValueAtPoint(1).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 1);

    LinearFunctionDefineOnSegment func7(LinearFunction(0, 1, 0), 0, 2);
    LinearFunctionDefineOnSegment func8(LinearFunction(3, -1, 6), 2, 3);
    LinearFunctionDefineOnSegment func9(LinearFunction(0, 1, 3), 3, 5);
    LinearFunctionDefineOnSegment func99(3, 5, 5);
    LinearFunctionDefineOnSegment func10(LinearFunction(1, -2, -5), 5, 7);
    LinearFunctionDefineOnSegment func11(LinearFunction(3, -1, 15), 7, 8);
    LinearFunctionDefineOnSegment func12(LinearFunction(0, 1, 9), 8, 11);

    PiecewiseLinearFunction piecewise_lin_func2({func7, func8, func9, func99, func10, func11, func12},
            Segment(0, 11));

    val = piecewise_lin_func2.GetValueAtPoint(0).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 0);
    val = piecewise_lin_func2.GetValueAtPoint(1).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 0);
    val = piecewise_lin_func2.GetValueAtPoint(2).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 0);
    val = piecewise_lin_func2.GetValueAtPoint(2.5).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 1.5);
    val = piecewise_lin_func2.GetValueAtPoint(3).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 3);
    val = piecewise_lin_func2.GetValueAtPoint(4).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 3);
    val1 = piecewise_lin_func2.GetValueAtPoint(5);
    EXPECT_FLOAT_EQ(val1.GetStart(), 3);
    EXPECT_FLOAT_EQ(val1.GetEnd(), 5);

    val = piecewise_lin_func2.GetValueAtPoint(6).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 5.5);
    val = piecewise_lin_func2.GetValueAtPoint(7).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 6);
    val = piecewise_lin_func2.GetValueAtPoint(7.5).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 7.5);
    val = piecewise_lin_func2.GetValueAtPoint(8).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 9);
    val = piecewise_lin_func2.GetValueAtPoint(9).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 9);
    val = piecewise_lin_func2.GetValueAtPoint(11).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 9);
    EXPECT_THROW(piecewise_lin_func2.GetValueAtPoint(113), std::runtime_error);

    auto sum_func = piecewise_lin_func1 + piecewise_lin_func2;
    EXPECT_DOUBLE_EQ(sum_func.GetFunctionDomain().GetStart(), 1);
    EXPECT_DOUBLE_EQ(sum_func.GetFunctionDomain().GetEnd(), 10);

    EXPECT_THROW(sum_func.GetValueAtPoint(-1), std::runtime_error);
    EXPECT_THROW(sum_func.GetValueAtPoint(0), std::runtime_error);

    val = sum_func.GetValueAtPoint(1).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 1);

    val = sum_func.GetValueAtPoint(2).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 1.333333);
    val = sum_func.GetValueAtPoint(3).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 4.6666665);
    val1 = sum_func.GetValueAtPoint(4.);
    EXPECT_FLOAT_EQ(val1.GetStart(), 5);
    EXPECT_FLOAT_EQ(val1.GetEnd(), 8);

    val1 = sum_func.GetValueAtPoint(5);
    EXPECT_FLOAT_EQ(val1.GetStart(), 8);
    EXPECT_FLOAT_EQ(val1.GetEnd(), 10);

    val = sum_func.GetValueAtPoint(6).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 10.5);
    val = sum_func.GetValueAtPoint(7).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 14);
    val = sum_func.GetValueAtPoint(10).GetSinglePoint();
    EXPECT_FLOAT_EQ(val, 20);
}

TEST(piecewise_linear_function, gen_non_increasing) {
    for (auto it = 0; it < 30; it++) {
        auto function_doman = GenerateFunctionDomain();
        //auto min_x = GenerateRandomValue(-1000, 0), max_x = GenerateRandomValue(0, 1000);
        auto f = PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(function_doman);

        for (auto it2 = 0; it2 < 10000; it2++) {
            auto x1 = GenerateRandomValue(function_doman.GetStart(), function_doman.GetEnd()),
                    x2 = GenerateRandomValue(function_doman.GetStart(), function_doman.GetEnd());
            if (x1 > x2) {
                std::swap(x1, x2);
            }
            auto val1 = f.GetValueAtPoint(x1).GetStart();
            auto val2 = f.GetValueAtPoint(x2).GetEnd();

            EXPECT_TRUE(val2 <= val1);
        }
    }
}

TEST(piecewise_linear_function, gen_non_decreasing) {
    for (auto it = 0; it < 30; it++) {
        auto min_x = GenerateRandomValue(-1000, 0), max_x = GenerateRandomValue(0, 1000);

        auto f = PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(Segment(min_x, max_x));

        for (auto it2 = 0; it2 < 10000; it2++) {
            auto x1 = GenerateRandomValue(min_x, max_x), x2 = GenerateRandomValue(min_x, max_x);
            if (x1 > x2) {
                std::swap(x1, x2);
            }
            auto val1 = f.GetValueAtPoint(x1).GetEnd();
            auto val2 = f.GetValueAtPoint(x2).GetStart();
            EXPECT_TRUE(val2 >= val1);
        }
    }
}

TEST(piecewise_linear_function, add_subtract) {

    for (auto it = 0; it < 100; it++) {
        auto function_domain1 = GenerateFunctionDomain();
        auto function_domain2 = GenerateFunctionDomain();
        auto mini = std::max(function_domain1.GetStart(), function_domain2.GetStart());
        auto maxi = std::min(function_domain1.GetEnd(), function_domain2.GetEnd());

        while (mini >= maxi) {
            function_domain1 = GenerateFunctionDomain();
            function_domain2 = GenerateFunctionDomain();
            mini = std::max(function_domain1.GetStart(), function_domain2.GetStart());
            maxi = std::min(function_domain1.GetEnd(), function_domain2.GetEnd());
        }

        auto f1 = PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(function_domain1);
        auto f2 = PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(function_domain2);


        if (GenerateRandomValue(0, 100) % 2 == 0) {
            f1 = PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(function_domain1);
        } else {
            f1 = PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(function_domain2);
        }

        if (GenerateRandomValue(0, 100) % 2 == 0) {
            f2 = PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(function_domain1);
        } else {
            f2 = PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(function_domain2);
        }

        PiecewiseLinearFunction res;
        auto rand = GenerateRandomValue(0, 100) % 2;
        if (rand == 0) {
            res = f1 - f2;
        } else {
            res = f1 + f2;
        }

        for (auto it2 = 0; it2 < 1000; it2++) {
            auto x = GenerateRandomValue(mini, maxi);
            auto res_val = res.GetValueAtPoint(x);
            if (rand == 0) {
                auto func_val = f1.GetValueAtPoint(x) - f2.GetValueAtPoint(x);
                EXPECT_TRUE(fabs(func_val.GetStart() - res_val.GetStart()) < kEPS);
                EXPECT_TRUE(fabs(func_val.GetEnd() - res_val.GetEnd()) < kEPS);
            } else {
                auto func_val = f1.GetValueAtPoint(x) + f2.GetValueAtPoint(x);
                EXPECT_TRUE(fabs(func_val.GetStart() - res_val.GetStart()) < kEPS);
                EXPECT_TRUE(fabs(func_val.GetEnd() - res_val.GetEnd()) < kEPS);
            }
        }
    }
}


TEST(piecewise_linear_function, add_with_vertical_segments) {
    Segment function_domain(-3, 3);
    std::vector<LinearFunctionDefineOnSegment> functions;
    functions.emplace_back(-kINF, 0, -3);
    functions.emplace_back(LinearFunction(0, 1, 0), -3, 0);
    functions.emplace_back(0, 3, 0);
    functions.emplace_back(LinearFunction(0, 1, 3), 0, 3);
    functions.emplace_back(3, kINF, 3);
    PiecewiseLinearFunction f1(functions, function_domain);


    Segment other_function_domain(-kINF, +kINF);
    std::vector<LinearFunctionDefineOnSegment> other_functions;
    other_functions.emplace_back(LinearFunction(2, -3, 6), -kINF, 0);
    other_functions.emplace_back(LinearFunction(0, 1, -2), 0, 4);
    other_functions.emplace_back(LinearFunction(1, -1, 6), 4, kINF);
    PiecewiseLinearFunction f2(other_functions, other_function_domain);

    auto res = f1 + f2;
    auto res_functions = res.GetFunctions();
    auto res_domain = res.GetFunctionDomain();
    EXPECT_EQ(res_domain.GetStart(), -3);
    EXPECT_EQ(res_domain.GetEnd(), 3);

    EXPECT_EQ(res_functions.size(), 5);
    EXPECT_EQ(res_functions[0], LinearFunctionDefineOnSegment(-kINF - 4, -4, -3));
    EXPECT_EQ(res_functions[1], LinearFunctionDefineOnSegment(LinearFunction(2, -3, 6), -3, 0));
    EXPECT_EQ(res_functions[2], LinearFunctionDefineOnSegment(-2, 1, 0));
    EXPECT_EQ(res_functions[3], LinearFunctionDefineOnSegment(LinearFunction(0, 1, 1), 0, 3));
    EXPECT_EQ(res_functions[4], LinearFunctionDefineOnSegment(1, kINF - 2, 3));


    res = f2 + f1;
    res_functions = res.GetFunctions();
    res_domain = res.GetFunctionDomain();
    EXPECT_EQ(res_domain.GetStart(), -3);
    EXPECT_EQ(res_domain.GetEnd(), 3);

    EXPECT_EQ(res_functions.size(), 5);
    EXPECT_EQ(res_functions[0], LinearFunctionDefineOnSegment(-kINF - 4, -4, -3));
    EXPECT_EQ(res_functions[1], LinearFunctionDefineOnSegment(LinearFunction(2, -3, 6), -3, 0));
    EXPECT_EQ(res_functions[2], LinearFunctionDefineOnSegment(-2, 1, 0));
    EXPECT_EQ(res_functions[3], LinearFunctionDefineOnSegment(LinearFunction(0, 1, 1), 0, 3));
    EXPECT_EQ(res_functions[4], LinearFunctionDefineOnSegment(1, kINF - 2, 3));
}

TEST(piecewise_linear_function, add_with_vertical_segments2) {
    Segment function_domain(-3, 3);
    std::vector<LinearFunctionDefineOnSegment> functions;
    functions.emplace_back(-kINF, 0, -3);
    functions.emplace_back(LinearFunction(0, 1, 0), -3, 0);
    functions.emplace_back(0, 3, 0);
    functions.emplace_back(LinearFunction(0, 1, 3), 0, 3);
    functions.emplace_back(3, kINF, 3);
    PiecewiseLinearFunction f1(functions, function_domain);


    Segment other_function_domain(-kINF, +kINF);
    std::vector<LinearFunctionDefineOnSegment> other_functions;
    other_functions.emplace_back(LinearFunction(2, -3, 6), -kINF, 0);
    other_functions.emplace_back(LinearFunction(0, 1, -2), 0, 3);
    other_functions.emplace_back(-2, 0, 3);
    other_functions.emplace_back(LinearFunction(1, -1, 3), 3, kINF);
    PiecewiseLinearFunction f2(other_functions, other_function_domain);

    auto res = f1 + f2;
    auto res_functions = res.GetFunctions();
    auto res_domain = res.GetFunctionDomain();
    EXPECT_EQ(res_domain.GetStart(), -3);
    EXPECT_EQ(res_domain.GetEnd(), 3);

    EXPECT_EQ(res_functions.size(), 5);
    EXPECT_EQ(res_functions[0], LinearFunctionDefineOnSegment(-kINF - 4, -4, -3));
    EXPECT_EQ(res_functions[1], LinearFunctionDefineOnSegment(LinearFunction(2, -3, 6), -3, 0));
    EXPECT_EQ(res_functions[2], LinearFunctionDefineOnSegment(-2, 1, 0));
    EXPECT_EQ(res_functions[3], LinearFunctionDefineOnSegment(LinearFunction(0, 1, 1), 0, 3));
    EXPECT_EQ(res_functions[4], LinearFunctionDefineOnSegment(1, kINF, 3));
}

TEST(piecewise_linear_function, mirror_x_y) {
    for (auto it = 0; it < 50; it++) {
        auto function_domain = GenerateFunctionDomain();
        auto f =
                GenerateRandomValue(0, 100) % 2 ? PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(
                        function_domain) : PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(
                        function_domain);
        auto f_inv = f.MirrorXAndY();
        EXPECT_DOUBLE_EQ(f_inv.GetFunctionDomain().GetStart(), -function_domain.GetEnd());
        EXPECT_DOUBLE_EQ(f_inv.GetFunctionDomain().GetEnd(), -function_domain.GetStart());
        for (auto it2 = 0; it2 < 10000; it2++) {
            auto x = GenerateRandomValue(function_domain.GetStart(), function_domain.GetEnd());
            auto val = f.GetValueAtPoint(x);
            auto val2 = f_inv.GetValueAtPoint(-x);

            EXPECT_TRUE(fabs(val.GetStart() + val2.GetEnd()) < kEPS);
            EXPECT_TRUE(fabs(val.GetEnd() + val2.GetStart()) < kEPS);
        }
    }
}


TEST(piecewise_linear_function, inverse_function1) {
    std::vector<LinearFunctionDefineOnSegment> functions;
    functions.emplace_back(-kINF, -3, -3);
    functions.emplace_back(LinearFunction(0, 1, -3), -3, 0);
    functions.emplace_back(-3, 2, 0);
    functions.emplace_back(LinearFunction(0, 1, 2), 0, 3);
    functions.emplace_back(2, kINF, 3);

    auto f = PiecewiseLinearFunction(functions, Segment(-3, 3));
    auto f_inv = f.GetInverseFunction();
    auto func = f_inv.GetFunctions();
    EXPECT_EQ(func[0], LinearFunctionDefineOnSegment(LinearFunction(0, 1, -3), -kINF, -3));
    EXPECT_EQ(func[1], LinearFunctionDefineOnSegment(-3, 0, -3));
    EXPECT_EQ(func[2], LinearFunctionDefineOnSegment(LinearFunction(0, 1, 0), -3, 2));
    EXPECT_EQ(func[3], LinearFunctionDefineOnSegment(0, 3, 2));
    EXPECT_EQ(func[4], LinearFunctionDefineOnSegment(LinearFunction(0, 1, 3), 2, kINF));
}

TEST(piecewise_linear_function, inverse_function) {
    for (auto it = 0; it < 50; it++) {
        auto function_domain = GenerateFunctionDomain();
        auto f =
                GenerateRandomValue(0, 100) % 2 ? PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(
                        function_domain) : PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(
                        function_domain);
        auto f_inv = f.GetInverseFunction();

        auto x = GenerateRandomValue(function_domain.GetStart(), function_domain.GetEnd());
        auto val = f.GetValueAtPoint(x);

        if (val.GetStart() == val.GetEnd()) {
            auto val1 = f_inv.GetValueAtPoint(val.GetSinglePoint());
            if (val1.GetStart() == val1.GetEnd()) {
                EXPECT_TRUE(fabs(val1.GetSinglePoint() - x) < kEPS);
            } else {
                EXPECT_TRUE(val1.GetStart() <= x);
                EXPECT_TRUE(val1.GetEnd() >= x);
            }
        } else {
            auto x_segment_val = GenerateRandomValue(val.GetStart(), val.GetEnd());
            auto val1 = f_inv.GetValueAtPoint(x_segment_val).GetSinglePoint();
            EXPECT_TRUE(fabs(val1 - x) < kEPS);
        }
    }
}

TEST(piecewise_linear_function, find_function_zero_value) {
    for (auto it = 0; it < 10009; it++) {
        auto function_domain = GenerateFunctionDomain();
        auto f =
                GenerateRandomValue(0, 100) % 2 ? PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(
                        function_domain) : PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(
                        function_domain);
        try {
            auto zero_x = f.FindFunctionZeroValue();
            if (zero_x.GetStart() == zero_x.GetEnd()) {
                auto start = f.GetValueAtPoint(zero_x.GetSinglePoint()).GetStart();
                auto end = f.GetValueAtPoint(zero_x.GetSinglePoint()).GetEnd();
                if (start == end) {
                    if (fabs(start) > kEPS) {
                        std::cerr << start << std::endl;
                    }
                    EXPECT_TRUE(fabs(start) < kEPS);
                } else {
                    EXPECT_TRUE(start <= kEPS);
                    EXPECT_TRUE(end >= -kEPS);
                }
            } else {
                for (auto it2 = 0; it < 100; it++) {
                    auto x = GenerateRandomValue(zero_x.GetStart(), zero_x.GetEnd());
                    auto val = f.GetValueAtPoint(x).GetSinglePoint();
                    if (fabs(val) > kEPS) {
                        std::cerr << val << std::endl;
                    }
                    EXPECT_TRUE(fabs(val) < kEPS);
                }
            }
        } catch (const std::runtime_error& e) {
            std::string ignore_message = "Function don't intersect Y axis";
            if (std::string(e.what()) != ignore_message) {
                std::cerr << e.what() << std::endl;
            }
        }
    }
}

TEST(piecewise_linear_function, create_sp_dp) {
    for (auto it = 0; it < 100; it++) {
        long double d = GenerateRandomValue<int>(1, 100);
        long double c = GenerateRandomValue<int>(1, 100);
        auto dp = PiecewiseLinearFunction::CreateDpFunction(c, d);
        auto sp = PiecewiseLinearFunction::CreateSpFunction(c, d);
        auto delta_s = sp - dp;
        auto zero_point = delta_s.FindFunctionZeroValue().GetSinglePoint();
        EXPECT_TRUE(fabs(zero_point - d / c) < kEPS);
    }
}


TEST(piecewise_linear_function, multiply_horizontal_segments) {
    std::vector<LinearFunctionDefineOnSegment> funcs;
    funcs.emplace_back(Point(0, 2), Point(1, 2));
    funcs.emplace_back(Point(1, 2), Point(4, 2));
    PiecewiseLinearFunction f(funcs, Segment(0, 4));
    EXPECT_EQ(f.GetFunctions().size(), 1);
}
