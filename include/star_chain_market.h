#pragma once

#include "node.h"
#include "edge.h"
#include <unordered_map>
#include <memory>

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

    std::vector<std::shared_ptr<Node>>& GetNodes() {
        return nodes_;
    }

    std::vector<std::shared_ptr<Edge>> GetEdges() {
        return edges_;
    }

    std::vector<std::vector<std::shared_ptr<Edge>>>& GetMatrix() {
        return matrix_;
    }

    std::shared_ptr<Node> GetCentralNode() const {
        return central_market_node_;
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

    void PrintNodes() {
        for (auto&& node : nodes_) {

            auto node_pos = GetVectorPosByNode(node);
            auto depth = node->GetDepth();
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
