#ifndef ENERGYORB_HPP_INCLUDED
#define ENERGYORB_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Player.hpp"

using namespace gce;

DECLARE_SCRIPT(EnergyOrb, ScriptFlag::CollisionEnter | ScriptFlag::Update)
	
float32 scale = 0.5f;

void Update() override
{
	scale -= 0.0005f;
	m_pOwner->transform.SetLocalScale({scale, scale, scale});

	if (scale < 0.001f)
		m_pOwner->Destroy();
}

void CollisionEnter(GameObject* other) override
{
	if (other->GetName() == "Player")
	{
		other->GetScript<Player>()->m_energyOrbs += 1;
		m_pOwner->Destroy();
	}
}


END_SCRIPT
#endif // !ENERGYORB_HPP_INCLUDED
