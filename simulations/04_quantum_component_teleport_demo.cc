#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "1_quantum_component.h"
#include "1_quantum_channel.h"

using namespace ns3;

// --- Free function for classical receive + correction ---
static std::map<Ptr<Socket>, std::pair<Ptr<QuantumComponent>, std::shared_ptr<Qubit>>> g_socketContext;

void ReceiveCallback(Ptr<Socket> socket) {
    auto [qBob, qB] = g_socketContext[socket];

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

    // Internet setup
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

    // Create quantum components
    Ptr<QuantumComponent> qAlice = CreateObject<QuantumComponent>();
    Ptr<QuantumComponent> qBob = CreateObject<QuantumComponent>();
    alice->AggregateObject(qAlice);
    bob->AggregateObject(qBob);

    // Create quantum channel
    QuantumChannel qChannel(2000.0);

    // Create EPR pair and transmit one qubit
    auto [qA, qB] = qAlice->CreateEntangledPair();
    qChannel.transmit(qB, *qBob);

    // Step 1–3: Alice prepares and measures qubits (at t = 1000 ns)
    Simulator::Schedule(NanoSeconds(1000), [qAlice, qA, alice, interfaces]() {
        auto psi = qAlice->CreateQubit();
        qAlice->ApplyGate(qpp::gt.X, psi);
        qAlice->ApplyGate(qpp::gt.H, psi);
        qAlice->ApplyGate(qpp::gt.CNOT, {psi, qA});
        qAlice->ApplyGate(qpp::gt.H, psi);

        qpp::idx m1 = qAlice->Measure(psi);
        qpp::idx m2 = qAlice->Measure(qA);

        // Send classical bits to Bobj
        Ptr<Socket> source = Socket::CreateSocket(alice, TypeId::LookupByName("ns3::UdpSocketFactory"));
        InetSocketAddress remote = InetSocketAddress(interfaces.GetAddress(1), 8080);
        source->Connect(remote);

        uint8_t data[2] = {static_cast<uint8_t>(m1), static_cast<uint8_t>(m2)};
        Ptr<Packet> packet = Create<Packet>(data, 2);

        Simulator::Schedule(NanoSeconds(100), [source, packet]() {
            source->Send(packet);
        });
    });

    // Setup Bob's receiving socket and bind receive callback
    Ptr<Socket> sink = Socket::CreateSocket(bob, TypeId::LookupByName("ns3::UdpSocketFactory"));
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 8080);
    sink->Bind(local);
    
    g_socketContext[sink] = {qBob, qB};
    sink->SetRecvCallback(MakeCallback(&ReceiveCallback));


    // Run simulation
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
