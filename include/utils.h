#pragma once
#include "segment.h"
#include <random>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <unordered_set>

constexpr long double kEPS = 0.0000001;
constexpr int64_t kINF = 1e4;

template<typename T>
using is_sampled = std::__or_<std::is_integral<T>, std::is_floating_point<T>>;

template<typename T>
typename std::enable_if<is_sampled<T>::value, T>::type GenerateRandomValue(const T& kMinValue,
        const T& kMaxValue) {
    static std::random_device rd;
    static std::mt19937 rng(rd());
    if constexpr(std::is_floating_point<T>::value) {
        std::uniform_real_distribution<T> uni(kMinValue, kMaxValue);
        auto res = uni(rng);
        return res;
    }
    else if constexpr (std::is_integral<T>::value) {
        std::uniform_int_distribution<T> uni(kMinValue, kMaxValue);
        auto res = uni(rng);
        return res;
    }

}

template<typename T>
typename std::enable_if<is_sampled<T>::value, std::vector<T>>::type GenerateVectorOfValues(
        const T& kMinValue, const T& kMaxValue, size_t sequence_size) {
    std::unordered_set<T> already_generated_values;
    std::vector<T> result;
    size_t num = 0;
    while (num < sequence_size) {
        auto val = GenerateRandomValue(kMinValue, kMaxValue);
        if (!already_generated_values.count(val)) {
            result.emplace_back(val);
            already_generated_values.insert(val);
            num++;
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}
