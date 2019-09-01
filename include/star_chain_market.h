#pragma once

#include "node.h"
#include "edge.h"
#include <unordered_map>
#include <memory>
#include <fstream>

class StarChainMarket {
public:
    StarChainMarket() = default;

    bool AddNode(std::shared_ptr<Node> node, bool is_central_market_node = false);
    bool AddEdge(std::shared_ptr<Edge> edge);
    bool AddNodeAndEdge(std::shared_ptr<Node> node, std::shared_ptr<Edge> edge);
    void SetCentralNode(std::shared_ptr<Node> node);
    bool MarketContainNode(std::shared_ptr<Node> node) const noexcept;
    void BuildTreeMinDepth();
    int64_t GetVectorPosByNode(std::shared_ptr<Node> node);

    static std::optional<StarChainMarket> GenerateRandomMarket(int64_t import_from_center_nodes_count,
            int64_t export_to_center_node_nodes_count, int64_t chain_nodes_count);

    std::vector<std::shared_ptr<Node>> GetNodes() {
        return nodes_;
    }

    std::vector<std::shared_ptr<Edge>> GetEdges() {
        return edges_;
    }

    std::vector<std::shared_ptr<Edge>> GetUndefinedChainEdges() {
        std::vector<std::shared_ptr<Edge>> result;
        for (auto&& edge : edges_) {
            if ((edge->GetEdgeType() == EdgeType::CHAIN_TO_CENTER
                    || edge->GetEdgeType() == EdgeType::CHAIN_FROM_CENTER)
                    && edge->GetAlgorithmType() == AlgorithmType::L_undefined) {
                result.emplace_back(edge);
            }
        }
        return result;
    }

    std::vector<std::vector<std::shared_ptr<Edge>>>& GetMatrix() {
        return matrix_;
    }

    std::shared_ptr<Node> GetCentralNode() const {
        return central_market_node_;
    }

    bool IsCentralNode(std::shared_ptr<Node> other) const {
        return central_market_node_ == other;
    }

    int64_t GetRootNodePos() const {
        return tree_root_pos_;
    }

    void SolveAuxiliarySubtask();

    void Dfs(size_t node_pos, std::vector<bool>& used, int64_t depth = 0);

    void PrintAll() {
        std::cout << "Tree Root position:\n";
        std::cout << tree_root_pos_ << std::endl;
        PrintNodes();
        PrintEdges();
    }

    int64_t UndefinedLinesCount() {
        int64_t result = 0;
        for (auto&& edge : edges_) {
            if (edge->GetAlgorithmType() == AlgorithmType::L_undefined) {
                result++;
            }
        }
        return result;
    }

    void ExtendAllSupplyAndDemandFunctionsToMaxDemandZeroingPrice() {
        long double zeroing_point = -1;
        for (auto&& node : nodes_) {
            zeroing_point = std::max(zeroing_point, node->GetDemandZeroingPrice());
        }
        zeroing_point += 1;
        for (auto&& node : nodes_) {
            node->ExtendSupplyAndDemandFunctions(zeroing_point);
        }
    }

    void PrintEdges() {
        std::cout << "Graph Edges:\n";
        for (size_t i = 0; i < matrix_.size(); i++) {
            for (auto&& edge : matrix_[i]) {
                auto from = edge->GetStartNode();
                if (from == nodes_[i]) {
                    auto start_node = GetVectorPosByNode(edge->GetStartNode());
                    auto end_node = GetVectorPosByNode(edge->GetEndNode());
                    std::cout << start_node << " -> " << end_node << std::endl;
                    edge->Print();
                    std::cout << start_node << " -> " << end_node << " qij: " << edge->Getqij() << std::endl;
                    std::cout << std::endl;

                }
            }
        }
        std::cout << std::endl;
    }

    size_t GetUndefinedChainLinesCount() {
        size_t answer = 0;
        for (auto&& edge : edges_) {
            if ((edge->GetEdgeType() == EdgeType::CHAIN_TO_CENTER
            || edge->GetEdgeType() == EdgeType::CHAIN_FROM_CENTER)
            && edge->GetAlgorithmType() == AlgorithmType::L_undefined) {
                answer++;
            }
        }
        return answer;
    }

