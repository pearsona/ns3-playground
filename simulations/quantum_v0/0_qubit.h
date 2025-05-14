#pragma once
#include "0_quantum_state.h"

class Qubit {
public:
    Qubit(size_t index, QuantumState::Ptr state);

    size_t index() const;
    QuantumState::Ptr state() const;

private:
    size_t index_;
    QuantumState::Ptr state_;
};


// ----------- Inline implementations ------------
inline Qubit::Qubit(size_t index, QuantumState::Ptr state) : index_{index}, state_{std::move(state)} {}
inline size_t Qubit::index() const { return index_; }
inline QuantumState::Ptr Qubit::state() const { return state_; }