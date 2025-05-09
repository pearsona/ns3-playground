#pragma once
#include <string>
#include <vector>
#include <iostream>

#include "qpp/qpp.hpp"
#include "qubit.h"

#include "ns3/core-module.h"


using qpp::cmat;
using qpp::idx;
using qpp::disp;
using qpp::ket;

class QuantumNode {
public:
    QuantumNode(std::string name = "") : name_(std::move(name)) {}

    Qubit create_qubit();
    void apply_gate(const cmat& gate, const Qubit& q);
    idx measure(const Qubit& q);
    void receive(const ket& received_state);

    void set_name(const std::string& name) { name_ = name; }
    const std::string& name() const { return name_; }

private:
    std::string name_;
    std::vector<Qubit> qubits_;
};


// ----------- Inline implementations ------------
inline Qubit QuantumNode::create_qubit() {
    auto state = std::make_shared<QuantumState>(1);
    Qubit q(0, state);
    qubits_.push_back(q);
    return q;
}

inline void QuantumNode::apply_gate(const cmat& gate, const Qubit& q) {
    q.state()->apply_gate(gate, {q.index()});
}

inline idx QuantumNode::measure(const Qubit& q) {
    return q.state()->measure({q.index()}).first;
}

inline void QuantumNode::receive(const ket& received_state) {
    std::cout << "[Quantum] ";
    if (!name_.empty()) std::cout << name_ << " ";
    std::cout << "received state at t = " << ns3::Simulator::Now().GetMicroSeconds() << "µs:\n" << disp(received_state) << "\n\n";
}