#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include "qpp/qpp.hpp"
#include <cstdlib>


using namespace ns3;
using namespace qpp;


Ptr<NetDevice> senderDevice;
Address receiverAddress;
uint16_t protocolNumber = 0x0800;

void SendQubit()
{
    ket psi = randket(2); // Random 1-qubit state
    std::cout << "[Quantum] Original state:" << "\n";
    std::cout << disp(psi) << std::endl;

    double re0 = std::real(psi(0));
    double im0 = std::imag(psi(0));
    double re1 = std::real(psi(1));
    double im1 = std::imag(psi(1));

    // Pack into buffer
    uint8_t buffer[32];
    std::memcpy(buffer,     &re0, 8);
    std::memcpy(buffer + 8, &im0, 8);
    std::memcpy(buffer + 16, &re1, 8);
    std::memcpy(buffer + 24, &im1, 8);

    Ptr<Packet> packet = Create<Packet>(buffer, 32);
    std::cout << "[NS-3] Created packet with UID " << packet->GetUid() << ", size: " << packet->GetSize() << " bytes\n";

    senderDevice->Send(packet, receiverAddress, protocolNumber);
    std::cout << "[NS-3] Packet sent at " << Simulator::Now().GetSeconds() << " s\n\n";
}

void RxCallback (Ptr<const Packet> packet)
{
    std::cout << "[NS-3] Packet UID " << packet->GetUid() << " received at " << Simulator::Now().GetSeconds() << " s\n";
    std::cout << "[NS-3] Packet size: " << packet->GetSize() << " bytes" << "\n";

    uint8_t tempBuffer[34];
    uint8_t buffer[32];

    packet->CopyData(tempBuffer, 34);
    std::memcpy(buffer, tempBuffer + 2, 32); // Skip header

    double re0, im0, re1, im1;
    std::memcpy(&re0,  buffer,     8);
    std::memcpy(&im0,  buffer + 8, 8);
    std::memcpy(&re1,  buffer + 16, 8);
    std::memcpy(&im1,  buffer + 24, 8);

    ket received(2);
    received << cplx(re0, im0), cplx(re1, im1);
    received /= norm(received); // Normalize

    std::cout << "[Quantum] Reconstructed state:" << "\n";
    std::cout << disp(received) << "\n\n";
}


int main (int argc, char *argv[])
{
    Time::SetResolution(Time::NS);

    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices = p2p.Install(nodes);
    senderDevice = devices.Get(0);
    receiverAddress = devices.Get(1)->GetAddress();

    devices.Get(1)->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&RxCallback));

    Simulator::Schedule(Seconds(0.0), &SendQubit);
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}