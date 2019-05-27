#include "linear_function.h"
#include "piecewise_linear_function.h"
#include "helpers.h"
#include "star_chain_market.h"
#include <gtest/gtest.h>
#include <cmath>

TEST(star_chain_market, build_tree_min_depth) {
    auto import_nodes = 20, export_nodex = 20, chain_nodes = 10;
    for (int iter = 0; iter < 50; iter++) {
        auto market_opt = StarChainMarket::GenerateRandomMarket(import_nodes, export_nodex, chain_nodes);
        while (!market_opt.has_value()) {
            market_opt = StarChainMarket::GenerateRandomMarket(import_nodes, export_nodex, chain_nodes);
        }
        auto market = market_opt.value();
        market.BuildTreeMinDepth();
        auto root_node_pos = market.GetRootNodePos();
        auto nodes = market.GetNodes();
        std::vector<bool> used(nodes.size(), false);
        market.Dfs(root_node_pos, used);
        std::shared_ptr<Node> max_node, min_node;
        for (auto&& node : nodes) {
            auto depth = node->GetDepth();
            if (node->IsLeaf()) {
                if (!max_node || depth > max_node->GetDepth()) {
                    max_node = node;
                }
                if (!min_node || depth < min_node->GetDepth()) {
                    min_node = node;
                }
            }
        }
        EXPECT_TRUE(std::fabs(max_node->GetDepth() - min_node->GetDepth()) <= 1);
        EXPECT_TRUE(market.GetMatrix()[root_node_pos].size() >= 2);
    }
}

void PrintF(LinearFunctionDefineOnSegment f) {
    std::cout << "x start: " << f.GetXStartCoordinate() << " xend: " << f.GetXEndCoordinate() << std::endl;
    if (f.IsVertical()) {
        std::cout << "Vertical Segment: " << f.GetVerticalSegment().GetStart() << f.GetVerticalSegment().GetEnd() << std::endl;
    } else {
        std::cout << "A coeff: " << f.GetLinearFunction().GetACoeff() << std::endl;
        std::cout << "B coeff: " << f.GetLinearFunction().GetBCoeff() << std::endl;
        std::cout << "C coeff: " << f.GetLinearFunction().GetCCoeff() << std::endl;
    }
}

