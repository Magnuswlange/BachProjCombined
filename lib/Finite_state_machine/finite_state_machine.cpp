#include "finite_state_machine.hpp"
#include "states.hpp"

FiniteStateMachine::FiniteStateMachine(StateCallback stateEnterCb, StateCallback stateExitCb)
    : m_OnStateEnterCallback(stateEnterCb), m_OnStateExitCallback(stateExitCb)
{
}

void FiniteStateMachine::SetState(State newState)
{
    if (newState == m_State)
    {
        return;
    }

    OnStateExit();
    OnStateEnter(newState);
    m_State = newState;
}

State FiniteStateMachine::GetState()
{
    return m_State;
}

void FiniteStateMachine::OnStateExit()
{
    if (m_OnStateExitCallback)
    {
        m_OnStateExitCallback(m_State);
    }
}

void FiniteStateMachine::OnStateEnter(State newState)
{
    if (m_OnStateEnterCallback)
    {
        m_OnStateEnterCallback(newState);
    }
}