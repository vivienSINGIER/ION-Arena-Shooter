#ifndef BAZOOKA_H_INCLUDED
#define BAZOOKA_H_INCLUDED

#include "define.h"
#include "Script.h"
#include "Weapon.hpp"
#include "WeaponController.hpp"
#include "BulletBazooka.hpp"
#include "CustomScene.h"

using namespace gce;

struct Enemy;
struct Kamikaze;
struct Tank;
struct Drone;

DECLARE_CHILD_SCRIPT(Bazooka, Weapon, ScriptFlag::Start | ScriptFlag::Update)

int m_weaponIndex = 3;

void Start() override;
void Update() override;
bool Shoot() override;

void LockMyself();

END_SCRIPT
#endif
