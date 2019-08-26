#include "linear_function.h"

LinearFunction::LinearFunction(long double a_coeff, long double b_coeff, long double c_coeff)
        :a_coeff_(a_coeff), b_coeff_(b_coeff), c_coeff_(c_coeff) {
    Validate();
    Normalize();
}

long double LinearFunction::GetValueAtPoint(long double x) const noexcept {
    return (c_coeff_ - a_coeff_ * x) / b_coeff_;
}

bool LinearFunction::operator==(const LinearFunction& other) const noexcept {
    return other.a_coeff_ == a_coeff_ && other.b_coeff_ == b_coeff_
            && other.c_coeff_ == c_coeff_;
}

LinearFunction LinearFunction::operator+(const LinearFunction& other) const {
    return LinearFunction(a_coeff_ * other.b_coeff_ + other.a_coeff_ * b_coeff_,
            b_coeff_ * other.b_coeff_, c_coeff_ * other.b_coeff_ + other.c_coeff_ * b_coeff_);
}

void LinearFunction::Normalize() {
    a_coeff_ /= b_coeff_;
    c_coeff_ /= b_coeff_;
    b_coeff_ = 1.;
}

LinearFunction LinearFunction::operator-(const LinearFunction& other) const {
    return LinearFunction(a_coeff_ * other.b_coeff_ - other.a_coeff_ * b_coeff_,
            b_coeff_ * other.b_coeff_, c_coeff_ * other.b_coeff_ - other.c_coeff_ * b_coeff_);
}

LinearFunction LinearFunction::GetInverse() const {
    return LinearFunction(b_coeff_, a_coeff_, c_coeff_);
}

LinearFunction LinearFunction::DerivativeX() const noexcept {
    return LinearFunction(0, 1, -a_coeff_ / b_coeff_);
}

void LinearFunction::Shift(long double x) {
    c_coeff_ += x * b_coeff_;
}

void LinearFunction::ShiftXAxis(long double x) {
    c_coeff_ -= a_coeff_ * x;
}

long double LinearFunction::GetACoeff() const noexcept {
    return a_coeff_;
}

long double LinearFunction::GetBCoeff() const noexcept {
    return b_coeff_;
}

long double LinearFunction::GetCCoeff() const noexcept {
    return c_coeff_;
}

LinearFunction LinearFunction::GenerateNonIncreasingLinearFunction() {
    auto a_coeff = GenerateRandomValue<int>(-15, 15);
    auto b_coeff = GenerateRandomValue<int>(-15, 15);
    while (b_coeff == 0) {
        b_coeff = GenerateRandomValue(-15, 15);
    }
    auto c_coeff = GenerateRandomValue(-15, 15);
    if (a_coeff * b_coeff < 0) {
        if (GenerateRandomValue(0, 100) % 2 == 0) {
            a_coeff *= -1;
        }
        else {
            b_coeff *= -1;
        }
    }
    return LinearFunction(a_coeff, b_coeff, c_coeff);
}

LinearFunction LinearFunction::GenerateNonDecreasingLinearFunction() {
    auto a_coeff = GenerateRandomValue(-15., 15.);
    auto b_coeff = GenerateRandomValue(-15., 15.);
    while (b_coeff == 0) {
        b_coeff = GenerateRandomValue(-15., 15.);
    }
    auto c_coeff = GenerateRandomValue(-15., 15.);
    if (a_coeff * b_coeff > 0) {
        if (GenerateRandomValue(0, 100) % 2 == 0) {
            a_coeff *= -1;
        } else {
            b_coeff *= -1;
        }
    }
    return LinearFunction(a_coeff, b_coeff, c_coeff);
}

void LinearFunction::Validate() const {
    if (b_coeff_ == 0) {
        throw std::runtime_error("B coeff must be non zero");
    }
}

std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "(" << p.x_coord_ << "," << p.y_coord_ << ")";
    return os;
}

std::istream& operator>>(std::istream& in, Point& p) {
    char c;
    in.get(c);
    assert(c == '(');
    in >> p.x_coord_;
    in.get(c);
    assert(c == ',');
    in >> p.y_coord_;
    in.get(c);
    assert(c == ')');
    return in;
}
