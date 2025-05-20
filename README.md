# Quantum Networking Playground (ns-3 + qpp)

This repository contains a set of simulation demos combining classical `ns-3` networking with `qpp` (Quantum++) to prototype quantum network behaviors to varying degrees.

## 🧠 Structure

- `simulations/` — All simulation demos.
- `simulations/quantum_v2/` — More mature, `ns3`-native quantum architecture:
  - `2_quantum_component.h/.cc` — Represents qubit logic at the node level (creation, gates, measurement). Aggregated with `ns3::Node`.
  - `2_qubit.h` — Lightweight handle for a single qubit, now with optional string ID.
  - `2_quantum_state.h` — Pure quantum state logic (ket/density matrix abstraction).
  - `2_quantum_state_registry.h/.cc` — Tracks global qubit-state associations to support entanglement and distributed updates.
  - `2_quantum_channel.h/.cc` — Subclasses `ns3::Channel`, enabling quantum link delay, with pluggable future support for noise/loss.
  - `2_quantum_net_device.h/.cc` — Subclass of `ns3::NetDevice`, connecting nodes to quantum channels. Integrates with `QuantumComponent`.
- `simulations/quantum_v1/` — First iteration quantum components (beginning to integrate more fully into ns3):
  - `1_quantum_state.h` — Represents shared quantum state (1+ qubits).
  - `1_quantum_state_registry.h` - Tracks all quantum states across the network, which is necessary for proper tracking of qubits/states that are entangled but at different nodes.
  - `1_quantum_state_registry.cc` - Implementation of methods for the `QuantumStateRegistry`
  - `1_qubit.h` — Lightweight handle for a single qubit.
  - `1_quantum_component.h` — Logic for qubit creation, gate application, and measurement. Subclasses `ns3::Object` and is intended to be aggregated with `Node`.
  - `1_quantum_channel.h` — Simulates quantum link delay and scheduling.
- `simulations/quantum_v0/` — Zeroth iteration quantum components (currently independent on ns3, but able to be scheduled by ns3's scheduler):
  - `0_quantum_state.h` — Represents shared quantum state (1+ qubits).
  - `0_qubit.h` — Lightweight handle for a single qubit.
  - `0_quantum_node.h` — Logic for qubit creation, gate application, and measurement.
  - `0_quantum_channel.h` — Simulates quantum link delay and scheduling.

Each class is implemented inline in its `.h` file to allow fast iteration and single-file simplicity.

---

## 🚀 Demos

### `00_classical_node_demo.cc`

A basic `ns-3` point-to-point network simulation that sends classical packets and logs when they're sent/received.

### `01_classical_node_quantum_condition_demo.cc`

A hybrid classical/quantum demo where a quantum measurement result conditions whether a classical packet is sent.

### `02_classical_node_quantum_state_demo.cc`

A fully classical `ns-3` setup that transmits a serialized random quantum state (ket) as a byte buffer and reconstructs it on the receiving end.

### `03_quantum_node_send_demo.cc`

The first fully quantum demo using custom `QuantumNode`, `QuantumChannel`, and `Qubit` classes. A qubit is created at Alice, a random gate is applied, and the resulting state is sent to Bob on a quantum channel with simulated link delay. Relies on `quantum_v0` files.

### `04_quantum_component_teleport_demo.cc`

A proper demo of teleportation with transmission of both qubits and classical bits. This is the first demo where ns3 nodes actually contain quantum behavior via a `QuantumComponent` class that replaces the previous `QuantumNode` class. NOTE: `QuantumChannel` is still a distinct class that uses the ns3 simulator scheduling mechanism, but is not in fact part of ns3's hierarchy. Relies on `quantum_v1` files.

### `05_quantum_channel_teleportation_demo.cc`

A proper demo of teleportation with `QuantumChannel` as an actual subclass of ns3's `Channel` and `QuantumNetDevice` as a subclass of ns3's `NetDevice` (this is to mimic ns3's style AND to provide a potential framework for where transduction physics can go). Also added the ability to have qubit receive callbacks in the `QuantumComponent` `StoreQubit` method that is called when a component receives a qubit, similar to `ns3::Socket` receive handlers.

---

## 🔭 Next Steps

- Point-to-point-style helper for quantum links (`QuantumPointToPointHelper`) to simplify simulation configuration.
- Adding gate and measurement duration logic with scheduling.
- QKD demo and potentially class.
- Valuable metrics like entanglement entropy, fidelity, etc.
- Support for **quantum loss**, **fidelity decay**, and **channel noise**.
- Introduce **entanglement generation** and **Bell state transmission**.
- Add **entanglement-based protocols** (e.g., teleportation, superdense coding).
- Support **multi-qubit states**, distributed tracking, and **entanglement swapping**.
- Gradually transition to `*.cc`/`*.h` pairs and modular build for long-term scaling.
- **Python bindings (proof-of-concept)** to test accessibility and automation potential.

---

## 🛠 Requirements

- C++20
- ns-3 (compiled with CMake, e.g. v3.44)
- Quantum++ (qpp) + Eigen + qasmtools (cloned locally)

---

## 📦 Usage

```bash
cd build
cmake ..
make -j
./03_quantum_node_send_demo
```

Use `std::cout` for output logging (preferred). To enable `NS_LOG`, set:

```bash
export NS_LOG="*=level_all"
```

or call `setenv(...)` in `main()`.

---
