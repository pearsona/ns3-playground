#pragma once
#include "qubit.h"
#include "quantum_node.h"

#include "ns3/core-module.h"

class QuantumChannel {
public:
    QuantumChannel(double delay_ns, double loss_prob = 0.0)
        : delay_(delay_ns), loss_prob_(loss_prob) {}

    void transmit(const Qubit& q, QuantumNode& receiver);
    

private:
    double delay_;
    double loss_prob_;
};


// ----------- Inline implementations ------------
inline void QuantumChannel::transmit(const Qubit& q, QuantumNode& receiver) {
    qpp::ket state = q.state()->get_ket();
    ns3::Simulator::Schedule(ns3::NanoSeconds(delay_), [&receiver, state]() {
        std::cout << "[QuantumChannel] Transmitting qubit at t = " 
        << ns3::Simulator::Now().GetMicroSeconds() << "µs\n";

        receiver.receive(state);
    });
}