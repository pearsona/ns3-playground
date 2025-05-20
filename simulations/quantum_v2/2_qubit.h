#pragma once
#include "2_quantum_state.h"
#include "2_quantum_state_registry.h"
#include <memory>

class Qubit : public std::enable_shared_from_this<Qubit> {
public:
    Qubit(size_t index, std::shared_ptr<QuantumState> state)
        : index_(index), state_(std::move(state)) {}

    size_t index() const;
    void set_index(size_t index) { index_ = index; }

    QuantumState::Ptr state() const;
    void set_state(std::shared_ptr<QuantumState> new_state);

private:
    std::string id_;
    size_t index_;
    QuantumState::Ptr state_;
};


// ----------- Inline implementations ------------
inline size_t Qubit::index() const { return index_; }
inline QuantumState::Ptr Qubit::state() const { return state_; }

inline void Qubit::set_state(std::shared_ptr<QuantumState> new_state) {
    QuantumStateRegistry::instance().unregister_qubit(shared_from_this());
    state_ = std::move(new_state);
    QuantumStateRegistry::instance().register_qubit(shared_from_this());
}