    static StarChainMarket LoadMarket(std::ifstream&);
    static void StoreMarket(std::ofstream&, StarChainMarket market);

    void PrintNodes() {
        for (auto&& node : nodes_) {

            auto node_pos = GetVectorPosByNode(node);
            [[ maybe_unused ]] auto depth = node->GetDepth();
            std::cout << "Node: " << node_pos << " p:" << node->GetP() <<
                      " vs:" << node->GetVs() << " vd:" << node->GetVd() << " zero price:"
                      << node->GetZeroPrice() << std::endl;

            std::cout << "S function: \n";
            node->GetS().Print();
            std::cout << "D function: \n";
            node->GetD().Print();
            std::cout << "Zero price: \n";
            std::cout << node->GetZeroPrice() << std::endl;
        }
        std::cout << std::endl;
    }

    long double CalculateWelfare();

    void CheckFoundMarketParameters();

    void ClearMarketEdges();

    int64_t CompareWelrafeAndChangeLinesSubsetForChainLines(
            std::vector<std::shared_ptr<Edge>> l_plus_lines,
            std::vector<std::shared_ptr<Edge>> l_minus_lines,
            EdgeType edge_type);

    template<typename CompareWelrafe>
    int64_t CompareWelrafeAndChangeLinesSubset(EdgeType line_type,
            std::map<EdgeType, std::set<EdgeType>> other_types,
            AlgorithmType result_line_type,
            CompareWelrafe comp) {
        int64_t add_lines_count = 0;
        for (auto&& edge : GetEdges()) {
            if (edge->GetAlgorithmType() == AlgorithmType::L_undefined && edge->GetEdgeType() == line_type) {
                for (auto&& e : GetEdges()) {
                    if (e->GetAlgorithmType() == AlgorithmType::L_plus) {
                        e->SetLineExpand();
                    } else if (e->GetAlgorithmType() == AlgorithmType::L_undefined
                            && other_types[line_type].count(e->GetEdgeType()) && e != edge) {
                        e->SetLineExpand();
                    } else {
                        e->SetLineNotExpand();
                    }
                }

                SolveAuxiliarySubtask();
                auto welrafe_without_line = CalculateWelfare();

                edge->SetLineExpand();
                SolveAuxiliarySubtask();
                auto welrafe_with_line = CalculateWelfare();

                //std::cout << "Welrafe: " << welrafe_without_line << " " << welrafe_with_line << std::endl;

                if (comp(welrafe_without_line, welrafe_with_line)) {
                    edge->SetAlgorithmType(result_line_type);
                    add_lines_count++;
                }
            }
        }
        return add_lines_count;
    }

private:
    static constexpr long double kdMin = 10;
    static constexpr long double kdMax = 20;
    static constexpr long double kcMin = 10;
    static constexpr long double kcMax = 20;
    static PiecewiseLinearFunction CreateDeltaSForLine(std::shared_ptr<Node> node, std::shared_ptr<Edge> edge);

    void FindMarketParameters(std::shared_ptr<Edge> edge, size_t node_pos, std::vector<bool>& used,
            double lambda);

    void FindSDBalance(size_t node_pos, std::vector<bool>& used);

    bool FindTreeRootWithMinDepth(size_t node_pos, size_t to, int64_t path_length, std::vector<bool>& used);

    // Get by node unique id position in vector
    std::unordered_map<int64_t, int64_t> unique_id_to_vector_pos_;
    // All nodes in market
    std::vector<std::shared_ptr<Node>> nodes_;
    // Tree root position in vector
    int64_t tree_root_pos_ = -1;
    // adjacency matrix
    std::vector<std::vector<std::shared_ptr<Edge>>> matrix_;
    // Вершина являющейся центром в части которая отвечает за звезду
    std::shared_ptr<Node> central_market_node_;
    // Все ребра в маркете
    std::vector<std::shared_ptr<Edge>> edges_;
};
