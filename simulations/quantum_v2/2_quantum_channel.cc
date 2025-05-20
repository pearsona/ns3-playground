#include "2_quantum_channel.h"

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/net-device.h"

#include <iostream>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(QuantumChannel);

TypeId QuantumChannel::GetTypeId() {
    static TypeId tid = TypeId("ns3::QuantumChannel")
        .SetParent<Channel>()
        .SetGroupName("Quantum")
        .AddConstructor<QuantumChannel>();
    return tid;
}

QuantumChannel::QuantumChannel()
    : m_delay(NanoSeconds(0)), m_lossProb(0.0) {}

void QuantumChannel::SetDelay(Time delay) {
    m_delay = delay;
}

Time QuantumChannel::GetDelay() const {
    return m_delay;
}

void QuantumChannel::SetLossProbability(double loss) {
    m_lossProb = loss;
}

void QuantumChannel::Connect(Ptr<QuantumComponent> sender, Ptr<QuantumComponent> receiver) {
    m_sender = sender;
    m_receiver = receiver;
}

void QuantumChannel::Transmit(std::shared_ptr<Qubit> q) {
    Simulator::Schedule(m_delay, [this, q]() {
        std::cout << "[QuantumChannel] t = "
                  << Simulator::Now().GetMicroSeconds() << "µs: Transmitting Qubit\n";
        this->m_receiver->StoreQubit(q);
    });
}

std::size_t QuantumChannel::GetNDevices() const {
    return 0; // not using NetDevices yet
}

Ptr<NetDevice> QuantumChannel::GetDevice(std::size_t i) const {
    return nullptr; // or actual device pointer in the future
}


}