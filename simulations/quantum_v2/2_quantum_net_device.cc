#include "2_quantum_net_device.h"
#include "2_quantum_channel.h"
#include "2_quantum_component.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/node.h"


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(QuantumNetDevice);

TypeId QuantumNetDevice::GetTypeId() {
    static TypeId tid = TypeId("ns3::QuantumNetDevice")
        .SetParent<NetDevice>()
        .SetGroupName("Quantum")
        .AddConstructor<QuantumNetDevice>();
    return tid;
}

QuantumNetDevice::QuantumNetDevice()
    : m_index(0) {}

void QuantumNetDevice::Attach(Ptr<QuantumChannel> channel) {
    m_channel = channel;
}

void QuantumNetDevice::SetComponent(Ptr<QuantumComponent> component) {
    m_component = component;
}

void QuantumNetDevice::SendQubit(std::shared_ptr<Qubit> q) {

    if (m_channel) {
        m_component->RemoveQubit(q); // Maybe this should also be scheduled?
        m_channel->Transmit(q);
    }
}

Ptr<Channel> QuantumNetDevice::GetChannel() const {
    return m_channel;
}

void QuantumNetDevice::SetNode(Ptr<Node> node) {
    m_node = node;
}

Ptr<Node> QuantumNetDevice::GetNode() const {
    return m_node;
}

}