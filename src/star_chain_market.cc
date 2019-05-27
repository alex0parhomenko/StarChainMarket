#include "star_chain_market.h"
#include <cassert>
#include <variant>

bool StarChainMarket::AddNode(std::shared_ptr<Node> node, bool is_central_market_node) {
    if (MarketContainNode(node)) {
        return false;
    }
    unique_id_to_vector_pos_[node->GetUniqueId()] = nodes_.size();
    nodes_.emplace_back(node);
    matrix_.emplace_back();
    if (is_central_market_node) {
        central_market_node_ = node;
    }
    return true;
}

bool StarChainMarket::AddEdge(std::shared_ptr<Edge> edge) {
    auto from = edge->GetStartNode();
    auto to = edge->GetEndNode();
    if (MarketContainNode(from) && MarketContainNode(to)) {
        auto from_index = GetVectorPosByNode(from);
        auto to_index = GetVectorPosByNode(to);
        matrix_[from_index].push_back(edge);
        matrix_[to_index].push_back(edge);
        edges_.push_back(edge);
        return true;
    }
    return false;
}

bool StarChainMarket::AddNodeAndEdge(std::shared_ptr<Node> node, std::shared_ptr<Edge> edge) {
    return AddNode(node) && AddEdge(edge);
}

void StarChainMarket::SetCentralNode(std::shared_ptr<Node> node) {
    central_market_node_ = node;
}

bool StarChainMarket::MarketContainNode(std::shared_ptr<Node> node) const noexcept {
    return unique_id_to_vector_pos_.count(node->GetUniqueId());
}

int64_t StarChainMarket::GetVectorPosByNode(std::shared_ptr<Node> node) {
    return unique_id_to_vector_pos_[node->GetUniqueId()];
}

std::optional<StarChainMarket> StarChainMarket::GenerateRandomMarket(int64_t import_from_center_nodes_count,
        int64_t export_to_center_node_nodes_count, int64_t chain_nodes_count) {
    StarChainMarket market;
    auto is_success = true;
    auto c = GenerateRandomValue(1, 10);
    auto d = GenerateRandomValue(1, 10);
    auto central_node = Node::GenerateRandomNode(c, d * c);
    market.AddNode(central_node, true);

    for (auto import_num = 0; import_num < import_from_center_nodes_count; import_num++) {
        auto node = Node::GenerateRandomNodeWithZeroPriceMoreThan(central_node->GetZeroPrice());
        auto edge = Edge::GenerateRandomEdge(central_node, node, EdgeType::STAR_FROM_CENTER);
        is_success = is_success && market.AddNodeAndEdge(node, edge);
    }

    for (auto export_num = 0; export_num < export_to_center_node_nodes_count; export_num++) {
        auto node = Node::GenerateRandomNodeWithZeroPriceLessThan(central_node->GetZeroPrice());
        auto edge = Edge::GenerateRandomEdge(node, central_node, EdgeType::STAR_TO_CENTER);
        is_success = is_success && market.AddNodeAndEdge(node, edge);
    }

    auto prev_node_in_chain = central_node;
    for (auto chain_num = 0; chain_num < chain_nodes_count; chain_num++) {
        auto c = GenerateRandomValue(1, 10);
        auto d = GenerateRandomValue(1, 10);
        auto node = Node::GenerateRandomNode(c, c * d);
        auto edge = (node->GetZeroPrice() > prev_node_in_chain->GetZeroPrice()) ? Edge::GenerateRandomEdge(
                prev_node_in_chain, node, EdgeType::CHAIN_FROM_CENTER) : Edge::GenerateRandomEdge(node,
                prev_node_in_chain, EdgeType::CHAIN_TO_CENTER);
        is_success = is_success && market.AddNodeAndEdge(node, edge);
        prev_node_in_chain = node;
    }
    market.ExtendAllSupplyAndDemandFunctionsToMaxDemandZeroingPrice();
    return is_success ? std::optional(market) : std::optional<StarChainMarket>();
}

void StarChainMarket::Dfs(size_t node_pos, std::vector<bool>& used, int64_t depth) {
    nodes_[node_pos]->SetDepth(depth);
    used[node_pos] = true;
    bool is_leaf = true;
    for (auto&& edge : matrix_[node_pos]) {
        auto to_index = GetVectorPosByNode(edge->GetAnotherNode(nodes_[node_pos]));
        if (!used[to_index]) {
            is_leaf = false;
            Dfs(to_index, used, depth + 1);
        }
    }
    nodes_[node_pos]->SetLeaf(is_leaf);
}

