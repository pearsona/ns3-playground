#pragma once
#include "qpp/qpp.hpp"
#include <memory>
#include <vector>
#include <cmath>

using qpp::cmat;
using qpp::idx;
using qpp::ket;

class QuantumState {
public:
    using Ptr = std::shared_ptr<QuantumState>;
    QuantumState(size_t num_qubits);

    cmat get_density_matrix() const;
    ket get_ket() const;

    void apply_gate(const cmat& U, const std::vector<idx>& targets);
    std::pair<idx, std::vector<double>> measure(const std::vector<idx>& targets);

    size_t num_qubits() const;

private:
    ket state_;
};


// ----------- Inline implementations ------------
inline QuantumState::QuantumState(size_t num_qubits) {
    state_ = qpp::mket(std::vector<idx>(num_qubits, 0)); // |00...0⟩
}

inline cmat QuantumState::get_density_matrix() const {
    return qpp::prj(state_);
}

inline ket QuantumState::get_ket() const {
    return state_;
}

inline void QuantumState::apply_gate(const cmat& U, const std::vector<idx>& targets) {
    state_ = qpp::apply(state_, U, targets);
}

inline std::pair<idx, std::vector<double>> QuantumState::measure(const std::vector<idx>& targets) {
    auto [result, probs, states] = qpp::measure(state_, qpp::gt.Z, targets);
    state_ = states[result];
    return {result, probs};
}

inline size_t QuantumState::num_qubits() const {
    return static_cast<size_t>(std::log2(state_.rows()));
}