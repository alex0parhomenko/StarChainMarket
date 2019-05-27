#include "piecewise_linear_function.h"
#include <algorithm>
#include <vector>
#include <cassert>

PiecewiseLinearFunction::PiecewiseLinearFunction(std::vector<LinearFunctionDefineOnSegment> functions, Segment function_domain)
        : functions_(std::move(functions)), function_domain_(function_domain) {
    std::sort(functions_.begin(), functions_.end(), [&](auto&& lhs, auto&& rhs) {
        return lhs.GetXStartCoordinate() < rhs.GetXStartCoordinate() ||
            (lhs.GetXStartCoordinate() == rhs.GetXStartCoordinate() &&
                    lhs.IsVertical());
    });
    ValidateFunctions();
}

void PiecewiseLinearFunction::Shift(long double x) {
    for (auto& func : functions_) {
        func.Shift(x);
    }
}

PiecewiseLinearFunction PiecewiseLinearFunction::GetInverseFunction() const {
    std::vector<LinearFunctionDefineOnSegment> result_functions;
    long double function_domain_start = 1e9;
    long double function_domain_end = -1e9;

    result_functions.reserve(functions_.size());
    for (auto&& function : functions_) {
        auto inv_func = function.GetInverse();

        if (fabs(inv_func.GetXStartCoordinate()) < kEPS) {
            inv_func.SetXStartCoordinate(0);
        }

        if (inv_func.GetXStartCoordinate() < function_domain_start) {
            function_domain_start = inv_func.GetXStartCoordinate();
        }
        if (inv_func.GetXEndCoordinate() > function_domain_end) {
            function_domain_end = inv_func.GetXEndCoordinate();
        }
        result_functions.push_back(inv_func);
    }
    return PiecewiseLinearFunction(result_functions, Segment(function_domain_start, function_domain_end));
}


PiecewiseLinearFunction PiecewiseLinearFunction::MirrorXAndY() const noexcept {
    Segment function_domain(-function_domain_.GetEnd(), -function_domain_.GetStart());
    decltype(functions_) functions;
    for (auto&& func : functions_) {
        functions.push_back(func.MirrorXAndY());
    }
    return PiecewiseLinearFunction(functions, function_domain);
}


PiecewiseLinearFunction PiecewiseLinearFunction::operator+(PiecewiseLinearFunction other) {
    auto f = [](auto&& left, auto&& right) {
        return left + right;
    };
    return AddOrSubtract(other, f);
}

PiecewiseLinearFunction PiecewiseLinearFunction::operator-(PiecewiseLinearFunction other) {
    auto f = [](auto&& left, auto&& right) {
        return left - right;
    };
    return AddOrSubtract(other, f);
}


Segment PiecewiseLinearFunction::FindFunctionZeroValue() const {
    for (auto&& f : functions_) {
        if (f.IsVertical()) {
            auto val = f.GetVerticalSegment();
            if (val.GetStart() <= 0 && val.GetEnd() >= 0) {
                return Segment(f.GetXStartCoordinate(), f.GetXStartCoordinate());
            }
        } else {
            const long double eps = 0.00000000001;
            if (f.GetLinearFunction().GetACoeff() == 0. && f.GetLinearFunction().GetCCoeff() == 0.) {
                return Segment(f.GetXStartCoordinate(), f.GetXEndCoordinate());
            }
            if (f.GetValueAtStartPoint().GetSinglePoint() == 0) {
                return Segment(f.GetXStartCoordinate(), f.GetXStartCoordinate());
            }
            if (f.GetValueAtEndPoint().GetSinglePoint() == 0.) {
                return Segment(f.GetXEndCoordinate(), f.GetXEndCoordinate());
            }
            if (f.GetValueAtStartPoint().GetSinglePoint() <= 0. && f.GetValueAtEndPoint().GetSinglePoint() >= 0) {
                auto l = f.GetXStartCoordinate(), r = f.GetXEndCoordinate();
                while (r - l > eps) {
                    auto mid = (r + l) / 2.;
                    if (f.GetValueAtPoint(mid).GetSinglePoint() <= 0.) {
                        l = mid;
                    } else {
                        r = mid;
                    }
                }
                return Segment(l, l);
            } else if (f.GetValueAtStartPoint().GetSinglePoint() >= 0. && f.GetValueAtEndPoint().GetSinglePoint() <= 0.) {
                auto l = f.GetXStartCoordinate(), r = f.GetXEndCoordinate();
                while (r - l > eps) {
                    auto mid = (r + l) / 2.;
                    if (f.GetValueAtPoint(mid).GetSinglePoint() <= 0.) {
                        r = mid;
                    } else {
                        l = mid;
                    }
                }
                return Segment(r, r);
            }
        }
    }
    throw std::runtime_error("Function don't intersect Y axis");
}