TEST(star_chain_market, auxularity_subtask) {
    /*
     * Node 0
     */
    std::vector<LinearFunctionDefineOnSegment> D0_funcs;
    D0_funcs.emplace_back(LinearFunction(1, 1, 6), 0, 6);
    D0_funcs.emplace_back(LinearFunction(0, 1, 0), 6, 20);
    PiecewiseLinearFunction D0(D0_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S0_funcs;
    S0_funcs.emplace_back(LinearFunction(1, -1, 0), 0, 6);
    S0_funcs.emplace_back(LinearFunction(-2, 1, -6), 6, 20);
    PiecewiseLinearFunction S0(S0_funcs, Segment(0, 20));

    auto node0 = std::make_shared<Node>(D0, S0);
    auto delta_S0_funcs = node0->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S0_funcs.size(), 2);
    EXPECT_EQ(delta_S0_funcs[0], LinearFunctionDefineOnSegment(LinearFunction(-2, 1, -6), 0, 6));
    EXPECT_EQ(delta_S0_funcs[1], LinearFunctionDefineOnSegment(LinearFunction(-2, 1, -6), 6, 20));

    /*
     * Node1
     */
    std::vector<LinearFunctionDefineOnSegment> D1_funcs;
    D1_funcs.emplace_back(LinearFunction(2, 1, 4), 0, 2);
    D1_funcs.emplace_back(LinearFunction(0, 1, 0), 2, 20);
    PiecewiseLinearFunction D1(D1_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S1_funcs;
    S1_funcs.emplace_back(LinearFunction(-2, 1, 0), 0, 2);
    S1_funcs.emplace_back(LinearFunction(-4, 1, -4), 2, 20);
    PiecewiseLinearFunction S1(S1_funcs, Segment(0, 20));

    auto node1 = std::make_shared<Node>(D1, S1);
    auto delta_S1_funcs = node1->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S1_funcs.size(), 2);
    EXPECT_EQ(delta_S1_funcs[0], LinearFunctionDefineOnSegment(LinearFunction(-4, 1, -4), 0, 2));
    EXPECT_EQ(delta_S1_funcs[1], LinearFunctionDefineOnSegment(LinearFunction(-4, 1, -4), 2, 20));

    /*
     * Node 2
     */
    std::vector<LinearFunctionDefineOnSegment> D2_funcs;
    D2_funcs.emplace_back(LinearFunction(1, 1, 8), 0, 8);
    D2_funcs.emplace_back(LinearFunction(0, 1, 0), 8, 20);
    PiecewiseLinearFunction D2(D2_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S2_funcs;
    S2_funcs.emplace_back(LinearFunction(-1, 1, 0), 0, 8);
    S2_funcs.emplace_back(LinearFunction(-2, 1, -8), 8, 20);
    PiecewiseLinearFunction S2(S2_funcs, Segment(0, 20));

    auto node2 = std::make_shared<Node>(D2, S2);
    auto delta_S2_funcs = node2->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S2_funcs.size(), 2);
    EXPECT_EQ(delta_S2_funcs[0], LinearFunctionDefineOnSegment(LinearFunction(-2, 1, -8), 0, 8));
    EXPECT_EQ(delta_S2_funcs[1], LinearFunctionDefineOnSegment(LinearFunction(-2, 1, -8), 8, 20));
    /*
     * Node 3
     */
    std::vector<LinearFunctionDefineOnSegment> D3_funcs;
    D3_funcs.emplace_back(LinearFunction(2, 1, 6), 0, 3);
    D3_funcs.emplace_back(LinearFunction(0, 1, 0), 3, 20);
    PiecewiseLinearFunction D3(D3_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S3_funcs;
    S3_funcs.emplace_back(LinearFunction(-2, 1, 0), 0, 3);
    S3_funcs.emplace_back(LinearFunction(-4, 1, -6), 3, 20);
    PiecewiseLinearFunction S3(S3_funcs, Segment(0, 20));

    auto node3 = std::make_shared<Node>(D3, S3);
    auto delta_S3_funcs = node3->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S3_funcs.size(), 2);
    EXPECT_EQ(delta_S3_funcs[0], LinearFunctionDefineOnSegment(LinearFunction(-4, 1, -6), 0, 3));
    EXPECT_EQ(delta_S3_funcs[1], LinearFunctionDefineOnSegment(LinearFunction(-4, 1, -6), 3, 20));

    StarChainMarket market;
    market.AddNode(node0, true);
    market.AddNode(node1);
    market.AddNode(node2);
    market.AddNode(node3);

    auto edge10 = std::make_shared<Edge>(1, 2, 2, 2, node1, node0, EdgeType::STAR_TO_CENTER);
    auto edge20 = std::make_shared<Edge>(2, 3, 1, 2, node2, node0, EdgeType::STAR_TO_CENTER);
    auto edge30 = std::make_shared<Edge>(3, 3, 1, 1, node0, node3, EdgeType::STAR_FROM_CENTER);
    market.AddEdge(edge10);
    market.AddEdge(edge20);
    market.AddEdge(edge30);

    market.BuildTreeMinDepth();
    EXPECT_EQ(market.GetRootNodePos(), 0);

    market.SolveAuxiliarySubtask();
    market.CheckFoundMarketParameters();
    auto welfare = market.CalculateWelfare();

    EXPECT_DOUBLE_EQ(node0->GetP(), 2.3333333333333335);
    EXPECT_DOUBLE_EQ(node0->GetVs(), 2.3333333333333335);
    EXPECT_DOUBLE_EQ(node0->GetVd(), 3.66666666666666666);

    EXPECT_DOUBLE_EQ(node1->GetP(), 1.3333333333333333);
    EXPECT_DOUBLE_EQ(node1->GetVs(), 2.6666666666666665);
    EXPECT_DOUBLE_EQ(node1->GetVd(), 1.3333333333333333);

    EXPECT_DOUBLE_EQ(node2->GetP(), 4);
    EXPECT_DOUBLE_EQ(node2->GetVs(), 4);
    EXPECT_DOUBLE_EQ(node2->GetVd(), 4);

    EXPECT_DOUBLE_EQ(node3->GetP(), 1.5);
    EXPECT_DOUBLE_EQ(node3->GetVs(), 3);
    EXPECT_DOUBLE_EQ(node3->GetVd(), 3);

    EXPECT_DOUBLE_EQ(edge10->Getqij(), 1.3333333333333333);
    EXPECT_DOUBLE_EQ(edge20->Getqij(), 0);
    EXPECT_DOUBLE_EQ(edge30->Getqij(), 0);

    EXPECT_DOUBLE_EQ(welfare, 32.166666666666664);
}

