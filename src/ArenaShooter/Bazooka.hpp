#ifndef BAZOOKA_HPP_INCLUDED
#define BAZOOKA_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Chrono.h"
#include "Weapon.hpp"
#include "BulletBazooka.hpp"
#include "WeaponController.hpp"

using namespace gce;

DECLARE_CHILD_SCRIPT(Bazooka, Weapon, ScriptFlag::Awake | ScriptFlag::Update)

int m_weaponIndex = 3;

void Awake() override
{
	Weapon::Awake();
}

void Update() override
{
	Weapon::Update();
}

bool Shoot() override
{
	if (!Weapon::Shoot())
		return false;
	Projectile* proj = GetFirstAvailableProjectile();
	BulletBazooka* bulletBazooka = dynamic_cast<BulletBazooka*>(proj);

	if (bulletBazooka)
		bulletBazooka->Init(m_pOwner->transform.GetWorldForward(), m_pOwner->transform.GetWorldPosition(), 1.f);

	Console::Log("Bazooka Shoot!");

    LockMyself();

	return true;
}

void LockMyself()
{
	if (m_pWeaponController == nullptr)
		return;

	m_pWeaponController->LockWeapon(m_weaponIndex);
	m_pWeaponController->EquipFirstUnlocked();
}

END_SCRIPT
#endif // !BAZOOKA_HPP_INCLUDED
