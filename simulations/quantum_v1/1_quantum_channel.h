#pragma once
#include "1_qubit.h"
#include "1_quantum_component.h"

#include "ns3/core-module.h"

class QuantumChannel {
public:
    QuantumChannel(double delay_ns, double loss_prob = 0.0)
        : delay_(delay_ns), loss_prob_(loss_prob) {}

    void transmit(std::shared_ptr<Qubit> q, QuantumComponent& receiver);
    

private:
    double delay_;
    double loss_prob_;
};


// ----------- Inline implementations ------------
inline void QuantumChannel::transmit(std::shared_ptr<Qubit> q, QuantumComponent& receiver) {
    ns3::Simulator::Schedule(ns3::NanoSeconds(delay_), [&receiver, q]() {
        std::cout << "[QuantumChannel] Transmitting qubit at t = " 
                  << ns3::Simulator::Now().GetMicroSeconds() << "µs\n";

        receiver.StoreQubit(q); // same shared pointer; entanglement preserved
    });
}