TEST(star_chain_market, auxularity_subtask2) {
    /*
     * Node 0
     */
    std::vector<LinearFunctionDefineOnSegment> D0_funcs;
    D0_funcs.emplace_back(LinearFunction(2, 1, 8), 0, 4);
    D0_funcs.emplace_back(LinearFunction(0, 1, 0), 4, 20);
    PiecewiseLinearFunction D0(D0_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S0_funcs;
    S0_funcs.emplace_back(LinearFunction(-2, 1, 0), 0, 4);
    S0_funcs.emplace_back(LinearFunction(-4, 1, -8), 4, 20);
    PiecewiseLinearFunction S0(S0_funcs, Segment(0, 20));

    auto node0 = std::make_shared<Node>(D0, S0);
    auto delta_S0_funcs = node0->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S0_funcs.size(), 2);
    EXPECT_EQ(delta_S0_funcs[0], LinearFunctionDefineOnSegment(LinearFunction(-4, 1, -8), 0, 4));
    EXPECT_EQ(delta_S0_funcs[1], LinearFunctionDefineOnSegment(LinearFunction(-4, 1, -8), 4, 20));

    /*
     * Node1
     */
    std::vector<LinearFunctionDefineOnSegment> D1_funcs;
    D1_funcs.emplace_back(LinearFunction(1, 1, 4), 0, 4);
    D1_funcs.emplace_back(LinearFunction(0, 1, 0), 4, 20);
    PiecewiseLinearFunction D1(D1_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S1_funcs;
    S1_funcs.emplace_back(LinearFunction(-1, 1, 0), 0, 4);
    S1_funcs.emplace_back(LinearFunction(-2, 1, -4), 4, 20);
    PiecewiseLinearFunction S1(S1_funcs, Segment(0, 20));

    auto node1 = std::make_shared<Node>(D1, S1);
    auto delta_S1_funcs = node1->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S1_funcs.size(), 2);
    EXPECT_EQ(delta_S1_funcs[0], LinearFunctionDefineOnSegment(LinearFunction(-2, 1, -4), 0, 4));
    EXPECT_EQ(delta_S1_funcs[1], LinearFunctionDefineOnSegment(LinearFunction(-2, 1, -4), 4, 20));

    /*
     * Node 2
     */
    std::vector<LinearFunctionDefineOnSegment> D2_funcs;
    D2_funcs.emplace_back(LinearFunction(1, 1, 6), 0, 6);
    D2_funcs.emplace_back(LinearFunction(0, 1, 0), 6, 20);
    PiecewiseLinearFunction D2(D2_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S2_funcs;
    S2_funcs.emplace_back(LinearFunction(-1, 1, 0), 0, 6);
    S2_funcs.emplace_back(LinearFunction(-2, 1, -6), 6, 20);
    PiecewiseLinearFunction S2(S2_funcs, Segment(0, 20));

    auto node2 = std::make_shared<Node>(D2, S2);
    auto delta_S2_funcs = node2->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S2_funcs.size(), 2);
    EXPECT_EQ(delta_S2_funcs[0], LinearFunctionDefineOnSegment(LinearFunction(-2, 1, -6), 0, 6));
    EXPECT_EQ(delta_S2_funcs[1], LinearFunctionDefineOnSegment(LinearFunction(-2, 1, -6), 6, 20));

    /*
     * Node 3
     */
    std::vector<LinearFunctionDefineOnSegment> D3_funcs;
    D3_funcs.emplace_back(LinearFunction(0.5, 1, 4), 0, 8);
    D3_funcs.emplace_back(LinearFunction(0, 1, 0), 8, 20);
    PiecewiseLinearFunction D3(D3_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S3_funcs;
    S3_funcs.emplace_back(LinearFunction(-0.5, 1, 0), 0, 8);
    S3_funcs.emplace_back(LinearFunction(-1, 1, -4), 8, 20);
    PiecewiseLinearFunction S3(S3_funcs, Segment(0, 20));

    auto node3 = std::make_shared<Node>(D3, S3);
    auto delta_S3_funcs = node3->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S3_funcs.size(), 2);
    EXPECT_EQ(delta_S3_funcs[0], LinearFunctionDefineOnSegment(LinearFunction(-1, 1, -4), 0, 8));
    EXPECT_EQ(delta_S3_funcs[1], LinearFunctionDefineOnSegment(LinearFunction(-1, 1, -4), 8, 20));

    StarChainMarket market;
    market.AddNode(node0, true);
    market.AddNode(node1);
    market.AddNode(node2);
    market.AddNode(node3);

    /*
     * Edge(double et, double Q, double ef, double Ev_coeff, std::shared_ptr<Node> from,
            std::shared_ptr<Node> to, EdgeType type);
     */
    auto edge10 = std::make_shared<Edge>(3, 2, 1, 1, node0, node1, EdgeType::STAR_FROM_CENTER);
    auto edge20 = std::make_shared<Edge>(2, 2, 1, 1, node2, node0, EdgeType::STAR_TO_CENTER);
    auto edge30 = std::make_shared<Edge>(2, 3, 1, 1, node0, node3, EdgeType::STAR_FROM_CENTER);

    market.AddEdge(edge10);
    market.AddEdge(edge20);
    market.AddEdge(edge30);

    market.BuildTreeMinDepth();
    EXPECT_EQ(market.GetRootNodePos(), 0);

    market.SolveAuxiliarySubtask();
    market.CheckFoundMarketParameters();

}

