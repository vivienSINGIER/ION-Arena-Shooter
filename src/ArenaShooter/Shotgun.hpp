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

float32 m_spreadAngle = 5.f; // Angle de dispersion des projectiles (horizontal)
float32 m_verticalSpread = 5.f; // Angle de dispersion verticale (au-dessus / en-dessous)
int m_numPellets = 8;         // Nombre de projectiles tir�s

void Start() override
{
    m_shotCooldown = 1.0f;     // Cooldown entre chaque tir (plus long pour le shotgun)
    m_reloadCooldown = 2.0f;   // Temps de rechargement
}

void Shoot() override
{
    m_shotTimer.Start();
    m_heat += m_heatPerShot;

    // On tire plusieurs projectiles
    for (int i = 0; i < m_numPellets; ++i)
    {
        // Calcul d'un angle al�atoire horizontal et vertical
        float32 horizontalAngle = ((rand() % 2001) - 1000) / 1000.f * m_spreadAngle; // -15 � +15 degr�s
        float32 verticalAngle = ((rand() % 2001) - 1000) / 1000.f * m_verticalSpread; // -10 � +10 degr�s

        // Calcul de la direction de tir
        Vector3f32 spreadDirection = GetSpreadDirection(horizontalAngle, verticalAngle);

        Vector3f32 forward = m_pOwner->transform.GetWorldForward();
        Vector3f32 right = m_pOwner->transform.GetWorldRight();
        Vector3f32 up = m_pOwner->transform.GetWorldUp();
        
        Vector3f32 worldDirection = 
            right * spreadDirection.x
            + up * spreadDirection.y
            + forward * spreadDirection.z;

        // Cr�ation du projectile
        GameObject& bullet = GameObject::Create(m_pOwner->GetScene());
        bullet.AddScript<BulletShotgun>()->Init(worldDirection, m_pOwner->transform.GetWorldPosition(), 25.f, m_PSO);
    }
}

Vector3f32 GetSpreadDirection(float32 horizontalAngle, float32 verticalAngle)
{
    // Conversion des angles en radians
    float32 horizontalRad = horizontalAngle * (PI / 180.f); // Angle horizontal en radians
    float32 verticalRad = verticalAngle * (PI / 180.f);   // Angle vertical en radians

    // Direction dans le plan horizontal (XY)
    float32 cosHorizontal = cos(horizontalRad);
    float32 sinHorizontal = sin(horizontalRad);

    // Direction dans le plan vertical (XZ)
    float32 cosVertical = cos(verticalRad);
    float32 sinVertical = sin(verticalRad);

    // Calcul de la direction finale
    Vector3f32 direction;
    direction.x = cosVertical * sinHorizontal; // Projection sur X
    direction.y = sinVertical;                 // Projection sur Y (�l�vation)
    direction.z = cosVertical * cosHorizontal; // Projection sur Z

    // Retourne la direction normalis�e
    return direction.Normalize();
}

void Init(D12PipelineObject* pso) override
{
    m_PSO = pso;

    MeshRenderer& meshProjectileShotgun = *m_pOwner->AddComponent<MeshRenderer>();
    meshProjectileShotgun.pGeometry = SHAPES.CYLINDER; // On pourrait avoir un mod�le de cartouche ou autre forme
    meshProjectileShotgun.pPso = pso;
}

END_SCRIPT

#endif // !SHOTGUN_HPP_INCLUDED
