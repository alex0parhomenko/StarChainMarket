#pragma once
#include "utils.h"
#include "segment.h"
#include <stdexcept>
#include <fstream>

/*
 * Implementation of function a * x + b * y = c
 * y = c / b - a * x / b, b != 0
 * B coeff must be non-zero
 */
class LinearFunction {
public:
    LinearFunction(long double a_coeff, long double b_coeff, long double c_coeff);

    LinearFunction& operator=(const LinearFunction&) = default;

    long double GetValueAtPoint(long double x) const noexcept;

    bool operator==(const LinearFunction& other) const noexcept;

    LinearFunction operator+(const LinearFunction& other) const;
    LinearFunction operator-(const LinearFunction& other) const;

    LinearFunction GetInverse() const;

    void Shift(long double x);

    void ShiftXAxis(long double x);

    void Normalize();

    bool IsHorizontal() const {
        return a_coeff_ == 0;
    }

    static LinearFunction GenerateNonIncreasingLinearFunction();

    static LinearFunction GenerateNonDecreasingLinearFunction();

    long double GetACoeff() const noexcept;

    long double GetBCoeff() const noexcept;

    long double GetCCoeff() const noexcept;

    LinearFunction DerivativeX() const noexcept;
private:
    void Validate() const;

    long double a_coeff_, b_coeff_, c_coeff_;
};

struct Point {
    Point(long double x, long double y) : x_coord_(x), y_coord_(y) {};

    Point SwapCoordinates() const {
        return Point{y_coord_, x_coord_};
    }

    long double x_coord_, y_coord_;
};

std::ostream& operator<<(std::ostream& os, const Point& p);
std::istream& operator>>(std::istream &in, Point& p);
