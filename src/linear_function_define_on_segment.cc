#include "linear_function_define_on_segment.h"

LinearFunctionDefineOnSegment::LinearFunctionDefineOnSegment(long double min_y, long double max_y, long double x)
        :
        func_(0, 1, 0),
        start_{x, min_y},
        end_{x, max_y},
        is_vertical_(true),
        vertical_segment_(min_y, max_y) {
    if (min_y > max_y) {
        throw std::runtime_error("min_y must be less than max_y");
    }
}

LinearFunctionDefineOnSegment::LinearFunctionDefineOnSegment(const LinearFunction& linear_function,
        long double x_start, long double x_end)
        :
        func_(linear_function),
        start_{x_start, linear_function.GetValueAtPoint(x_start)},
        end_{x_end, linear_function.GetValueAtPoint(x_end)},
        is_vertical_(false) {
    Validate();
}

LinearFunctionDefineOnSegment::LinearFunctionDefineOnSegment(Point start, Point end) :
    func_{start.y_coord_ - end.y_coord_, end.x_coord_ - start.x_coord_,
           end.x_coord_ * start.y_coord_ - start.x_coord_ * end.y_coord_},
    start_(start),
    end_(end),
    is_vertical_(false) {
    Validate();
}

bool LinearFunctionDefineOnSegment::operator==(const LinearFunctionDefineOnSegment& other) const noexcept {
    if (is_vertical_) {
        return other.is_vertical_ && other.vertical_segment_ == vertical_segment_;
    } else {
        return func_ == other.func_ && start_.x_coord_ == other.start_.x_coord_
                && end_.x_coord_ == other.end_.x_coord_;
    }
}

LinearFunctionDefineOnSegment LinearFunctionDefineOnSegment::operator+(
        const LinearFunctionDefineOnSegment& other) {

    if (start_.x_coord_ != other.start_.x_coord_ || end_.x_coord_ != other.end_.x_coord_) {
        throw std::runtime_error("Can't sum two linear function defined on different segments");
    }
    if (!IsVertical() && !other.IsVertical()) {
        auto result = func_ + other.func_;
        return LinearFunctionDefineOnSegment(result, start_.x_coord_, end_.x_coord_);
    } else if (IsVertical() && !other.IsVertical()) {
        auto val = other.GetValueAtEndPoint().GetSinglePoint();
        return LinearFunctionDefineOnSegment(vertical_segment_.GetStart() + val,
                vertical_segment_.GetEnd() + val, start_.x_coord_);
    } else if (!IsVertical() && other.IsVertical()) {
        auto val = GetValueAtEndPoint().GetSinglePoint();
        return LinearFunctionDefineOnSegment(other.vertical_segment_.GetStart() + val,
                other.vertical_segment_.GetEnd() + val, start_.x_coord_);
    } else if (IsVertical() && other.IsVertical()) {
        auto start_coord = other.vertical_segment_.GetStart() + vertical_segment_.GetStart();
        return LinearFunctionDefineOnSegment(start_coord,
                start_coord + other.vertical_segment_.GetLength() + vertical_segment_.GetLength(), start_.x_coord_);
    }

}

LinearFunctionDefineOnSegment LinearFunctionDefineOnSegment::operator-(
        const LinearFunctionDefineOnSegment& other) {
    if (start_.x_coord_ != other.start_.x_coord_ || end_.x_coord_ != other.end_.x_coord_) {
        throw std::runtime_error("Can't sum two linear function defined on different segments");
    }
    if (!IsVertical() && !other.IsVertical()) {
        auto result = func_ - other.func_;
        return LinearFunctionDefineOnSegment(result, start_.x_coord_, end_.x_coord_);
    } else if (IsVertical() && !other.IsVertical()) {
        auto val = other.GetValueAtEndPoint().GetSinglePoint();
        return LinearFunctionDefineOnSegment(vertical_segment_.GetStart() - val,
                vertical_segment_.GetEnd() - val, start_.x_coord_);
    } else if (!IsVertical() && other.IsVertical()) {
        auto val = GetValueAtEndPoint().GetSinglePoint();
        return LinearFunctionDefineOnSegment(val - other.vertical_segment_.GetEnd(), val - other.vertical_segment_.GetStart(), start_.x_coord_);
    } else if (IsVertical() && other.IsVertical()) {
        auto x1 = vertical_segment_.GetStart() - other.vertical_segment_.GetStart();
        auto x2 = x1 + vertical_segment_.GetLength() - other.vertical_segment_.GetLength();
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        return LinearFunctionDefineOnSegment(x1, x2, start_.x_coord_);
    }
    throw std::runtime_error("Unexpected case");
}