bool StarChainMarket::FindTreeRootWithMinDepth(size_t node_pos, size_t to, int64_t path_length,
        std::vector<bool>& used) {
    used[node_pos] = true;
    if (node_pos == to) {
        return true;
    }
    for (auto&& edge : matrix_[node_pos]) {
        auto to_index = GetVectorPosByNode(edge->GetAnotherNode(nodes_[node_pos]));
        if (!used[to_index]) {
            bool is_on_path = FindTreeRootWithMinDepth(to_index, to, path_length - 1, used);
            if (is_on_path) {
                if (path_length == 0) {
                    tree_root_pos_ = node_pos;
                }
                return true;
            }
        }
    }
    return false;
}

void StarChainMarket::BuildTreeMinDepth() {
    auto random_node_pos = GenerateRandomValue<int>(0, kINF) % nodes_.size();
    while (matrix_[random_node_pos].size() == 1) {
        random_node_pos = GenerateRandomValue<int>(0, kINF) % nodes_.size();
    }

    std::vector<bool> used(nodes_.size(), false);
    Dfs(random_node_pos, used, 0);

    std::shared_ptr<Node> max_node, min_node;
    for (auto&& node : nodes_) {
        if (node->IsLeaf()) {
            auto depth = node->GetDepth();
            if (!max_node || depth > max_node->GetDepth()) {
                max_node = node;
            }
            if (!min_node || depth < min_node->GetDepth()) {
                min_node = node;
            }
        }
    }

    if (fabs(min_node->GetDepth() - max_node->GetDepth()) <= 1) {
        tree_root_pos_ = random_node_pos;
    } else {
        auto graph_diameter = min_node->GetDepth() + max_node->GetDepth();
        auto min_depth = graph_diameter / 2;
        used.assign(used.size(), false);
        auto res = FindTreeRootWithMinDepth(GetVectorPosByNode(min_node),
                GetVectorPosByNode(max_node),
                min_depth, used);
        assert(res == true);
    }
}

PiecewiseLinearFunction StarChainMarket::CreateDeltaSForLine(std::shared_ptr<Node> node,
        std::shared_ptr<Edge> edge) {
    auto to_node = edge->GetAnotherNode(node);

    auto Si_inv = to_node->GetDeltaSDash().GetInverseFunction();
    auto eij = edge->Getev();

    PiecewiseLinearFunction eij_final;
    // Одинаковое направление
    if (edge->GetStartNode() == to_node) {
        eij_final = eij;
    } // Разное направление
    else {
        eij_final = eij.MirrorXAndY();
    }
    auto sum = Si_inv + eij_final;
    auto sum_inv = sum.GetInverseFunction();
    return sum_inv;
}

void StarChainMarket::SolveAuxiliarySubtask() {
    std::vector<bool> used(nodes_.size(), false);
    FindSDBalance(tree_root_pos_, used);
    used.assign(nodes_.size(), false);
    FindMarketParameters(nullptr, tree_root_pos_, used, -1);
    CheckFoundMarketParameters();
}

void StarChainMarket::FindSDBalance(size_t node_pos, std::vector<bool>& used) {
    used[node_pos] = true;
    bool is_leaf = true;
    std::vector<std::shared_ptr<Edge>> children_edges;
    for (auto&& edge : matrix_[node_pos]) {
        auto to_index = GetVectorPosByNode(edge->GetAnotherNode(nodes_[node_pos]));
        if (!used[to_index]) {
            children_edges.emplace_back(edge);
            is_leaf = false;
            FindSDBalance(to_index, used);
        }
    }
    if (is_leaf) {
        nodes_[node_pos]->SetDeltaSDash(nodes_[node_pos]->GetDeltaS());
    } else {
        PiecewiseLinearFunction delta_S_dash = nodes_[node_pos]->GetDeltaS();

        for (auto&& child_edge : children_edges) {
            auto delta_S_for_edge = CreateDeltaSForLine(nodes_[node_pos], child_edge);
            child_edge->SetDeltaSij(delta_S_for_edge);
            delta_S_dash = delta_S_dash + delta_S_for_edge;
        }
        nodes_[node_pos]->SetDeltaSDash(delta_S_dash);
    }
}

