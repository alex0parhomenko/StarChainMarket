#include "algorithm.h"
#include "star_chain_market.h"
#include "experiment.h"
#include <iostream>
#include <fstream>

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
    return market;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    const auto max_lines = 16;
    for (int i = 1; i < 8; i++) {
        for (int j = 1; j < 8; j++) {
            auto&& [brute_force_welrafe, algorithm_welrafe] = Experiment(i, j, max_lines - i - j);
            if (algorithm_welrafe >= brute_force_welrafe) {
                std::cout << "Success" << std::flush << std::endl;
            } else {
                std::cout << "Fail" << std::flush << std::endl;
            }
        }
    }

    return 0;
}
