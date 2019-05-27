#pragma once
#include "linear_function.h"
#include "linear_function_define_on_segment.h"
#include "segment.h"
#include <vector>
#include <iostream>
#include <optional>

/*
 * Implements piecewise linear function, defined on sequence of segments [ai, bi],
 * by default function must is continuous.
 */
class PiecewiseLinearFunction {
public:
    /*
     * Zero function only
     */
    PiecewiseLinearFunction() noexcept {
        functions_.push_back(LinearFunctionDefineOnSegment(LinearFunction(0, 1, 0), -kINF, kINF));
    }

    PiecewiseLinearFunction(std::vector<LinearFunctionDefineOnSegment> functions, Segment function_domain);

    PiecewiseLinearFunction GetInverseFunction() const;

    PiecewiseLinearFunction& operator=(const PiecewiseLinearFunction&) = default;
    PiecewiseLinearFunction operator+(PiecewiseLinearFunction other);
    PiecewiseLinearFunction operator-(PiecewiseLinearFunction other);

    Segment GetValueAtPoint(long double x) const;

    std::optional<Segment> GetHorizontalSegmentWithPoint(long double x) const;

    Segment GetFunctionDomain() const {
        return function_domain_;
    }

    void ExtendFunctionDomain(long double x) {
        if (functions_.back().IsVertical()) {
            throw std::runtime_error("Can't extend x end coordinate of vertical function");
        } else if (function_domain_.GetEnd() < x) {
            function_domain_.SetEnd(x);
            functions_.back().SetXEndCoordinate(x);
        }
    }

    void Shift(long double x);

    /*
     * TODO: add test
     */
    long double Integrate(long double from, long double to) {
        long double result = 0;
        for (auto&& f : functions_) {
            if (f.IsVertical()) {
                continue;
            }
            auto x_start = f.GetXStartCoordinate();
            auto x_end = f.GetXEndCoordinate();
            if (IsSegmentsIntersects(x_start, x_end, from, to) && x_start < to && x_end > from) {
                auto start = std::max(x_start, from);
                auto end = std::min(x_end, to);
                result += f.Integrate(start, end);
            }
        }
        return result;
    }

    std::vector<LinearFunctionDefineOnSegment> GetFunctions() const {
        return functions_;
    }

    PiecewiseLinearFunction MirrorXAndY() const noexcept;

    Segment FindFunctionZeroValue() const;

    void Print() const {
        for (auto&& func : functions_) {
            std::cout << "("  << func.GetXStartCoordinate() << ", " << func.GetValueAtStartPoint().GetStart() << ") -- "
            <<  "("  << func.GetXEndCoordinate() << ", " << func.GetValueAtEndPoint().GetEnd() << ")" << std::endl;
        }
        std::cout << std::endl << std::endl;
    }


    static PiecewiseLinearFunction CreateSpFunction(long double c, long double d);
    static PiecewiseLinearFunction CreateDpFunction(long double c, long double d);

    static PiecewiseLinearFunction GenerateSpFunction();
    static PiecewiseLinearFunction GenerateDpFunction();
    static PiecewiseLinearFunction GenerateNonIncreasingLinearFunction(Segment function_domain);
    static PiecewiseLinearFunction GenerateNonDecreasingLinearFunction(Segment function_domain);

private:
    static constexpr long double kDpXEndCoodinate = 1000;
    //static constexpr long double kMinDpValueAtZero = 10;
    //static constexpr long double kMaxDpValueAtZero = 200;
    static constexpr int64_t kMinCoordsCount = 1;
    static constexpr int64_t kMaxCoordsCount = 5;

    bool IsSegmentsIntersects(long double x1_start, long double x1_end, long double x2_start, long double x2_end) {
        if (x1_start >= x2_start && x1_start <= x2_end) {
            return true;
        } else if (x1_end >= x2_start && x1_end <= x2_end) {
            return true;
        } else if (x2_start >= x1_start && x2_start <= x1_end) {
            return true;
        } else if (x2_end >= x1_start && x2_end <= x1_end) {
            return true;
        }
        return false;
    }

    template<typename Operator>
    PiecewiseLinearFunction AddOrSubtract(PiecewiseLinearFunction other, Operator op);

    template<typename FunctionGenerator, typename ShiftComparator>
    static PiecewiseLinearFunction GenerateLinearFunction(FunctionGenerator linear_function_generator,
            ShiftComparator shift_comparator, Segment function_domain);
    void ValidateFunctions();
    std::vector<LinearFunctionDefineOnSegment> functions_;
    //Segment where function define
    Segment function_domain_;
};
