#include "edge.h"

Edge::Edge(double et, double Q, double ef, double Ev_coeff, std::shared_ptr<Node> from,
        std::shared_ptr<Node> to, EdgeType type)
        :et_(et), Q_(Q), ef_(ef), Ev_coeff_(Ev_coeff), from_(from), to_(to), is_expand_(false), qij_(-1),
         type_(type), algorithm_edge_type_(AlgorithmType::L_undefined) {
    if (from->GetZeroPrice() > to->GetZeroPrice()) {
        //throw std::runtime_error("Edge must start in node with zero price less than end");
    }
    CalcEFunction();
    CalcevFunction();
}

void Edge::CalcEFunction() {
    if (is_expand_) {
        E_ = [&](auto&& q)->double {
            if (fabs(q) > Q_) {
                auto val = (fabs(q) - Q_);
                return ef_ + Ev_coeff_ * val * val + et_ * fabs(q);
            } else {
                return ef_ + et_ * fabs(q);
            }
        };
    } else {
        E_ = [&](auto&& q) -> double {
            if (fabs(q) > Q_) {
                throw std::runtime_error("Edge is not expand");
            } else {
                return et_ * fabs(q);
            }
        };
    }
}

void Edge::CalcevFunction() {
    std::vector<LinearFunctionDefineOnSegment> functions;
    if (is_expand_) {
        functions.emplace_back(-kINF, et_, 0);
        functions.emplace_back(LinearFunction(0, 1, et_), 0, Q_);
        functions.emplace_back(LinearFunction(-2 * Ev_coeff_, 1, et_ - 2 * Ev_coeff_ * Q_), Q_, kINF);
        ev_ = PiecewiseLinearFunction(functions, Segment(0, kINF));
    } else {
        functions.emplace_back(-kINF, et_, 0);
        functions.emplace_back(LinearFunction(0, 1, et_), 0, Q_);
        functions.emplace_back(et_, et_ + kINF, Q_);
        ev_ = PiecewiseLinearFunction(functions, Segment(0, Q_));
    }
}

bool Edge::operator==(const Edge& other) const {
    return from_ == other.from_ && to_ == other.to_;
}

std::shared_ptr<Node> Edge::GetStartNode() const {
    return from_;
}

std::shared_ptr<Node> Edge::GetEndNode() const {
    return to_;
}

std::shared_ptr<Node> Edge::GetAnotherNode(std::shared_ptr<Node> node) {
    return to_ == node ? from_ : to_;
}

bool Edge::IsExpand() {
    return is_expand_;
}

void Edge::SetLineExpand() {
    is_expand_ = true;
    CalcEFunction();
    CalcevFunction();
}

void Edge::SetLineNotExpand() {
    is_expand_ = false;
    CalcEFunction();
    CalcevFunction();
}

std::shared_ptr<Edge> Edge::GenerateRandomEdge(std::shared_ptr<Node> from, std::shared_ptr<Node> to, EdgeType type) {
    auto et = GenerateRandomValue(1, 10);
    auto Q = GenerateRandomValue(1, 10);
    auto ef = GenerateRandomValue(1, 10);
    auto Ev_coeff = GenerateRandomValue(1, 10);
    return std::make_shared<Edge>(et, Q, ef, Ev_coeff, from, to, type);
}

long double Edge::Getqij() const {
    return qij_;
}

std::shared_ptr<Node> Edge::GetqijParentNode() {
    return parent_qij_node_;
}

void Edge::Setqij(long double qij) {
    qij_ = qij;
}

PiecewiseLinearFunction Edge::Getev() const {
    return ev_;
}

long double Edge::Getet() const {
    return et_;
}

long double Edge::GetQ() const {
    return Q_;
}

void Edge::SetDeltaSij(PiecewiseLinearFunction deltaSij) {
    delta_S_ij_ = deltaSij;
}

PiecewiseLinearFunction Edge::GetDeltaSij() const {
    return delta_S_ij_;
}

long double Edge::GetEValueAtPoint(long double x) {
    return E_(x);
}
