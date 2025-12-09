#ifndef SHOTGUN_HPP_INCLUDED
#define SHOTGUN_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Chrono.h"
#include "Weapon.hpp"
#include "BulletShotgun.hpp"

using namespace gce;

DECLARE_CHILD_SCRIPT(Shotgun, Weapon, ScriptFlag::Start | ScriptFlag::Update)

float32 m_spreadAngle = 5.f;
float32 m_verticalSpread = 5.f;
int m_numPellets = 8;        

void Start() override
{
    m_shotCooldown = 1.0f;     
    m_reloadCooldown = 2.0f;   

    MeshRenderer& meshProjectileShotgun = *m_pOwner->AddComponent<MeshRenderer>();
    meshProjectileShotgun.pGeometry = SHAPES.CYLINDER; 
}

void Shoot() override
{
    m_shotTimer.Start();
    m_heat += m_heatPerShot;
    
    for (int i = 0; i < m_numPellets; ++i)
    {
        float32 horizontalAngle = ((rand() % 2001) - 1000) / 1000.f * m_spreadAngle;
        float32 verticalAngle = ((rand() % 2001) - 1000) / 1000.f * m_verticalSpread;
        
        Vector3f32 spreadDirection = GetSpreadDirection(horizontalAngle, verticalAngle);
        
        GameObject& bullet = GameObject::Create(m_pOwner->GetScene());
        bullet.AddScript<BulletShotgun>()->Init(spreadDirection, m_pOwner->transform.GetWorldPosition(), 25.f);
    }
}

Vector3f32 GetSpreadDirection(float32 horizontalAngle, float32 verticalAngle)
{
    float32 horizontalRad = horizontalAngle * (PI / 180.f);
    float32 verticalRad = verticalAngle * (PI / 180.f);
    
    float32 cosHorizontal = cos(horizontalRad);
    float32 sinHorizontal = sin(horizontalRad);
    
    float32 cosVertical = cos(verticalRad);
    float32 sinVertical = sin(verticalRad);
    
    Vector3f32 direction;
    direction.x = cosVertical * sinHorizontal;
    direction.y = sinVertical;
    direction.z = cosVertical * cosHorizontal;
    
    return direction.Normalize();
}

END_SCRIPT

#endif // !SHOTGUN_HPP_INCLUDED
