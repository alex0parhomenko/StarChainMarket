
#include "star_chain_market.h"
#include <iostream>
#include <fstream>

void Algorithm(StarChainMarket& market) {
    auto changed_lines = 0;

    while (market.UndefinedLinesCount() && changed_lines > 0) {
        changed_lines = 0;
        auto step1_changed_lines = 0;
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
    }
}

void StoreGeneratedMarket(char** argv) {
    std::ofstream f(argv[1], std::ios_base::out);
    auto market = StarChainMarket::GenerateRandomMarket(2, 3, 4);
    StarChainMarket::StoreMarket(f, *market);
    f.close();
}

inline auto LoadMarket(char** argv) {
    std::ifstream f(argv[1], std::ios_base::in);
    auto market = StarChainMarket::LoadMarket(f);
    f.close();
    market.PrintAll();
    return market;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        throw std::runtime_error("Too few arguments");
    }
    auto market = LoadMarket(argv);
    Algorithm(market);
    return 0;
}
