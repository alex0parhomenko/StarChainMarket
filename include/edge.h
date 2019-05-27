#pragma once
#include "piecewise_linear_function.h"
#include "node.h"
#include <memory>
#include <functional>



enum class EdgeType {
    STAR_TO_CENTER = 0,
    STAR_FROM_CENTER = 1,
    CHAIN_TO_CENTER = 2,
    CHAIN_FROM_CENTER = 3
};



enum class AlgorithmType {
    L_plus = 0,
    L_minus = 1,
    L_undefined = 2
};

class Edge {
public:
    Edge(double et, double Q, double ef, double Ev_coeff, std::shared_ptr<Node> from,
            std::shared_ptr<Node> to, EdgeType type);

    bool operator==(const Edge& other) const;
    std::shared_ptr<Node> GetStartNode() const;
    std::shared_ptr<Node> GetEndNode() const;
    std::shared_ptr<Node> GetAnotherNode(std::shared_ptr<Node> node);
    void SetDeltaSij(PiecewiseLinearFunction deltaSij);
    PiecewiseLinearFunction GetDeltaSij() const;
    long double GetEValueAtPoint(long double x);
    bool IsExpand();
    void SetLineExpand();
    void SetLineNotExpand();
    long double Getqij() const;
    void Setqij(long double qij);
    std::shared_ptr<Node> GetqijParentNode();
    void SetqijParentNode(std::shared_ptr<Node> parent_node) {
        parent_qij_node_ = parent_node;
    }

    long double GetEvCoeff() {
        return Ev_coeff_;
    }

    PiecewiseLinearFunction Getev() const;
    long double Getet() const;
    long double GetQ() const;
    EdgeType GetEdgeType() {
        return type_;
    }
    AlgorithmType GetAlgorithmType() {
        return algorithm_edge_type_;
    }

    void Print() {
        std::cout << "et: " << et_ << " Q: " << Q_ << " ef: " << ef_ << " Ev_coeff: " << Ev_coeff_ << std::endl;
        std::cout << "IsExpand: " << is_expand_;
        std::cout << " EdgeType: " << EdgeTypeText[static_cast<int>(type_)] << " Algorithm type: " <<
        AlgorithmTypeText[static_cast<int>(algorithm_edge_type_)] << std::endl;
    }

    void SetAlgorithmType(AlgorithmType type) {
        algorithm_edge_type_ = type;
    }

    static std::shared_ptr<Edge> GenerateRandomEdge(std::shared_ptr<Node> from, std::shared_ptr<Node> to, EdgeType type);
private:
    void CalcEFunction();
    void CalcevFunction();

    // удельные затраты на передачу товара
    long double et_;
    // Начальная пропускная способность
    long double Q_;
    // фиксированные затраты на увеличение пропускной способности
    long double ef_;
    // функция переменных затрах на увеличение пропускной способности имеет вид Ev_coeff * x^2
    long double Ev_coeff_;
    // Sij
    PiecewiseLinearFunction delta_S_ij_;
    // функция предельных транспортных затрат
    PiecewiseLinearFunction ev_;
    // Start node of edge
    std::shared_ptr<Node> from_;
    // Ends node of edge
    std::shared_ptr<Node> to_;
    // Parent node when calculating qij
    std::shared_ptr<Node> parent_qij_node_;
    // Is line expand
    bool is_expand_;
    // Full transportation costs
    std::function<double(double)> E_;
    // Поток через ребро
    long double qij_;
    // тип множества к которому принадлежит ребро, зависит от того находится оно в звезде или в цепочке и куда оно направлено
    EdgeType type_;
    // L+, L-
    AlgorithmType algorithm_edge_type_;

    std::vector<std::string> AlgorithmTypeText = {
            "L_plus",
            "L_minus",
            "L_undefined"
    };
    std::vector<std::string> EdgeTypeText = {
            "STAR_TO_CENTER",
            "STAR_FROM_CENTER",
            "CHAIN_TO_CENTER",
            "CHAIN_FROM_CENTER"
    };
};
