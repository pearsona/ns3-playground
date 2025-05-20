#pragma once
#include "ns3/net-device.h"
#include "ns3/ptr.h"
#include "2_qubit.h"
#include <memory>

namespace ns3 {

class QuantumChannel;
class QuantumComponent;

class QuantumNetDevice : public NetDevice {
public:
    static TypeId GetTypeId();

    QuantumNetDevice();

    void Attach(Ptr<QuantumChannel> channel);
    void SetComponent(Ptr<QuantumComponent> component);

    void SendQubit(std::shared_ptr<Qubit> q);

    // Minimal overrides for now
    Ptr<Channel> GetChannel() const override;
    void SetNode(Ptr<Node> node) override;
    Ptr<Node> GetNode() const override;

    void SetAddress(Address address) override {}
    Address GetAddress() const override { return Address(); }

    void SetIfIndex(uint32_t index) override { m_index = index; }
    uint32_t GetIfIndex() const override { return m_index; }

    bool SetMtu(uint16_t mtu) override { return true; }
    uint16_t GetMtu() const override { return 1500; }

    bool IsLinkUp() const override { return true; }
    void AddLinkChangeCallback(Callback<void> callback) override {}

    bool IsBroadcast() const override { return false; }
    Address GetBroadcast() const override { return Address(); }

    bool IsMulticast() const override { return false; }
    Address GetMulticast(Ipv4Address) const override { return Address(); }
    Address GetMulticast(Ipv6Address) const override { return Address(); }

    bool IsBridge() const override { return false; }
    bool IsPointToPoint() const override { return true; }

    bool NeedsArp() const override { return false; }

    void SetReceiveCallback(ReceiveCallback cb) override {}
    void SetPromiscReceiveCallback(PromiscReceiveCallback cb) override {}
    bool SupportsSendFrom() const override { return false; }

    bool Send(Ptr<Packet>, const Address&, uint16_t) override { return false; }
    bool SendFrom(Ptr<Packet>, const Address&, const Address&, uint16_t) override { return false; }


private:
    uint32_t m_index;
    Ptr<Node> m_node;
    Ptr<QuantumChannel> m_channel;
    Ptr<QuantumComponent> m_component;
};

} 
