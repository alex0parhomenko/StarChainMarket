#pragma once
#include "piecewise_linear_function.h"
#include "utils.h"
#include <string>
#include <random>
#include <memory>

class Node {
public:
    Node() = default;
    Node(const Node& other) = default;
    Node& operator=(const Node& other) = default;
    bool operator==(const Node& other) const;

    Node(PiecewiseLinearFunction D, PiecewiseLinearFunction S);

    int64_t GetUniqueId() const noexcept;

    int64_t GetDepth() const;
    void SetDepth(size_t depth);

    PiecewiseLinearFunction GetDeltaS();
    void SetDeltaSDash(PiecewiseLinearFunction delta_s_dash);

    PiecewiseLinearFunction GetDeltaSDash();

    void GenerateNewUniqueId() noexcept;

    PiecewiseLinearFunction GetS() const {
        return S_;
    }

    bool IsLeaf() const {
        return is_leaf_;
    }

    void SetLeaf(bool is_leaf) {
        is_leaf_ = is_leaf;
    }

    PiecewiseLinearFunction GetD() const {
        return D_;
    }

    auto GetVs() const {
        return vs_;
    }

    auto SetVs(long double v) {
        vs_ = v;
    }

    auto GetVd() const {
        return vd_;
    }

    auto SetVd(long double vd) {
        vd_ = vd;
    }

    long double GetP() const {
        return p_;
    }

    void SetP(long double p) {
        p_ = p;
    }

    long double GetZeroPrice() {
        return GetDeltaS().FindFunctionZeroValue().GetSinglePoint();
    }

    long double GetDemandZeroingPrice() {
        long double result = -1;
        for (auto&& func : D_.GetFunctions()) {
            if (func.IsHorizontal() && func.GetLinearFunction().GetACoeff() == 0 && func.GetLinearFunction().GetCCoeff() == 0) {
                result = func.GetXStartCoordinate();
                break;
            }
        }
        if (result == -1) {
            throw std::runtime_error("Can't find demand zeroing price");
        }
        return result;
    }

    void ExtendSupplyAndDemandFunctions(long double x) {
        S_.ExtendFunctionDomain(x);
        D_.ExtendFunctionDomain(x);
    }

    static std::shared_ptr<Node> GenerateRandomNode(long double c, long double d);
    static std::shared_ptr<Node> GenerateRandomNodeWithZeroPriceMoreThan(long double p);
    static std::shared_ptr<Node> GenerateRandomNodeWithZeroPriceLessThan(long double p);
private:
    // Unique identifier for node
    int64_t unique_id_;
    // Функция спроса
    PiecewiseLinearFunction D_;
    // Функция предложения
    PiecewiseLinearFunction S_;
    // Функция чистого предложения
    PiecewiseLinearFunction delta_S_;
    // S-D balance
    PiecewiseLinearFunction delta_S_dash_;
    // Глубина вершины в дереве
    int64_t depth_ = -1;
    // Объем потребления в узле
    long double vs_;
    // Объем производства
    long double vd_;
    // Цена в узле
    long double p_;
    // Является ли вершина листом
    bool is_leaf_;
};
