#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include "qpp/qpp.hpp"

using namespace ns3;
using namespace qpp;

NS_LOG_COMPONENT_DEFINE ("QuantumConditionalSend");

Ptr<NetDevice> senderDevice;
Address receiverAddress;
uint16_t protocolNumber = 0x0800; // Arbitrary value

void SendPacket()
{
    Ptr<Packet> packet = Create<Packet> (1024); // 1024 bytes
    senderDevice->Send(packet, receiverAddress, protocolNumber);
    NS_LOG_UNCOND ("[Classical] Packet sent at " << Simulator::Now ().GetSeconds () << " seconds");
}

int main (int argc, char *argv[])
{
    Time::SetResolution (Time::NS);

    NodeContainer nodes;
    nodes.Create (2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer devices = pointToPoint.Install (nodes);
    senderDevice = devices.Get(0);
    receiverAddress = devices.Get(1)->GetAddress();

    // Quantum logic
    ket plus = st.z0;              // |0⟩
    plus = gt.H * plus;            // Apply Hadamard: now |+⟩
    auto result = measure(plus, gt.Z, {0});
    idx meas = std::get<0>(result);

    NS_LOG_UNCOND("[Quantum] Measured |+⟩: " << meas);

    if (meas == 1)
    {
        Simulator::Schedule (Seconds(0.0), &SendPacket);
    }
    else
    {
        NS_LOG_UNCOND ("[Quantum] Measurement was 0, so no packet sent.");
    }

    Simulator::Run();
    Simulator::Destroy();

    std::clog << std::flush; // Ensure NS_LOG_UNCOND output is shown
    return 0;
}