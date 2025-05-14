#pragma once
#include "ns3/object.h"
#include "1_qubit.h"
#include <iostream>
#include <unordered_set>

class QuantumComponent : public ns3::Object {
public:
    static ns3::TypeId GetTypeId() {
        static ns3::TypeId tid = ns3::TypeId("QuantumComponent")
            .SetParent<ns3::Object>()
            .SetGroupName("Quantum")
            .AddConstructor<QuantumComponent>();
        return tid;
    }

    QuantumComponent() = default;

    std::shared_ptr<Qubit> CreateQubit() {
        auto state = std::make_shared<QuantumState>(1);
        auto q = std::make_shared<Qubit>(0, state);
        QuantumStateRegistry::instance().register_qubit(q);
        qubits_.push_back(q);
        return q;
    }

    void ApplyGate(const qpp::cmat& gate, const std::shared_ptr<Qubit>& q) {
        q->state()->apply_gate(gate, {q->index()});
    }

    void ApplyGate(const qpp::cmat& gate, const std::vector<std::shared_ptr<Qubit>>& qs) {
        if (qs.empty()) return;
    
        // Step 1: Group qubits by unique QuantumState
        std::unordered_map<QuantumState::Ptr, std::vector<std::shared_ptr<Qubit>>> state_groups;
        for (const auto& q : qs) {
            state_groups[q->state()].push_back(q);
        }
    
        // Step 2: If all qubits already share the same state, apply directly
        if (state_groups.size() == 1) {
            std::vector<qpp::idx> targets;
            for (const auto& q : qs)
                targets.push_back(q->index());
            qs[0]->state()->apply_gate(gate, targets);
            return;
        }
    
        // Step 3: Gather all related qubits (union of all entangled states)
        std::unordered_set<std::shared_ptr<Qubit>> all_qs_set;
        for (const auto& q : qs) {
            auto related = QuantumStateRegistry::instance().get_qubits(q->state());
    
            for (const auto& qb : related) {
                all_qs_set.insert(qb);
            }
        }
    
        std::vector<std::shared_ptr<Qubit>> all_qs(all_qs_set.begin(), all_qs_set.end());
        std::sort(all_qs.begin(), all_qs.end(), [](const auto& a, const auto& b) {
            return a->state().get() < b->state().get(); // deterministic ordering by state pointer
        });

    
        // Step 4: Build the combined ket from unique states, ordered
        std::vector<std::shared_ptr<QuantumState>> ordered_states;
        for (const auto& [state, _] : state_groups)
            ordered_states.push_back(state);
    
        std::sort(ordered_states.begin(), ordered_states.end(), [](const auto& a, const auto& b) {
            return a.get() < b.get(); // ensure deterministic tensor product order
        });
    
        qpp::ket combined = ordered_states[0]->get_ket();
        for (size_t i = 1; i < ordered_states.size(); ++i)
            combined = qpp::kron(combined, ordered_states[i]->get_ket());

    
        // Step 5: Assign new joint state and update qubit indices
        auto new_state = std::make_shared<QuantumState>(combined);
        for (size_t i = 0; i < all_qs.size(); ++i) {
            all_qs[i]->set_index(i);
            all_qs[i]->set_state(new_state);
        }
    
        // Step 6: Apply the gate to correct targets
        std::vector<qpp::idx> targets;
        for (const auto& q : qs)
            targets.push_back(q->index());
    
        new_state->apply_gate(gate, targets);
    }
    

    qpp::idx Measure(std::shared_ptr<Qubit> q) {

        auto current_state = q->state();
        auto related_qubits = QuantumStateRegistry::instance().get_qubits(current_state);

        auto [result, collapsed_state] = q->state()->measure(q->index());

        int removed_index = static_cast<int>(q->index());
        auto new_state = std::make_shared<QuantumState>(collapsed_state);
        for (auto& qb : related_qubits) {
            if (qb == q) continue;

            int old_idx = static_cast<int>(qb->index());
            int new_idx = old_idx > removed_index ? old_idx - 1 : old_idx;

            qb->set_state(new_state);
            qb->set_index(new_idx);
        }

        // Measured qubit becomes |0⟩ or |1⟩
        q->set_state(std::make_shared<QuantumState>(result == 0 ? qpp::st.z0 : qpp::st.z1));
        q->set_index(0);

        return result;
    }


    std::pair<std::shared_ptr<Qubit>, std::shared_ptr<Qubit>> CreateEntangledPair() {
        auto state = std::make_shared<QuantumState>(2);
        state->apply_gate(qpp::gt.H, {0});
        state->apply_gate(qpp::gt.CNOT, {0, 1});
    
        auto q1 = std::make_shared<Qubit>(0, state);
        auto q2 = std::make_shared<Qubit>(1, state);
    
        QuantumStateRegistry::instance().register_qubit(q1);
        QuantumStateRegistry::instance().register_qubit(q2);
    
        qubits_.push_back(q1); // Only store one
        return std::make_pair(q1, q2); // Return both by shared_ptr
    }
    

    void StoreQubit(std::shared_ptr<Qubit> q) {
        QuantumStateRegistry::instance().register_qubit(q);
        qubits_.push_back(q);
    }

    void PrintAllStates() const {
        std::cout << "[QuantumComponent] Printing all states. Number of qubits: " << qubits_.size() << "\n";
        int i = 0;
        for (const auto& q : qubits_) {
            std::cout << "[QuantumComponent] Qubit " << i << " is index " << q->index() << " in state:\n"
                      << qpp::disp(q->state()->get_ket()) << "\n";
            i++;
        }
        std::cout << "\n\n";
    }

private:
    std::vector<std::shared_ptr<Qubit>> qubits_;
};