TEST(star_chain_market, auxularity_subtask3) {
    /*
     * Node 0
     */
    std::vector<LinearFunctionDefineOnSegment> D0_funcs;
    D0_funcs.emplace_back(Point(0, 9), Point(6, 0));
    D0_funcs.emplace_back(Point(6, 0), Point(16, 0));
    PiecewiseLinearFunction D0(D0_funcs, Segment(0, 16));

    std::vector<LinearFunctionDefineOnSegment> S0_funcs;
    S0_funcs.emplace_back(Point(0, 0), Point(6, 9));
    S0_funcs.emplace_back(Point(6, 9), Point(16, 39));
    PiecewiseLinearFunction S0(S0_funcs, Segment(0, 16));

    auto node0 = std::make_shared<Node>(D0, S0);
    auto delta_S0_funcs = node0->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S0_funcs.size(), 2);

    EXPECT_EQ(delta_S0_funcs[0], LinearFunctionDefineOnSegment(Point(0, -9), Point(6, 9)));
    EXPECT_EQ(delta_S0_funcs[1], LinearFunctionDefineOnSegment(Point(6, 9), Point(16, 39)));

    /*
     * Node1
     */
    std::vector<LinearFunctionDefineOnSegment> D1_funcs;
    D1_funcs.emplace_back(Point(0, 15), Point(10, 0));
    D1_funcs.emplace_back(Point(10, 0), Point(20, 0));
    PiecewiseLinearFunction D1(D1_funcs, Segment(0, 20));

    std::vector<LinearFunctionDefineOnSegment> S1_funcs;
    S1_funcs.emplace_back(Point(0, 0), Point(10, 15));
    S1_funcs.emplace_back(Point(10, 15), Point(20, 45));
    PiecewiseLinearFunction S1(S1_funcs, Segment(0, 20));

    auto node1 = std::make_shared<Node>(D1, S1);
    auto delta_S1_funcs = node1->GetDeltaS().GetFunctions();

    EXPECT_EQ(delta_S1_funcs.size(), 2);
    EXPECT_EQ(delta_S1_funcs[0], LinearFunctionDefineOnSegment(Point(0, -15), Point(10, 15)));
    EXPECT_EQ(delta_S1_funcs[1], LinearFunctionDefineOnSegment(Point(10, 15), Point(20, 45)));

    /*
     * Node 2
     */
    std::vector<LinearFunctionDefineOnSegment> D2_funcs;
    D2_funcs.emplace_back(Point(0, 2), Point(4, 0));
    D2_funcs.emplace_back(Point(4, 0), Point(14, 0));
    PiecewiseLinearFunction D2(D2_funcs, Segment(0, 14));

    std::vector<LinearFunctionDefineOnSegment> S2_funcs;
    S2_funcs.emplace_back(Point(0, 0), Point(4, 2));
    S2_funcs.emplace_back(Point(4, 2), Point(14, 12));
    PiecewiseLinearFunction S2(S2_funcs, Segment(0, 14));

    auto node2 = std::make_shared<Node>(D2, S2);
    auto delta_S2_funcs = node2->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S2_funcs.size(), 2);
    EXPECT_EQ(delta_S2_funcs[0], LinearFunctionDefineOnSegment(Point(0, -2), Point(4, 2)));
    EXPECT_EQ(delta_S2_funcs[1], LinearFunctionDefineOnSegment(Point(4, 2), Point(14, 12)));

    /*
     * Node 3
     */
    std::vector<LinearFunctionDefineOnSegment> D3_funcs;
    D3_funcs.emplace_back(Point(0, 8), Point(4, 0));
    D3_funcs.emplace_back(Point(4, 0), Point(14, 0));
    PiecewiseLinearFunction D3(D3_funcs, Segment(0, 14));

    std::vector<LinearFunctionDefineOnSegment> S3_funcs;
    S3_funcs.emplace_back(Point(0, 0), Point(4, 8));
    S3_funcs.emplace_back(Point(4, 8), Point(14, 48));
    PiecewiseLinearFunction S3(S3_funcs, Segment(0, 14));

    auto node3 = std::make_shared<Node>(D3, S3);
    auto delta_S3_funcs = node3->GetDeltaS().GetFunctions();
    EXPECT_EQ(delta_S3_funcs.size(), 2);
    EXPECT_EQ(delta_S3_funcs[0], LinearFunctionDefineOnSegment(Point(0, -8), Point(4, 8)));
    EXPECT_EQ(delta_S3_funcs[1], LinearFunctionDefineOnSegment(Point(4, 8), Point(14, 48)));


    StarChainMarket market;
    market.AddNode(node0, true);
    market.AddNode(node1);
    market.AddNode(node2);
    market.AddNode(node3);

    /*
     * Edge(double et, double Q, double ef, double Ev_coeff, std::shared_ptr<Node> from,
            std::shared_ptr<Node> to, EdgeType type);
     */
    auto edge10 = std::make_shared<Edge>(1, 1, 4, 4, node0, node1, EdgeType::STAR_FROM_CENTER);
    auto edge20 = std::make_shared<Edge>(3, 5, 1, 2, node2, node0, EdgeType::STAR_TO_CENTER);
    auto edge30 = std::make_shared<Edge>(3, 2, 1, 3, node3, node0, EdgeType::STAR_TO_CENTER);

    market.AddEdge(edge10);
    market.AddEdge(edge20);
    market.AddEdge(edge30);

    market.BuildTreeMinDepth();
    EXPECT_EQ(market.GetRootNodePos(), 0);

    market.SolveAuxiliarySubtask();
    market.CheckFoundMarketParameters();

    market.PrintAll();
}
