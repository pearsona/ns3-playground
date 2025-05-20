#include "2_quantum_component.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include <unordered_set>
#include <algorithm>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(QuantumComponent);

TypeId QuantumComponent::GetTypeId() {
    static TypeId tid = TypeId("ns3::QuantumComponent")
        .SetParent<Object>()
        .SetGroupName("Quantum")
        .AddConstructor<QuantumComponent>();
    return tid;
}

QuantumComponent::QuantumComponent() = default;

std::shared_ptr<Qubit> QuantumComponent::CreateQubit(const std::string& id) {
    auto q = std::make_shared<Qubit>(id);
    QuantumStateRegistry::instance().register_qubit(q);
    qubits_.push_back(q);
    return q;
}

std::shared_ptr<Qubit> QuantumComponent::GetQubitById(const std::string& id) const {
    for (const auto& q : qubits_) {
        if (q->get_id() == id)
            return q;
    }
    return nullptr;
}

std::pair<std::shared_ptr<Qubit>, std::shared_ptr<Qubit>> QuantumComponent::CreateEntangledPair() {
    auto state = std::make_shared<QuantumState>(2);
    state->apply_gate(qpp::gt.H, {0});
    state->apply_gate(qpp::gt.CNOT, {0, 1});

    auto q1 = std::make_shared<Qubit>(0, state);
    auto q2 = std::make_shared<Qubit>(1, state);

    QuantumStateRegistry::instance().register_qubit(q1);
    QuantumStateRegistry::instance().register_qubit(q2);

    StoreQubit(q1);
    StoreQubit(q2);
    return {q1, q2};
}

void QuantumComponent::StoreQubit(std::shared_ptr<Qubit> q) {
    QuantumStateRegistry::instance().register_qubit(q);
    qubits_.push_back(q);
    if (receive_callback_) {
        receive_callback_(q);
    }
}


void QuantumComponent::RemoveQubit(std::shared_ptr<Qubit> q) {
    QuantumStateRegistry::instance().unregister_qubit(q);
    qubits_.erase(std::remove(qubits_.begin(), qubits_.end(), q), qubits_.end());
}


void QuantumComponent::ApplyGate(const qpp::cmat& gate, const std::shared_ptr<Qubit>& q) {
    q->state()->apply_gate(gate, {q->index()});
}

void QuantumComponent::ApplyGate(const qpp::cmat& gate, const std::vector<std::shared_ptr<Qubit>>& qs) {
    if (qs.empty()) return;

    std::unordered_map<QuantumState::Ptr, std::vector<std::shared_ptr<Qubit>>> state_groups;
    for (const auto& q : qs) {
        state_groups[q->state()].push_back(q);
    }

    if (state_groups.size() == 1) {
        std::vector<qpp::idx> targets;
        for (const auto& q : qs)
            targets.push_back(q->index());
        qs[0]->state()->apply_gate(gate, targets);
        return;
    }

    std::unordered_set<std::shared_ptr<Qubit>> all_qs_set;
    for (const auto& q : qs) {
        auto related = QuantumStateRegistry::instance().get_qubits(q->state());
        for (const auto& qb : related)
            all_qs_set.insert(qb);
    }

    std::vector<std::shared_ptr<Qubit>> all_qs(all_qs_set.begin(), all_qs_set.end());
    std::sort(all_qs.begin(), all_qs.end(), [](const auto& a, const auto& b) {
        return a->state().get() < b->state().get();
    });

    std::vector<std::shared_ptr<QuantumState>> ordered_states;
    for (const auto& [state, _] : state_groups)
        ordered_states.push_back(state);

    std::sort(ordered_states.begin(), ordered_states.end(), [](const auto& a, const auto& b) {
        return a.get() < b.get();
    });

    qpp::ket combined = ordered_states[0]->get_ket();
    for (size_t i = 1; i < ordered_states.size(); ++i)
        combined = qpp::kron(combined, ordered_states[i]->get_ket());

    auto new_state = std::make_shared<QuantumState>(combined);
    for (size_t i = 0; i < all_qs.size(); ++i) {
        all_qs[i]->set_index(i);
        all_qs[i]->set_state(new_state);
    }

    std::vector<qpp::idx> targets;
    for (const auto& q : qs)
        targets.push_back(q->index());

    new_state->apply_gate(gate, targets);
}

qpp::idx QuantumComponent::Measure(std::shared_ptr<Qubit> q) {
    auto current_state = q->state();
    auto related_qubits = QuantumStateRegistry::instance().get_qubits(current_state);

    auto [result, collapsed_state] = current_state->measure(q->index());

    int removed_index = static_cast<int>(q->index());
    auto new_state = std::make_shared<QuantumState>(collapsed_state);

    for (auto& qb : related_qubits) {
        if (qb == q) continue;
        int old_idx = static_cast<int>(qb->index());
        int new_idx = old_idx > removed_index ? old_idx - 1 : old_idx;
        qb->set_state(new_state);
        qb->set_index(new_idx);
    }

    q->set_state(std::make_shared<QuantumState>(result == 0 ? qpp::st.z0 : qpp::st.z1));
    q->set_index(0);
    return result;
}

void QuantumComponent::AddDevice(Ptr<QuantumNetDevice> dev) {
    Ptr<Node> node = GetObject<Node>();
    if (node) {
        node->AddDevice(dev);
    }
    m_netDevices.push_back(dev);
    dev->SetComponent(this);
}

void QuantumComponent::SetReceiveCallback(QubitReceiveCallback cb) {
    receive_callback_ = std::move(cb);
}



void QuantumComponent::PrintAllStates() const {
    std::cout << "[QuantumComponent] Printing all states. Number of qubits: " << qubits_.size() << "\n";
    int i = 0;
    for (const auto& q : qubits_) {
        std::cout << "[QuantumComponent] Qubit " << i << " is index " << q->index() << " in state:\n"
                  << qpp::disp(q->state()->get_ket()) << "\n";
        i++;
    }
    std::cout << "\n\n";
}

}