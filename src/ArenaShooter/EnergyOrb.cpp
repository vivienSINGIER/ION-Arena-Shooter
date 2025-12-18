#include "EnergyOrb.h"
#include "Player.hpp"  

void EnergyOrb::Update()
{
    scale -= 0.0005f;
    m_pOwner->transform.SetLocalScale({ scale, scale, scale });

    if (scale < 0.001f)
        m_pOwner->Destroy();
}

void EnergyOrb::CollisionEnter(GameObject* other)
{
    if (other->GetName() == "Player")
    {
        Player* player = other->GetScript<Player>(); // ? OK
        if (player)
        {
            player->AddEnergyOrb();
            m_pOwner->Destroy();
        }
    }
}