template<typename Operator>
PiecewiseLinearFunction PiecewiseLinearFunction::AddOrSubtract(PiecewiseLinearFunction other, Operator op) {
    std::vector<LinearFunctionDefineOnSegment> curr_functions = functions_;
    std::vector<LinearFunctionDefineOnSegment> other_functions = other.functions_;

    std::vector<LinearFunctionDefineOnSegment> curr_functions_in_domain_zone;
    std::vector<LinearFunctionDefineOnSegment> other_functions_in_domain_zone;

    std::vector<LinearFunctionDefineOnSegment> functions;
    size_t curr_pointer = 0, other_pointer = 0;

    Segment function_domain(std::max(function_domain_.GetStart(), other.function_domain_.GetStart()), std::min(function_domain_.GetEnd(), other.function_domain_.GetEnd()));
    if (function_domain.GetStart() > function_domain.GetEnd()) {
        throw std::runtime_error("Function domain don't intersects");
    }

    for (auto&& f : curr_functions) {
        if (f.GetXStartCoordinate() >= function_domain.GetStart() && f.GetXEndCoordinate() <= function_domain.GetEnd()) {
            curr_functions_in_domain_zone.push_back(f);
        } else if (IsSegmentsIntersects(f.GetXStartCoordinate(), f.GetXEndCoordinate(), function_domain.GetStart(), function_domain.GetEnd())) {
            if (f.IsVertical()) {
                curr_functions_in_domain_zone.push_back(f);
            } else if (f.GetXStartCoordinate() != function_domain.GetEnd() && f.GetXEndCoordinate() != function_domain.GetStart()) {
                f.SetXStartCoordinate(std::max(f.GetXStartCoordinate(), function_domain.GetStart()));
                f.SetXEndCoordinate(std::min(f.GetXEndCoordinate(), function_domain.GetEnd()));
                curr_functions_in_domain_zone.push_back(f);
            }
        }
    }

    for (auto&& f : other_functions) {
        if (f.GetXStartCoordinate() >= function_domain.GetStart() && f.GetXEndCoordinate() <= function_domain.GetEnd()) {
            other_functions_in_domain_zone.push_back(f);
        } else if (IsSegmentsIntersects(f.GetXStartCoordinate(), f.GetXEndCoordinate(), function_domain.GetStart(), function_domain.GetEnd())) {
            if (f.IsVertical()) {
                other_functions_in_domain_zone.push_back(f);
            } else if (f.GetXStartCoordinate() != function_domain.GetEnd() && f.GetXEndCoordinate() != function_domain.GetStart()) {
                f.SetXStartCoordinate(std::max(f.GetXStartCoordinate(), function_domain.GetStart()));
                f.SetXEndCoordinate(std::min(f.GetXEndCoordinate(), function_domain.GetEnd()));
                other_functions_in_domain_zone.push_back(f);
            }
        }
    }

    if (curr_functions_in_domain_zone.back().IsVertical() && !other_functions_in_domain_zone.back().IsVertical()) {
        auto end_point = other_functions_in_domain_zone.back().GetXEndCoordinate();
        auto value_at_end_point = other_functions_in_domain_zone.back().GetValueAtEndPoint().GetSinglePoint();
        other_functions_in_domain_zone.emplace_back(value_at_end_point, value_at_end_point, end_point);
    } else if (!curr_functions_in_domain_zone.back().IsVertical() && other_functions_in_domain_zone.back().IsVertical()) {
        auto end_point = curr_functions_in_domain_zone.back().GetXEndCoordinate();
        auto value_at_end_point = curr_functions_in_domain_zone.back().GetValueAtEndPoint().GetSinglePoint();
        curr_functions_in_domain_zone.emplace_back(value_at_end_point, value_at_end_point, end_point);
    }

    while (curr_pointer < curr_functions_in_domain_zone.size() && other_pointer < other_functions_in_domain_zone.size()) {
        if (curr_functions_in_domain_zone[curr_pointer].GetXStartCoordinate() == other_functions_in_domain_zone[other_pointer].GetXStartCoordinate()) {

            if (curr_functions_in_domain_zone[curr_pointer].GetXEndCoordinate() < other_functions_in_domain_zone[other_pointer].GetXEndCoordinate()) {
                LinearFunctionDefineOnSegment func_to_add = other_functions_in_domain_zone[other_pointer];
                func_to_add.SetXEndCoordinate(curr_functions_in_domain_zone[curr_pointer].GetXEndCoordinate());
                other_functions_in_domain_zone[other_pointer].SetXStartCoordinate( curr_functions_in_domain_zone[curr_pointer].GetXEndCoordinate());
                functions.emplace_back(op(curr_functions_in_domain_zone[curr_pointer++], func_to_add));
            } else if (curr_functions_in_domain_zone[curr_pointer].GetXEndCoordinate() > other_functions_in_domain_zone[other_pointer].GetXEndCoordinate()) {
                LinearFunctionDefineOnSegment func_to_add = curr_functions_in_domain_zone[curr_pointer];
                func_to_add.SetXEndCoordinate(other_functions_in_domain_zone[other_pointer].GetXEndCoordinate());
                curr_functions_in_domain_zone[curr_pointer].SetXStartCoordinate(other_functions_in_domain_zone[other_pointer].GetXEndCoordinate());
                functions.emplace_back(op(func_to_add, other_functions_in_domain_zone[other_pointer++]));
            } else {
                functions.emplace_back(op(curr_functions_in_domain_zone[curr_pointer++], other_functions_in_domain_zone[other_pointer++]));
            }
        }
        else {
            throw std::runtime_error("Functions starts in diffrent points");
        }
    }
    return PiecewiseLinearFunction(functions, function_domain);
}

