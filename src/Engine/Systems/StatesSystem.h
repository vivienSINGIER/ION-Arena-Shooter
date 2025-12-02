#ifndef ENGINE_STATES_SYSTEM_H_INCLUDED
#define ENGINE_STATES_SYSTEM_H_INCLUDED
#include "define.h"

namespace gce 
{

	struct StateMachine;

	class StatesSystem final
	{
	Vector<StateMachine*> m_stateMachineList;
		
	StatesSystem() = default;
	~StatesSystem() = default;

	void HandleStateMachines();

		friend class GameManager;
		friend struct StateMachine;

	};

}
#endif
