#pragma once
#include "ns3/object.h"
#include "2_qubit.h"
#include "2_quantum_net_device.h"

#include <memory>
#include <vector>
#include <functional>

using QubitReceiveCallback = std::function<void(std::shared_ptr<Qubit>)>;

namespace ns3 {

class QuantumComponent : public Object {
public:
    static TypeId GetTypeId();

    QuantumComponent();

    std::shared_ptr<Qubit> CreateQubit(const std::string& id = "");
    std::shared_ptr<Qubit> GetQubitById(const std::string& id) const;

    void StoreQubit(std::shared_ptr<Qubit> q);
    void RemoveQubit(std::shared_ptr<Qubit> q);


    std::pair<std::shared_ptr<Qubit>, std::shared_ptr<Qubit>> CreateEntangledPair();

    void ApplyGate(const qpp::cmat& gate, const std::shared_ptr<Qubit>& q);
    void ApplyGate(const qpp::cmat& gate, const std::vector<std::shared_ptr<Qubit>>& qs);

    qpp::idx Measure(std::shared_ptr<Qubit> q);

    void AddDevice(Ptr<QuantumNetDevice> dev);
    void SetReceiveCallback(QubitReceiveCallback cb);

    void PrintAllStates() const;

private:
    std::vector<std::shared_ptr<Qubit>> qubits_;
    std::vector<Ptr<QuantumNetDevice>> m_netDevices;
    QubitReceiveCallback receive_callback_;
};

}