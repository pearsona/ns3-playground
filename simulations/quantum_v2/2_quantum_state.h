#pragma once
#include "qpp/qpp.hpp"
#include <memory>
#include <vector>
#include <cmath>

using namespace qpp;

class QuantumState {
public:
    using Ptr = std::shared_ptr<QuantumState>;
    QuantumState(size_t num_qubits);
    QuantumState(qpp::ket state);

    cmat get_density_matrix() const;
    ket get_ket() const;

    void apply_gate(const cmat& U, const std::vector<idx>& targets);
    std::pair<idx, qpp::ket> measure(const idx& target);

    size_t num_qubits() const;

private:
    ket state_;

};


// ----------- Inline implementations ------------
inline QuantumState::QuantumState(size_t num_qubits) {
    state_ = qpp::mket(std::vector<idx>(num_qubits, 0)); // |00...0⟩
}

inline QuantumState::QuantumState(qpp::ket state) {
    state_ = state;
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

// Single qubit measurement
inline std::pair<idx, qpp::ket> QuantumState::measure(const idx& target) {
    auto [result, probs, states] = qpp::measure(state_, qpp::gt.Z, {target});

    return {result, states[result]};
}


inline size_t QuantumState::num_qubits() const {
    return static_cast<size_t>(std::log2(state_.rows()));
}