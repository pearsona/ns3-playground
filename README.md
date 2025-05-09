# Quantum Networking Playground (ns-3 + qpp)

This repository contains a set of simulation demos combining classical `ns-3` networking with `qpp` (Quantum++) to prototype quantum network behaviors to varying degrees.

## 🧠 Structure

- `simulations/` — All simulation demos.
- `simulations/quantum/` — Core quantum components (currently independent on ns3, but able to be scheduled by ns3's scheduler):
  - `quantum_state.h` — Represents shared quantum state (1+ qubits).
  - `qubit.h` — Lightweight handle for a single qubit.
  - `quantum_node.h` — Logic for qubit creation, gate application, and measurement.
  - `quantum_channel.h` — Simulates quantum link delay and scheduling.

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

The first fully quantum demo using custom `QuantumNode`, `QuantumChannel`, and `Qubit` classes. A qubit is created at Alice, a random gate is applied, and the resulting state is sent to Bob on a quantum channel with simulated link delay.

---

## 🔭 Next Steps

- Consider pros/cons of making quantum components subclasses of their classical counterparts in ns3.
- Support for **quantum loss**, **fidelity decay**, and **channel noise**.
- Introduce **entanglement generation** and **Bell state transmission**.
- Add **entanglement-based protocols** (e.g., teleportation, superdense coding).
- Support **multi-qubit states**, distributed tracking, and **entanglement swapping**.
- Gradually transition to `*.cc`/`*.h` pairs and modular build for long-term scaling.

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
