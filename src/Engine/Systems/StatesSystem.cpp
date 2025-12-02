#include "pch.h"
#include "StatesSystem.h"
#include "StateMachine/StateMachine.h"

namespace gce 
{
	//////////////////////////////////////////////////////////////////////
	/// @brief Updates all registered state machines.
	/// Iterates through the list of state machines and calls
	/// their Update() method to evaluate transition conditions
	/// and execute the current state logic.
	///
	/// @note This function is typically called once per frame
	/// to ensure real-time behavior processing of all state machines.
	//////////////////////////////////////////////////////////////////////
void StatesSystem::HandleStateMachines()
{
	for (int i = 0; i < m_stateMachineList.Size(); i++)
	{
		m_stateMachineList[i]->Update();
	}
}

}