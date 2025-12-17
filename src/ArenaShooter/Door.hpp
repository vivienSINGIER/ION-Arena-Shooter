#ifndef DOOR_HPP_INCLUDED
#define DOOR_HPP_INCLUDED

#include "Engine.h"
#include "define.h"
#include "Tweens.hpp"

using namespace gce;

DECLARE_SCRIPT(Door, ScriptFlag::Start)

bool isClosed = false;

Tween<Vector3f32>* pUpTween = nullptr;
Tween<Vector3f32>* pDownTween = nullptr;

void Start() override
{
    Vector3f32 pos = m_pOwner->transform.GetWorldPosition();
    float32 upPos = m_pOwner->transform.GetWorldPosition().y + m_pOwner->transform.GetWorldScale().y * 2.0f;
    
    pUpTween = &TweenSystem::Create<Vector3f32>( { pos.x, pos.y, pos.z }, { pos.x, upPos, pos.z }, Tweens::EaseInOut );
    pDownTween = &TweenSystem::Create<Vector3f32>( { pos.x, upPos, pos.z }, { pos.x, pos.y, pos.z }, Tweens::EaseInOut );
}

void Toggle()
{
    if (isClosed == false)
        pUpTween->StartDuration(1.0f, Function<void(GameTransform::*)(Vector3f32 const&)>( &GameTransform::SetWorldPosition, &m_pOwner->transform ) );
    else
        pDownTween->StartDuration(1.0f, Function<void(GameTransform::*)(Vector3f32 const&)>( &GameTransform::SetWorldPosition, &m_pOwner->transform ) );

    isClosed ^= true;
}

END_SCRIPT

#endif