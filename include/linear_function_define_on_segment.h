#pragma once
#include "linear_function.h"
#include "segment.h"

/*
 * Linear function which define on segment, it can be vertical, inclined, or horizontal segment
 * Start point x coordinate must be different from end point x coordinate
 */
class LinearFunctionDefineOnSegment {
public:
    LinearFunctionDefineOnSegment(const LinearFunction& linear_function, long double x_start, long double x_end);
    LinearFunctionDefineOnSegment(long double min_y, long double max_y, long double x_start);
    LinearFunctionDefineOnSegment(Point start, Point end);

    LinearFunctionDefineOnSegment& operator=(const LinearFunctionDefineOnSegment&) = default;

    bool operator==(const LinearFunctionDefineOnSegment& other) const noexcept;

    Segment GetVerticalSegment() const {
        if (is_vertical_) {
            return vertical_segment_;
        }
        throw std::runtime_error("Can't get vertical segment on not vertical function");
    }

    LinearFunctionDefineOnSegment operator+(const LinearFunctionDefineOnSegment& other);
    LinearFunctionDefineOnSegment operator-(const LinearFunctionDefineOnSegment& other);

    bool IsHorizontal() const;

    Segment GetValueAtPoint(long double x) const;

    LinearFunctionDefineOnSegment GetInverse() const;

    void Shift(long double x);

    void ShiftXAxis(long double x);

    static LinearFunctionDefineOnSegment GenerateNonIncreasingLinearFunction(long double x_start, long double x_end);

    static LinearFunctionDefineOnSegment GenerateNonDecreasingLinearFunction(long double x_start, long double x_end);

    LinearFunction GetLinearFunction() const {
        if (is_vertical_) {
            throw std::runtime_error("Function is vertical");
        }
        return func_;
    }

    void SetLinearFunction(const LinearFunction& func);

    Segment GetValueAtEndPoint() const;

    long double Integrate() const;

    long double Integrate(long double from, long double to) const;

    Segment GetValueAtStartPoint() const;

    long double GetXStartCoordinate() const noexcept;

    void SetXStartCoordinate(long double x_coordinate) noexcept;

    long double GetXEndCoordinate() const noexcept;

    void SetXEndCoordinate(long double x_coordinate) noexcept;

    LinearFunctionDefineOnSegment MirrorXAndY() const noexcept;

    bool IsVertical() const {
        return is_vertical_;
    }

    LinearFunctionDefineOnSegment Derivative() const noexcept;

private:
    void Validate();

    LinearFunction func_;
    Point start_, end_;

    // Is function vertical segment
    bool is_vertical_ = false;
    // Using if we have vertical Function
    Segment vertical_segment_;
};
