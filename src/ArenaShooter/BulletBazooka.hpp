#ifndef BULLETBAZOOKA_HPP_INCLUDED
#define BULLETBAZOOKA_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Shapes.h"
#include "Chrono.h"
#include "Projectile.hpp"

using namespace gce;

DECLARE_CHILD_SCRIPT(BulletBazooka, Projectile, ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::CollisionEnter)

void Start() override
{
	m_MaxDistance = 100.f;
	m_dmgBullet = 50.f;
	m_pOwner->SetActive(false);
}

void Update() override
{
	Projectile::Update();
}

void Init(Vector3f32 dir,Vector3f32 pos, float32 speed) override
{
	Projectile::Init(dir,pos,speed);
}

END_SCRIPT
#endif // !BULLETBAZOOKA_HPP_INCLUDED
