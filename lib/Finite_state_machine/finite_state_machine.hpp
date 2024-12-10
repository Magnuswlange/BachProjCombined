#pragma once
#include <cstdint>
#include <functional> // for c++ std::function func ptr
#include "states.hpp"

using StateCallback = std::function<void(State)>;

class FiniteStateMachine
{
public:
    FiniteStateMachine(StateCallback stateEnterCb, StateCallback stateExitCb);
    State GetState();
    void SetState(State newState);
    void OnStateEnterCallback();
    void OnStateExitCallback();

private:
    void OnStateExit();
    void OnStateEnter(State newState);

    StateCallback m_OnStateEnterCallback;
    StateCallback m_OnStateExitCallback;
    State m_State = State::IDLE;
};