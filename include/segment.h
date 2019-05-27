#pragma once
#include <assert.h>
#include <algorithm>
#include <stdexcept>

class Segment {
public:
    Segment() : start_(0), end_(0) {};
    Segment(long double start, long double end) : start_(start), end_(end) {
        if (start_ > end_) {
            std::swap(start_, end_);
        }
    }

    /*
     * If we sure that only one point in segment
     */
    long double GetSinglePoint() const {
        if (start_ != end_) {
            throw std::runtime_error("Start not equal to end");
        }
        assert(start_ == end_);
        return start_;
    }

    long double GetStart() const noexcept {
        return start_;
    }

    long double GetLength() const noexcept {
        return end_ - start_;
    }

    void SetStart(long double x) noexcept {
        start_ = x;
    }

    long double GetEnd() const noexcept {
        return end_;
    }

    void SetEnd(long double x) noexcept {
        end_ = x;
    }

    bool IsSinglePoint() const {
        return start_ == end_;
    }

    long double GetProportion(long double lambda) const {
        return start_ + (end_ - start_) * lambda;
    }

    long double FindProportion(long double x) const {
        if (x >= start_ && x <= end_) {
            return (x - start_) / GetLength();
        }
        throw std::runtime_error("Can't find proportion for point which outside the segment");
    }

    Segment operator+(Segment other) const {
        return Segment(start_ + other.start_, end_ + other.end_);
    }

    Segment operator-(Segment other) const {
        return Segment(start_ - other.start_, end_ - other.end_);
    }

    bool operator==(const Segment& other) const {
        return start_ == other.start_ && end_ == other.end_;
    }

    bool operator!=(Segment other) const {
        return !(*this == other);
    }
private:
    long double start_, end_;
};
