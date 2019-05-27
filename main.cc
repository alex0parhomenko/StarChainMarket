#include "piecewise_linear_function.h"
#include "linear_function.h"
#include "star_chain_market.h"
#include <iostream>

void ClearMarketEdges(StarChainMarket& market) {
    for (auto&& edge : market.GetEdges()) {
        edge->SetLineNotExpand();
    }
}

int64_t AddLinesFromL1ToLplus(StarChainMarket& market) {
    int64_t add_lines_count = 0;
    for (auto&& edge : market.GetEdges()) {
        ClearMarketEdges(market);

        if (edge->GetAlgorithmType() == AlgorithmType::L_undefined && edge->GetEdgeType() == EdgeType::STAR_TO_CENTER) {
            for (auto&& e : market.GetEdges()) {
                if (e->GetAlgorithmType() == AlgorithmType::L_plus) {
                    e->SetLineExpand();
                }
                if (e->GetAlgorithmType() == AlgorithmType::L_undefined && (e->GetEdgeType() == EdgeType::STAR_TO_CENTER ||
                        e->GetEdgeType() == EdgeType::CHAIN_TO_CENTER) && e != edge) {
                    e->SetLineExpand();
                }
            }

            market.SolveAuxiliarySubtask();
            auto welrafe_without_line = market.CalculateWelfare();

            edge->SetLineExpand();
            market.SolveAuxiliarySubtask();
            auto welrafe_with_line = market.CalculateWelfare();

            if (welrafe_without_line < welrafe_with_line) {
                edge->SetAlgorithmType(AlgorithmType::L_plus);
                add_lines_count++;
            }
        }
    }
    return add_lines_count;
}

int64_t AddLinesFromL2ToLminus(StarChainMarket& market) {
    int64_t add_lines_count = 0;
    for (auto&& edge : market.GetEdges()) {
        ClearMarketEdges(market);

        if (edge->GetAlgorithmType() == AlgorithmType::L_undefined && edge->GetEdgeType() == EdgeType::STAR_FROM_CENTER) {
            for (auto&& e : market.GetEdges()) {
                if (e->GetAlgorithmType() == AlgorithmType::L_plus) {
                    e->SetLineExpand();
                }
                if (e->GetAlgorithmType() == AlgorithmType::L_undefined && (e->GetEdgeType() == EdgeType::STAR_TO_CENTER ||
                        e->GetEdgeType() == EdgeType::CHAIN_TO_CENTER) && e != edge) {
                    e->SetLineExpand();
                }
            }

            market.SolveAuxiliarySubtask();
            auto welrafe_without_line = market.CalculateWelfare();

            edge->SetLineExpand();
            market.SolveAuxiliarySubtask();
            auto welrafe_with_line = market.CalculateWelfare();

            if (welrafe_with_line < welrafe_without_line) {
                edge->SetAlgorithmType(AlgorithmType::L_minus);
                add_lines_count++;
            }
        }
    }
    return add_lines_count;
}


int64_t AddLinesFromL1ToLminus(StarChainMarket& market) {
    int64_t add_lines_count = 0;
    for (auto&& edge : market.GetEdges()) {
        ClearMarketEdges(market);

        if (edge->GetAlgorithmType() == AlgorithmType::L_undefined
                && edge->GetEdgeType() == EdgeType::STAR_TO_CENTER) {
            for (auto&& e : market.GetEdges()) {
                if (e->GetAlgorithmType() == AlgorithmType::L_plus) {
                    e->SetLineExpand();
                }
                if (e->GetAlgorithmType() == AlgorithmType::L_undefined
                        && (e->GetEdgeType() == EdgeType::STAR_FROM_CENTER ||
                                e->GetEdgeType() == EdgeType::CHAIN_FROM_CENTER) && e != edge) {
                    e->SetLineExpand();
                }
            }

            market.SolveAuxiliarySubtask();
            auto welrafe_without_line = market.CalculateWelfare();

            edge->SetLineExpand();
            market.SolveAuxiliarySubtask();
            auto welrafe_with_line = market.CalculateWelfare();

            if (welrafe_with_line < welrafe_without_line) {
                edge->SetAlgorithmType(AlgorithmType::L_minus);
                add_lines_count++;
            }
        }
    }
    return add_lines_count;
}

int64_t AddLinesFromL2ToLplus(StarChainMarket& market) {
    int64_t add_lines_count = 0;
    for (auto&& edge : market.GetEdges()) {
        ClearMarketEdges(market);

        if (edge->GetAlgorithmType() == AlgorithmType::L_undefined && edge->GetEdgeType() == EdgeType::STAR_FROM_CENTER) {
            for (auto&& e : market.GetEdges()) {
                if (e->GetAlgorithmType() == AlgorithmType::L_plus) {
                    e->SetLineExpand();
                }
                if (e->GetAlgorithmType() == AlgorithmType::L_undefined && (e->GetEdgeType() == EdgeType::STAR_FROM_CENTER ||
                        e->GetEdgeType() == EdgeType::CHAIN_FROM_CENTER) && e != edge) {
                    e->SetLineExpand();
                }
            }

            market.SolveAuxiliarySubtask();
            auto welrafe_without_line = market.CalculateWelfare();

            edge->SetLineExpand();
            market.SolveAuxiliarySubtask();
            auto welrafe_with_line = market.CalculateWelfare();

            if ( welrafe_without_line < welrafe_with_line) {
                edge->SetAlgorithmType(AlgorithmType::L_plus);
                add_lines_count++;
            }
        }
    }
    return add_lines_count;
}

void Algorithm(StarChainMarket& market) {
    auto it = 0;

    auto add_lines = AddLinesFromL1ToLplus(market);
    auto remove_lines = AddLinesFromL2ToLminus(market);

    if (market.UndefinedLinesCount() != 0) {
        AddLinesFromL1ToLminus(market);
        AddLinesFromL2ToLplus(market);
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    for (int nodes_count = 5; nodes_count < 6; nodes_count++) {
        auto import_nodes = nodes_count;
        auto export_nodes = nodes_count;
        auto chain_nodes = nodes_count;
        auto market_opt = StarChainMarket::GenerateRandomMarket(import_nodes, export_nodes, chain_nodes);
        while (!market_opt.has_value()) {
            market_opt = StarChainMarket::GenerateRandomMarket(import_nodes, export_nodes, chain_nodes);
        }
        auto market = market_opt.value();

        market.BuildTreeMinDepth();
        market.SolveAuxiliarySubtask();

        market.PrintAll();
        //Algorithm(market);

        //for (auto&& edge : market.GetEdges()) {
        //    edge->Print();
        //}
    }

    return 0;
}