PiecewiseLinearFunction PiecewiseLinearFunction::CreateSpFunction(long double c, long double d) {
    auto demand_zeroing_price = 2 * d / c;
    Segment function_domain(0, demand_zeroing_price + 10);
    std::vector<LinearFunctionDefineOnSegment> functions;
    functions.emplace_back(LinearFunction(-c, 2, 0), 0, demand_zeroing_price);
    functions.emplace_back(LinearFunction(-c, 1, -d), demand_zeroing_price, function_domain.GetEnd());
    auto result = PiecewiseLinearFunction(functions, function_domain);
    return result;
}

PiecewiseLinearFunction PiecewiseLinearFunction::CreateDpFunction(long double c, long double d) {
    long double demand_zeroing_price = 2 * d / c;
    Segment function_domain(0, demand_zeroing_price + 10);
    std::vector<LinearFunctionDefineOnSegment> functions;
    functions.emplace_back(LinearFunction(c, 2, 2 * d), function_domain.GetStart(), demand_zeroing_price);
    functions.emplace_back(LinearFunction(0, 1, 0), demand_zeroing_price, function_domain.GetEnd());
    auto result = PiecewiseLinearFunction(functions, function_domain);
    return result;
}

/*
 * Generate Non Increasing function Dp(+inf) = 0
 */
PiecewiseLinearFunction PiecewiseLinearFunction::GenerateDpFunction() {
    Segment function_domain(0, kINF);
    auto f = PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(Segment(0, kINF - 1));
    auto functions = f.GetFunctions();
    auto min_value = functions.back().GetValueAtEndPoint().GetSinglePoint();
    for (auto& func : functions) {
        func.Shift(-min_value);
    }

    functions.push_back(LinearFunctionDefineOnSegment(LinearFunction(0, 1, 0), kINF - 1, kINF));
    return PiecewiseLinearFunction(functions, function_domain);
}

