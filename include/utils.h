#pragma once
#include "segment.h"
#include <random>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include <map>
#include <sstream>
#include <iostream>

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

inline auto GetOnesBitsCount(int64_t mask) {
    int result = 0;
    for (auto i = 0; i < 64; i++) {
        if ((mask >> i) & 1) {
            result++;
        }
    }
    return result;
}

inline auto GenerateBitMasks(size_t length) {
    if (length > 64) {
        throw std::runtime_error("Length of mask overflow");
    }
    int64_t mask = 0;
    for (size_t i = 0; i < length; i++) {
        mask |= (1 << i);
    }
    std::unordered_map<size_t, std::vector<int64_t>> ones_bits_to_masks;
    while (mask != 0) {
        auto ones_bits_count = GetOnesBitsCount(mask);
        ones_bits_to_masks[ones_bits_count].emplace_back(mask);
        mask--;
    }
    ones_bits_to_masks[0].emplace_back(0);
    return ones_bits_to_masks;
}

inline auto MaskToVector(int64_t mask, size_t edges_count) {
    std::vector<bool> answer;
    for (size_t i = 0; i < edges_count; i++) {
        if ((mask >> i) & 1) {
            answer.emplace_back(true);
        } else {
            answer.emplace_back(false);
        }
    }
    return answer;
}

template<typename Vector>
inline void PrintVector(Vector v) {
    for (auto&& elem : v) {
        std::cout << elem << ", ";
    }
}
