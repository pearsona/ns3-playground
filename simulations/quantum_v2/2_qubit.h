#pragma once
#include "2_quantum_state.h"
#include "2_quantum_state_registry.h"
#include <memory>
#include <string>

class Qubit : public std::enable_shared_from_this<Qubit> {
public:
    // Default constructor: qubit in |0⟩
    Qubit(const std::string& id = "")
        : id_(id), index_(0), state_(std::make_shared<QuantumState>(1)) {}



    // Constructor for manually assigned index/state
    Qubit(size_t index, std::shared_ptr<QuantumState> state, const std::string& id = "")
        : id_(id), index_(index), state_(std::move(state)) {}

        
    size_t index() const;
    void set_index(size_t index) { index_ = index; }

    QuantumState::Ptr state() const;
    void set_state(std::shared_ptr<QuantumState> new_state);

    std::string get_id() const { return id_; }
    void set_id(const std::string& id) { id_ = id; }

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