Segment LinearFunctionDefineOnSegment::GetValueAtPoint(long double x) const {
    if (x >= start_.x_coord_ && x <= end_.x_coord_) {
        if (is_vertical_) {
            return vertical_segment_;
        } else {
            auto val = func_.GetValueAtPoint(x);
            return Segment(val, val);
        }
    }
    throw std::runtime_error("Coorinate out of bounds");
}

long double LinearFunctionDefineOnSegment::Integrate() const {
    if (is_vertical_) {
        return 0;
    } else {
        auto a_coeff = func_.GetACoeff(), b_coeff = func_.GetBCoeff(), c_coeff = func_.GetCCoeff();
        auto primitive = [&](auto x) {
            return (c_coeff / b_coeff) * x - (a_coeff / (2 * b_coeff)) * x * x;
        };
        return primitive(end_.x_coord_) - primitive(start_.x_coord_);
    }
}

long double LinearFunctionDefineOnSegment::Integrate(long double from, long double to) const {
    if (from > to) {
        throw std::runtime_error("While integrate, from point must be less than to point");
    }
    if (is_vertical_) {
        return 0;
    } else {
        auto a_coeff = func_.GetACoeff(), b_coeff = func_.GetBCoeff(), c_coeff = func_.GetCCoeff();
        auto primitive = [&](auto x) {
            return (c_coeff / b_coeff) * x - (a_coeff / (2 * b_coeff)) * x * x;
        };
        return primitive(to) - primitive(from);
    }
}

LinearFunctionDefineOnSegment LinearFunctionDefineOnSegment::GetInverse() const {
    if (is_vertical_) {
        auto y_coord = start_.x_coord_;
        auto start_x = vertical_segment_.GetStart(), end_x = vertical_segment_.GetEnd();
        return LinearFunctionDefineOnSegment(LinearFunction(0, 1, y_coord), start_x, end_x);
    } else {
        if (func_.GetACoeff() == 0) {
            return LinearFunctionDefineOnSegment(start_.x_coord_, end_.x_coord_, start_.y_coord_);
        } else {
            auto y_start = std::min(start_.y_coord_, end_.y_coord_), y_end = std::max(start_.y_coord_,
                    end_.y_coord_);
            return LinearFunctionDefineOnSegment(func_.GetInverse(), y_start, y_end);
        }
    }
}

void LinearFunctionDefineOnSegment::SetLinearFunction(const LinearFunction& func) {
    if (is_vertical_) {
        throw std::runtime_error("Can't set function for vertical segment");
    } else {
        func_ = func;
        start_.y_coord_ = func_.GetValueAtPoint(start_.x_coord_);
        end_.y_coord_ = func_.GetValueAtPoint(end_.x_coord_);
    }
}
/*
 * Shift up/down
 */
void LinearFunctionDefineOnSegment::Shift(long double x) {
    if (is_vertical_) {
        start_.y_coord_ += x;
        end_.y_coord_ += x;

        auto start = vertical_segment_.GetStart(), end = vertical_segment_.GetEnd();
        vertical_segment_.SetStart(start + x);
        vertical_segment_.SetEnd(end + x);
    } else {
        func_.Shift(x);
        start_.y_coord_ = GetValueAtStartPoint().GetSinglePoint();
        end_.y_coord_ = GetValueAtEndPoint().GetSinglePoint();
    }
}

