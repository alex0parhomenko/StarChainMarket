#include "node.h"

Node::Node(PiecewiseLinearFunction D, PiecewiseLinearFunction S)
: D_(D), S_(S), delta_S_(S_ - D_), vs_(-1), vd_(-1), p_(-1), is_leaf_(false) {
    GenerateNewUniqueId();
}

bool Node::operator==(const Node& other) const {
    return unique_id_ == other.unique_id_;
}

int64_t Node::GetUniqueId() const noexcept {
    return unique_id_;
}

int64_t Node::GetDepth() const {
    return depth_;
}

PiecewiseLinearFunction Node::GetDeltaS() {
    return delta_S_;
}

PiecewiseLinearFunction Node::GetDeltaSDash() {
    return delta_S_dash_;
}

void Node::SetDeltaSDash(PiecewiseLinearFunction delta_s_dash) {
    delta_S_dash_ = delta_s_dash;
}

void Node::SetDepth(size_t depth) {
    depth_ = depth;
}

void Node::GenerateNewUniqueId() noexcept {
    unique_id_ = GenerateRandomValue<int64_t>(1, kINF);
}

std::shared_ptr<Node> Node::GenerateRandomNode(long double c, long double d) {
    return std::make_shared<Node>(PiecewiseLinearFunction::CreateDpFunction(c, d),
            PiecewiseLinearFunction::CreateSpFunction(c, d));
}

/*
 * d / c > p
 */
std::shared_ptr<Node> Node::GenerateRandomNodeWithZeroPriceMoreThan(long double p) {
    auto c = GenerateRandomValue<int64_t>(1, 10);
    auto d = GenerateRandomValue<int64_t>(1, 10);
    return GenerateRandomNode(c, (d + p) * c);
}

/*
 * d / c < p
 */
std::shared_ptr<Node> Node::GenerateRandomNodeWithZeroPriceLessThan(long double p) {
    auto c = GenerateRandomValue<int64_t>(1, 10);
    auto d = GenerateRandomValue<int64_t>(1, p);
    return GenerateRandomNode(c, d * c);
}
