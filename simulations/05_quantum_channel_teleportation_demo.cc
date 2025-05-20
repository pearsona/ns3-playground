#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "quantum_v2/2_qubit.h"
#include "quantum_v2/2_quantum_component.h"
#include "quantum_v2/2_quantum_channel.h"
#include "quantum_v2/2_quantum_net_device.h"

using namespace ns3;


// --- Free function for classical receive + correction ---
void ReceiveCallback(Ptr<Socket> socket) {
    std::cout << "[main] t = " << Simulator::Now().GetMicroSeconds() 
              << "µs: Bob receives classical message and applies corrections\n";

    Ptr<Node> bob = socket->GetNode();
    Ptr<QuantumComponent> qBob = bob->GetObject<QuantumComponent>();
    auto qB = qBob->GetQubitById("teleport_target");


    Ptr<Packet> packet = socket->Recv();
    uint8_t data[2];
    packet->CopyData(data, 2);
    qpp::idx m1 = data[0];
    qpp::idx m2 = data[1];

    if (m2 == 1) qBob->ApplyGate(qpp::gt.X, qB);
    if (m1 == 1) qBob->ApplyGate(qpp::gt.Z, qB);

    qBob->PrintAllStates();
}

int main() {
    Time::SetResolution(Time::NS);

    // Create nodes
    Ptr<Node> alice = CreateObject<Node>();
    Ptr<Node> bob = CreateObject<Node>();

    // Classical network setup
    InternetStackHelper internet;
    internet.Install(alice);
    internet.Install(bob);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer devices = p2p.Install(alice, bob);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

    // Classical receive socket on Bob
    Ptr<Socket> sink = Socket::CreateSocket(bob, TypeId::LookupByName("ns3::UdpSocketFactory"));
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 8080);
    sink->Bind(local);
    sink->SetRecvCallback(MakeCallback(&ReceiveCallback));

    // Quantum components
    Ptr<QuantumComponent> qAlice = CreateObject<QuantumComponent>();
    Ptr<QuantumComponent> qBob = CreateObject<QuantumComponent>();
    alice->AggregateObject(qAlice);
    bob->AggregateObject(qBob);

    // Quantum devices
    Ptr<QuantumNetDevice> devA = CreateObject<QuantumNetDevice>();
    Ptr<QuantumNetDevice> devB = CreateObject<QuantumNetDevice>();

    qAlice->AddDevice(devA);
    qBob->AddDevice(devB);

    // Quantum channel
    Ptr<QuantumChannel> qChannel = CreateObject<QuantumChannel>();
    qChannel->SetDelay(NanoSeconds(2000));
    qChannel->Connect(qAlice, qBob);
    devA->Attach(qChannel);
    devB->Attach(qChannel);

    // Full Teleportation Protocol
    Simulator::Schedule(NanoSeconds(0), [=]() {

        // Alice creates an EPR pair and sends half to Bob
        std::cout << "[main] t = " << Simulator::Now().GetMicroSeconds() 
            << "µs: Alice creates entangled pair and sends half to Bob\n";
        auto [qA, qB] = qAlice->CreateEntangledPair();
        qB->set_id("teleport_target");
        devA->SendQubit(qB);

        // Alice prepares and measures her special state, psi
        Simulator::Schedule(Simulator::Now() + NanoSeconds(2000), [=]() {
            std::cout << "[main] t = "<< Simulator::Now().GetMicroSeconds() 
                << "µs: Alice prepares and measures psi\n";

            auto psi = qAlice->CreateQubit();
            qAlice->ApplyGate(qpp::gt.X, psi);
            qAlice->ApplyGate(qpp::gt.H, psi);
            qAlice->ApplyGate(qpp::gt.CNOT, {psi, qA});
            qAlice->ApplyGate(qpp::gt.H, psi);

            qpp::idx m1 = qAlice->Measure(psi);
            qpp::idx m2 = qAlice->Measure(qA);

            // Alice sends measurement results to Bob
            // NOTE: Bob's corrections are built into the ReceiveCallback function
            Simulator::Schedule(Simulator::Now() + NanoSeconds(100), [=]() {
                Ptr<Socket> source = Socket::CreateSocket(alice, TypeId::LookupByName("ns3::UdpSocketFactory"));
                InetSocketAddress remote = InetSocketAddress(interfaces.GetAddress(1), 8080);
                source->Connect(remote);

                uint8_t data[2] = {static_cast<uint8_t>(m1), static_cast<uint8_t>(m2)};
                Ptr<Packet> packet = Create<Packet>(data, 2);

                std::cout << "[main] t = " << Simulator::Now().GetMicroSeconds() 
                    << "µs: Alice sends measurement results\n";
                source->Send(packet);
            });
        });

    });

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}