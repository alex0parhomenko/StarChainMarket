#pragma once

#include "star_chain_market.h"

inline int AlgorithmProcessStarEdges(StarChainMarket& market) {
    auto step1_changed_lines = 0;
    auto changed_lines = 0;
    do {
        step1_changed_lines = 0;
        // AddLinesFromL1ToLplus
        step1_changed_lines += market.CompareWelrafeAndChangeLinesSubset(EdgeType::STAR_TO_CENTER,
                ConcurentTypes, AlgorithmType::L_plus, std::less_equal<>());
        //AddLinesFromL2ToLminus
        step1_changed_lines += market.CompareWelrafeAndChangeLinesSubset(EdgeType::STAR_FROM_CENTER,
                AdditionalTypes, AlgorithmType::L_minus, std::greater_equal<>());
        //AddLinesFromL1ToLminus
        step1_changed_lines += market.CompareWelrafeAndChangeLinesSubset(EdgeType::STAR_TO_CENTER,
                AdditionalTypes, AlgorithmType::L_minus, std::greater_equal<>());
        //AddLinesFromL2ToLplus
        step1_changed_lines += market.CompareWelrafeAndChangeLinesSubset(EdgeType::STAR_FROM_CENTER,
                ConcurentTypes, AlgorithmType::L_plus, std::less_equal<>());
        changed_lines += step1_changed_lines;
    }
    while (market.UndefinedLinesCount() && step1_changed_lines > 0);
    return changed_lines;
}


inline int AlgorithmProcessChainEdges(StarChainMarket& market) {
    auto changed_lines = 0;
    auto step2_changed_lines = 0;
    do {
        step2_changed_lines = 0;
        // AddLinesFromL'1ToLplus
        step2_changed_lines += market.CompareWelrafeAndChangeLinesSubset(EdgeType::CHAIN_TO_CENTER,
                ConcurentTypes,
                AlgorithmType::L_plus, std::less_equal<>());
        //AddLinesFromL'2ToLminus
        step2_changed_lines += market.CompareWelrafeAndChangeLinesSubset(EdgeType::CHAIN_FROM_CENTER,
                AdditionalTypes,
                AlgorithmType::L_minus, std::greater_equal<>());
        //AddLinesFromL'1ToLminus
        step2_changed_lines += market.CompareWelrafeAndChangeLinesSubset(EdgeType::CHAIN_TO_CENTER,
                AdditionalTypes, AlgorithmType::L_minus, std::greater_equal<>());
        //AddLinesFromL'2ToLplus
        step2_changed_lines += market.CompareWelrafeAndChangeLinesSubset(EdgeType::CHAIN_FROM_CENTER,
                ConcurentTypes, AlgorithmType::L_plus, std::less_equal<>());
        changed_lines += step2_changed_lines;
    }
    while (market.UndefinedLinesCount() > 0 && step2_changed_lines > 0);
    return changed_lines;
}

inline void Algorithm(StarChainMarket& market) {
    auto multiply_lines_in_chain_changed = 0;
    do {
        auto changed_lines = 0;
        do {
            changed_lines = 0;
            changed_lines += AlgorithmProcessStarEdges(market);
            changed_lines += AlgorithmProcessChainEdges(market);
        } while (market.UndefinedLinesCount() && changed_lines > 0);

        multiply_lines_in_chain_changed = 0;
        const auto undefined_chain_lines_count = market.GetUndefinedChainLinesCount();
        if (undefined_chain_lines_count < 2) {
            break;
        }
        auto bit_masks_map = GenerateBitMasks(undefined_chain_lines_count);
        auto undefined_chain_lines = market.GetUndefinedChainEdges();
        for (size_t i = 2; i <= undefined_chain_lines_count && multiply_lines_in_chain_changed == 0; i++) {
            auto masks = bit_masks_map[i];
            for (auto&& mask : masks) {
                std::vector<std::shared_ptr<Edge>> lines_to_change_chain_to_center;
                std::vector<std::shared_ptr<Edge>> lines_to_change_chain_from_center;
                for (size_t j = 0; j < undefined_chain_lines.size(); j++) {
                    if ((mask >> j) & 1) {
                        auto curr_edge = undefined_chain_lines[i];
                        if (curr_edge->GetEdgeType() == EdgeType::CHAIN_TO_CENTER) {
                            lines_to_change_chain_to_center.emplace_back(curr_edge);
                        } else if (curr_edge->GetEdgeType() == EdgeType::CHAIN_FROM_CENTER) {
                            lines_to_change_chain_from_center.emplace_back(curr_edge);
                        } else {
                            throw std::runtime_error("Unsupported type");
                        }
                    }
                }
                multiply_lines_in_chain_changed +=
                        market.CompareWelrafeAndChangeLinesSubsetForChainLines(lines_to_change_chain_to_center,
                                lines_to_change_chain_from_center, EdgeType::CHAIN_FROM_CENTER);
                if (multiply_lines_in_chain_changed) {
                    break;
                }
                multiply_lines_in_chain_changed +=
                        market.CompareWelrafeAndChangeLinesSubsetForChainLines(lines_to_change_chain_from_center,
                                lines_to_change_chain_to_center, EdgeType::CHAIN_FROM_CENTER);
                if (multiply_lines_in_chain_changed) {
                    break;
                }
            }
        }
    } while (market.UndefinedLinesCount() && multiply_lines_in_chain_changed > 0);
}