/*
 * Generate Non Decreasing function Sp(0) = 0
 */
PiecewiseLinearFunction PiecewiseLinearFunction::GenerateSpFunction() {
    Segment function_domain(0,kINF);
    auto f = PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(function_domain);
    auto functions = f.GetFunctions();
    auto value_at_zero = f.GetValueAtPoint(0);
    for (auto& func : functions) {
        func.Shift(-value_at_zero.GetSinglePoint());
    }
    //functions.push_back(LinearFunctionDefineOnSegment(LinearFunction(-1, 1, 0), kINF - 1, kINF));
    return PiecewiseLinearFunction(functions, function_domain);
}

PiecewiseLinearFunction PiecewiseLinearFunction::GenerateNonIncreasingLinearFunction(Segment function_domain) {
    auto result = PiecewiseLinearFunction::GenerateLinearFunction(
            LinearFunctionDefineOnSegment::GenerateNonIncreasingLinearFunction,
            [](auto func_value_at_start_point, auto prev_func_value_at_end_point) {
                return func_value_at_start_point > prev_func_value_at_end_point;
            }, function_domain);
    return result;
}

PiecewiseLinearFunction PiecewiseLinearFunction::GenerateNonDecreasingLinearFunction(Segment function_domain) {
    auto result = PiecewiseLinearFunction::GenerateLinearFunction(
            LinearFunctionDefineOnSegment::GenerateNonDecreasingLinearFunction,
            [](auto func_value_at_start_point, auto prev_func_value_at_end_point) {
                return func_value_at_start_point < prev_func_value_at_end_point;
            }, function_domain);
    return result;
}

template<typename FunctionGenerator, typename ShiftComparator>
PiecewiseLinearFunction PiecewiseLinearFunction::GenerateLinearFunction(
        FunctionGenerator linear_function_generator,
        ShiftComparator shift_comparator,
        Segment function_domain)
{
    long double prev_x_coord = function_domain.GetStart();
    auto coords_count = GenerateRandomValue(kMinCoordsCount, kMaxCoordsCount);
    auto x_coords = GenerateVectorOfValues<>(function_domain.GetStart(), function_domain.GetEnd(), coords_count);
    std::vector<LinearFunctionDefineOnSegment> functions;

    for (auto&& x_coord : x_coords) {
        auto func = linear_function_generator(prev_x_coord, x_coord);
        auto func_value_at_start_point = func.GetValueAtStartPoint().GetSinglePoint();
        if (!functions.empty()) {
            auto prev_func_value_at_end_point = functions.back().GetValueAtEndPoint().GetSinglePoint();

            if (shift_comparator(func.GetValueAtStartPoint().GetSinglePoint(), prev_func_value_at_end_point)) {
                auto shift_value = prev_func_value_at_end_point - func_value_at_start_point;
                auto small_add = GenerateRandomValue<long double>(0.1, 1.);
                if (shift_value > 0) {
                    shift_value += small_add;
                } else {
                    shift_value -= small_add;
                }
                func.Shift(shift_value);
            }

            func_value_at_start_point = func.GetValueAtStartPoint().GetSinglePoint();
            if (func_value_at_start_point != prev_func_value_at_end_point) {
                auto min_y_value = std::min(func_value_at_start_point, prev_func_value_at_end_point);
                auto max_y_value = std::max(func_value_at_start_point, prev_func_value_at_end_point);
                assert(!functions.back().IsVertical());
                functions.emplace_back(min_y_value, max_y_value, prev_x_coord);
            }
        }
        if (!functions.empty() && !functions.back().IsVertical() &&
        func.GetLinearFunction().GetACoeff() == 0 && functions.back().GetLinearFunction().GetACoeff() == 0 &&
                func.GetValueAtStartPoint()  == functions.back().GetValueAtEndPoint()) {
            functions.back().SetXEndCoordinate(x_coord);
        } else {
            functions.emplace_back(func);
        }
        prev_x_coord = x_coord;
    }
    functions.back().SetXEndCoordinate(function_domain.GetEnd());

    return PiecewiseLinearFunction(functions, function_domain);
}

