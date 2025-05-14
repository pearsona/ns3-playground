#include "1_quantum_state_registry.h"
#include "1_qubit.h"
#include <algorithm>

QuantumStateRegistry& QuantumStateRegistry::instance() {
    static QuantumStateRegistry registry;
    return registry;
}

void QuantumStateRegistry::register_qubit(const std::shared_ptr<Qubit>& q) {
    auto& vec = state_to_qubits_[q->state()];
    for (const auto& wq : vec) {
        if (auto sq = wq.lock(); sq == q) {
            return; // already registered
        }
    }

    vec.push_back(q);
}



void QuantumStateRegistry::unregister_qubit(const std::shared_ptr<Qubit>& q) {
    auto& vec = state_to_qubits_[q->state()];
    vec.erase(std::remove_if(vec.begin(), vec.end(),
        [&](const std::weak_ptr<Qubit>& wq) {
            auto sq = wq.lock();
            return !sq || sq == q;
        }), vec.end());

    if (vec.empty())
        state_to_qubits_.erase(q->state());
}

std::vector<std::shared_ptr<Qubit>> QuantumStateRegistry::get_qubits(std::shared_ptr<QuantumState> state) {
    std::vector<std::shared_ptr<Qubit>> result;
    auto it = state_to_qubits_.find(state);
    if (it != state_to_qubits_.end()) {
        for (auto& wq : it->second) {
            if (auto sq = wq.lock())
                result.push_back(sq);
        }
    }
    return result;
}
