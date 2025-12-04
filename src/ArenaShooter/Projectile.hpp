#ifndef PROJECTILE_HPP_INCLUDED
#define PROJECTILE_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Chrono.h"
using namespace gce;

DECLARE_SCRIPT(Projectile, ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::CollisionEnter)

Vector3f32 m_Direction;
float32 m_Speed;
float32 m_MaxDistance;
float32 m_CurrentDistance;


void Start() override
{

}


void Update() override
{

}

void CollisionEnter(GameObject* other) override
{

}

void Init()

END_SCRIPT

#endif