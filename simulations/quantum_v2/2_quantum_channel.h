#pragma once
#include "ns3/channel.h"
#include "ns3/nstime.h"
#include "2_qubit.h"
#include "2_quantum_component.h"

namespace ns3 {

class QuantumChannel : public Channel {
public:
    static TypeId GetTypeId();

    QuantumChannel();
    
    void SetDelay(Time delay);
    Time GetDelay() const;

    void SetLossProbability(double loss);

    std::size_t GetNDevices() const override;
    Ptr<NetDevice> GetDevice(std::size_t i) const override;


    void Connect(Ptr<QuantumComponent> sender, Ptr<QuantumComponent> receiver);
    void Transmit(std::shared_ptr<Qubit> q);

private:
    Time m_delay;
    double m_lossProb;

    Ptr<QuantumComponent> m_sender;
    Ptr<QuantumComponent> m_receiver;
};

}