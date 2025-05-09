#include "ns3/core-module.h"

#include "quantum/quantum_channel.h"
#include "quantum/quantum_node.h"

using namespace ns3;
using namespace qpp;


int main(int argc, char* argv[]) {
    Time::SetResolution(Time::NS);

    QuantumNode alice("Alice");
    QuantumNode bob("Bob");

    QuantumChannel channel(2000.0);

    Qubit q = alice.create_qubit();
    alice.apply_gate(randU(2), q);
    std::cout << "[Alice] Sending state:\n" << qpp::disp(q.state()->get_ket()) << "\n\n";

    channel.transmit(q, bob);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}