Segment PiecewiseLinearFunction::GetValueAtPoint(long double x) const {
    if (x >= function_domain_.GetStart() && x <= function_domain_.GetEnd()) {
        Segment answer;
        for (auto&& f : functions_) {
            if (x >= f.GetXStartCoordinate() && x <= f.GetXEndCoordinate()) {
                if (f.IsVertical()) {
                    return f.GetValueAtPoint(x);
                } else {
                   answer = f.GetValueAtPoint(x);
                }
            }
        }
        return answer;
    } else {
        throw std::runtime_error("X coordinate does not contain function domain");
    }
}


std::optional<Segment> PiecewiseLinearFunction::GetHorizontalSegmentWithPoint(long double x) const {
    std::optional<Segment> result;

    if (x >= function_domain_.GetStart() && x <= function_domain_.GetEnd()) {
        for (auto&& f : functions_) {
            if (x >= f.GetXStartCoordinate() && x <= f.GetXEndCoordinate() && f.IsHorizontal()) {
                result = Segment(f.GetXStartCoordinate(), f.GetXEndCoordinate());
            }
        }
    } else {
        throw std::runtime_error("X coordinate does not contain function domain");
    }
    return result;
}

void PiecewiseLinearFunction::ValidateFunctions() {
    if (function_domain_.GetStart() != functions_[0].GetXStartCoordinate()) {
        throw std::runtime_error("Start coordinate of first function does not match func domain");
    }

    if (function_domain_.GetEnd() != functions_.back().GetXEndCoordinate()) {
        throw std::runtime_error("Start coordinate of first function does not match func domain");
    }

    for (size_t func_pos = 1; func_pos < functions_.size(); func_pos++) {
        if (fabs(functions_[func_pos - 1].GetXEndCoordinate() - functions_[func_pos].GetXStartCoordinate()) > kEPS) {
            throw std::runtime_error("Function segments intersects");
        }
        if (functions_[func_pos].IsVertical()) {
            auto val = functions_[func_pos - 1].GetValueAtEndPoint().GetSinglePoint();
            auto val2 = functions_[func_pos].GetValueAtEndPoint();
            auto flag = (fabs(val - val2.GetStart()) < kEPS || fabs(val - val2.GetEnd()) < kEPS);
            if (!flag) {
                throw std::runtime_error("Prev function end point val don't match to next function value");
            }
        } else if (functions_[func_pos - 1].IsVertical()) {
            auto val = functions_[func_pos].GetValueAtStartPoint().GetSinglePoint();
            auto val2 = functions_[func_pos - 1].GetValueAtEndPoint();
            auto flag = (fabs(val - val2.GetStart()) < kEPS || fabs(val - val2.GetEnd()) < kEPS);
            if (!flag) {
                throw std::runtime_error("Prev function end point val don't match to next function value");
            }
        } else {
            auto val_prev = functions_[func_pos - 1].GetValueAtEndPoint().GetSinglePoint();
            auto val_curr = functions_[func_pos].GetValueAtStartPoint().GetSinglePoint();
            auto diff = fabs(val_prev - val_curr);
            if (diff > kEPS) {
                throw std::runtime_error("Prev function end point val don't match to next function value");
            }
        }
    }
    for (size_t func_pos = 1; func_pos < functions_.size(); func_pos++) {
        auto x_end_prev = functions_[func_pos - 1].GetXEndCoordinate();
        auto x_start = functions_[func_pos].GetXStartCoordinate();
        if (x_start > x_end_prev) {
            functions_[func_pos].SetXStartCoordinate(x_end_prev);
        } else if (x_start < x_end_prev) {
            functions_[func_pos].SetXStartCoordinate(x_end_prev);
        }
    }

    std::vector<LinearFunctionDefineOnSegment> result_functions{functions_[0]};
    for (size_t func_pos = 1; func_pos < functions_.size(); func_pos++) {
        if (functions_[func_pos - 1].IsHorizontal() && functions_[func_pos].IsHorizontal()) {
            result_functions.back().SetXEndCoordinate(functions_[func_pos].GetXEndCoordinate());
        } else {
            result_functions.push_back(functions_[func_pos]);
        }
    }
    functions_ = result_functions;
}
