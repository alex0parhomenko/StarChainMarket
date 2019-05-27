#pragma once
#include <utils.h>

static auto GenerateFunctionDomain() {
    auto vec = GenerateVectorOfValues(-10000, 10000, 2);
    Segment function_domain(vec[0], vec[1]);
    return function_domain;
}