void StarChainMarket::FindMarketParameters(std::shared_ptr<Edge> edge, size_t node_pos,
        std::vector<bool>& used, double lambda) {
    used[node_pos] = true;
    auto curr_node = nodes_[node_pos];

    long double qij = 0;
    // Not Root node
    if (edge) {
        auto parent_node = edge->GetAnotherNode(curr_node);

        // Find qij
        auto qij_segment = edge->GetDeltaSij().GetValueAtPoint(parent_node->GetP());
        if (qij_segment.IsSinglePoint()) {
            qij = qij_segment.GetSinglePoint();

            auto segment = edge->GetDeltaSij().GetHorizontalSegmentWithPoint(parent_node->GetP());
            if (segment.has_value()) {
                lambda = segment.value().FindProportion(parent_node->GetP());
            }
        } else {
            // Если lambda = 0 то дальше она будет равна 1 и если lambda = 1 то дальше она равняется 0
            qij = qij_segment.GetProportion(lambda);
        }
        edge->Setqij(qij);
        edge->SetqijParentNode(parent_node);
    }

    // Finding pi
    auto pi_segment = curr_node->GetDeltaSDash().GetInverseFunction().GetValueAtPoint(qij);
    if (pi_segment.IsSinglePoint()) {
        curr_node->SetP(pi_segment.GetSinglePoint());
        auto segment = curr_node->GetDeltaSDash().GetInverseFunction().GetHorizontalSegmentWithPoint(qij);
        if (segment.has_value()) {
            lambda = segment.value().FindProportion(qij);
        }
    } else {
        // Если lambda = 0 то дальше она будет равна 1 и если lambda = 1 то дальше она равняется 0
        curr_node->SetP(pi_segment.GetProportion(lambda));
    }

    // Finding vs
    auto vs_segment = curr_node->GetS().GetValueAtPoint(curr_node->GetP());
    if (vs_segment.IsSinglePoint()) {
        curr_node->SetVs(vs_segment.GetSinglePoint());
    } else {
        curr_node->SetVs(vs_segment.GetProportion(lambda));
    }

    // Finding vd
    auto vd_segment = curr_node->GetD().GetValueAtPoint(curr_node->GetP());
    long double vd = 0;
    if (vd_segment.IsSinglePoint()) {
        curr_node->SetVd(vd_segment.GetSinglePoint());
    } else {
        curr_node->SetVd(vd_segment.GetProportion(1 - lambda));
    }

    for (auto&& edge : matrix_[node_pos]) {
        auto to_index = GetVectorPosByNode(edge->GetAnotherNode(curr_node));
        if (!used[to_index]) {
            FindMarketParameters(edge, to_index, used, lambda);
        }
    }
}

void StarChainMarket::CheckFoundMarketParameters() {
    for (auto&& node : nodes_) {
        auto p = node->GetP();
        auto vs1 = node->GetVs();
        auto vs2 = node->GetS().GetValueAtPoint(p).GetSinglePoint();
        if (fabs(vs1 - vs2) > kEPS) {
            throw std::runtime_error("Vs different");
        }

        long double streams_sum = 0.0;
        long double val = node->GetDeltaS().GetValueAtPoint(node->GetP()).GetSinglePoint();
        for (auto&& edge : matrix_[GetVectorPosByNode(node)]) {
            if (edge->GetqijParentNode() == node) {
                streams_sum -= edge->Getqij();
            } else {
                streams_sum += edge->Getqij();
            }
        }

        if (fabs(val - streams_sum) > kEPS) {
            throw std::runtime_error("Streams and delta S different in p " + std::to_string(val) +
                    " " + std::to_string(streams_sum));
        }
    }

    for (auto&& edge : edges_) {
        auto parent_node = edge->GetqijParentNode();
        auto child_node = edge->GetAnotherNode(parent_node);
        auto pj = parent_node->GetP();
        auto pi = child_node->GetP();
        auto qij = edge->Getqij();
        auto Q = edge->GetQ();
        auto et = edge->Getet();
        auto ev_coeff = edge->GetEvCoeff();

        if (fabs(pj - pi + et) < kEPS) {
            assert(qij >= -Q);
            assert(qij <= 0);
        } else if (fabs(pj - pi - et) < kEPS) {
            assert(qij >= 0);
            assert(qij <= Q);
        } else if (!edge->IsExpand() && pj - pi < -et) {
            assert(fabs(qij + Q) < kEPS);
        } else if (edge->IsExpand() && qij < -Q) {
            assert(pj - pi ==
                    -edge->Getet() + 2 * ev_coeff * (fabs(qij) - Q));
        } else if (pj - pi > -et && pj - pi < et) {
            assert(fabs(qij) < kEPS);
        }  else if (edge->IsExpand() && qij > Q) {
            assert(pj - pi == et + 2 * ev_coeff * (fabs(qij) - Q));
        } else if (!edge->IsExpand() && pj - pi > et) {
            assert(fabs(qij - Q) < kEPS);
        }
    }
}

long double StarChainMarket::CalculateWelfare() {
    long double answer = 0.0;
    for (auto&& node : nodes_) {
        auto D_inv = node->GetD().GetInverseFunction();
        auto S_inv = node->GetS().GetInverseFunction();

        auto vd = node->GetVd();
        auto vs = node->GetVs();

        auto U = D_inv.Integrate(0, node->GetVd());
        auto c = S_inv.Integrate(0, node->GetVs());
        //std::cout << GetVectorPosByNode(node) << " U: " << U << " c: " << c << std::endl;

        answer = answer + U - c;
    }
    long double E_sum = 0.0;
    for (auto&& edge : edges_) {
        E_sum += edge->GetEValueAtPoint(edge->Getqij());
    }
    answer -= E_sum;
    return answer;
}
