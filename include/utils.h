#pragma once
#include "segment.h"
#include <random>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include <sstream>

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

inline auto Split(const std::string& txt, std::vector<std::stringstream>& strs, char ch) {
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        std::stringstream ss;
        ss << txt.substr( initialPos, pos - initialPos );
        strs.emplace_back(std::move(ss));
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    std::stringstream ss;
    ss << txt.substr( initialPos, pos - initialPos );
    strs.emplace_back(std::move(ss));
    return strs.size();
}
