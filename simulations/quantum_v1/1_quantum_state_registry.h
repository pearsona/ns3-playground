#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include "1_quantum_state.h"

class Qubit;

class QuantumStateRegistry {
public:
    static QuantumStateRegistry& instance();

    void register_qubit(const std::shared_ptr<Qubit>& q);
    void unregister_qubit(const std::shared_ptr<Qubit>& q);

    std::vector<std::shared_ptr<Qubit>> get_qubits(std::shared_ptr<QuantumState> state);

private:
    std::unordered_map<std::shared_ptr<QuantumState>, std::vector<std::weak_ptr<Qubit>>> state_to_qubits_;
};