void LinearFunctionDefineOnSegment::ShiftXAxis(long double x) {
    if (!is_vertical_) {
        func_.ShiftXAxis(x);
        start_.y_coord_ = GetValueAtStartPoint().GetSinglePoint();
        end_.y_coord_ = GetValueAtEndPoint().GetSinglePoint();
    }
}

LinearFunctionDefineOnSegment LinearFunctionDefineOnSegment::GenerateNonIncreasingLinearFunction(
        long double x_start, long double x_end) {
    return LinearFunctionDefineOnSegment(LinearFunction::GenerateNonIncreasingLinearFunction(), x_start,
            x_end);
}

LinearFunctionDefineOnSegment LinearFunctionDefineOnSegment::GenerateNonDecreasingLinearFunction(
        long double x_start, long double x_end) {
    return LinearFunctionDefineOnSegment(LinearFunction::GenerateNonDecreasingLinearFunction(), x_start,
            x_end);
}

Segment LinearFunctionDefineOnSegment::GetValueAtEndPoint() const {
    return GetValueAtPoint(end_.x_coord_);
}

Segment LinearFunctionDefineOnSegment::GetValueAtStartPoint() const {
    return GetValueAtPoint(start_.x_coord_);
}

long double LinearFunctionDefineOnSegment::GetXStartCoordinate() const noexcept {
    return start_.x_coord_;
}

void LinearFunctionDefineOnSegment::SetXStartCoordinate(long double x_coordinate) noexcept {
    if (!is_vertical_) {
        start_.x_coord_ = x_coordinate;
        start_.y_coord_ = func_.GetValueAtPoint(x_coordinate);
        assert(start_.x_coord_ <= end_.x_coord_);
    } else {
        start_.x_coord_ = x_coordinate;
        end_.x_coord_ = x_coordinate;
    }
}

long double LinearFunctionDefineOnSegment::GetXEndCoordinate() const noexcept {
    return end_.x_coord_;
}

void LinearFunctionDefineOnSegment::SetXEndCoordinate(long double x_coordinate) noexcept {
    if (!is_vertical_) {
        end_.x_coord_ = x_coordinate;
        end_.y_coord_ = func_.GetValueAtPoint(x_coordinate);
        assert(start_.x_coord_ <= end_.x_coord_);
    } else {
        start_.x_coord_ = x_coordinate;
        end_.x_coord_ = x_coordinate;
    }
}

bool LinearFunctionDefineOnSegment::IsHorizontal() const {
    return !is_vertical_ && func_.IsHorizontal();
}

LinearFunctionDefineOnSegment LinearFunctionDefineOnSegment::Derivative() const noexcept {
    if (!is_vertical_) {
        return LinearFunctionDefineOnSegment(func_.DerivativeX(), start_.x_coord_, end_.x_coord_);
    }
    throw std::runtime_error("Vertical function can't derivative");
}

LinearFunctionDefineOnSegment LinearFunctionDefineOnSegment::MirrorXAndY() const noexcept {
    if (is_vertical_) {
        assert(start_.x_coord_ == end_.x_coord_);
        auto x = -start_.x_coord_;
        auto y_end = -vertical_segment_.GetStart();
        auto y_start = -vertical_segment_.GetEnd();
        return LinearFunctionDefineOnSegment(y_start, y_end, x);
    } else {
        auto x1 = -end_.x_coord_, y1 = -end_.y_coord_;
        auto x2 = -start_.x_coord_, y2 = -start_.y_coord_;
        auto func = LinearFunction(y1 - y2, x2 - x1, -(x1 * y2 - x2 * y1));
        return LinearFunctionDefineOnSegment(func, x1, x2);
    }
}

void LinearFunctionDefineOnSegment::Validate() {
    // Add validation
}
