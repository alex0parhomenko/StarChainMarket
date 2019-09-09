#pragma once

#include "algorithm.h"
#include <unordered_map>

inline auto Experiment(
        int64_t import_from_center_nodes_count,
        int64_t export_to_center_node_nodes_count,
        int64_t chain_nodes_count) {
    auto market = StarChainMarket::GenerateRandomMarket(import_from_center_nodes_count,
            export_to_center_node_nodes_count, chain_nodes_count).value();
    market.BuildTreeMinDepth();

    auto edges = market.GetEdges();
    auto bit_masks_map = GenerateBitMasks(edges.size());
    std::unordered_map<int64_t, float> mask_to_welrafe;
    float best_brute_force_welrafe = -1e5;
    int64_t best_mask = 0;
    for (auto&& [bits_count, masks] : bit_masks_map) {
        for (auto&& mask : masks) {
            market.ClearMarketEdgesAlgorithmType();
            for (size_t i = 0; i < edges.size(); i++) {
                if ((mask >> i) & 1) {
                    edges[i]->SetAlgorithmType(AlgorithmType::L_plus);
                    edges[i]->SetLineExpand();
                }
            }
            market.SolveAuxiliarySubtask();
            const auto welrafe = market.CalculateWelfare();
            if (welrafe < 0 ) {
                market.PrintAll();
                exit(1);
            }
            mask_to_welrafe[mask] = welrafe;
            //std::cout << "Welrafe: " << mask_to_welrafe[mask] << std::endl;
            if (welrafe > best_brute_force_welrafe) {
                best_mask = mask;
                best_brute_force_welrafe = welrafe;
            }
        }
    }
    market.ClearMarketEdgesAlgorithmType();
    Algorithm(market);
    market.SolveAuxiliarySubtask();
    float algorithm_welrafe = market.CalculateWelfare();
    std::cout << "algorithm l plus mask: ";
    PrintVector(market.GetLplushLinesMask());
    std::cout << std::endl;
    std::cout << "brute force best mask: ";
    PrintVector(MaskToVector(best_mask, market.GetEdges().size()));
    std::cout << std::endl;
    return std::make_pair(best_brute_force_welrafe, algorithm_welrafe);
}
