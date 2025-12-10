#ifndef ENEMY_HPP_INCLUDED
#define ENEMY_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Health.h"
#include "Projectile.hpp"
#include "StateMachine/StateMachine.h"

using namespace gce;

class Enemy;

struct EnemyStates
{
	static bool IsPlayerClose(GameObject* me)
	{
		Enemy* e = me->GetScript<Enemy>();
		if (!e || !e->m_pPlayer) return false;

		Vector3f32 DistVect = e->m_pPlayer->transform.GetLocalPosition()
			- me->transform.GetLocalPosition();
		return DistVect.Norm() < 8.f;
	}

	static bool IsPlayerVeryClose(GameObject* me)
	{
		Enemy* e = me->GetScript<Enemy>();
		if (!e || !e->m_pPlayer) return false;

		Vector3f32 DistVect = e->m_pPlayer->transform.GetLocalPosition()
			- me->transform.GetLocalPosition();
		return DistVect.Norm() < 2.f;
	}

	static bool IsPlayerFar(GameObject* me)
	{
		Enemy* e = me->GetScript<Enemy>();
		if (!e || !e->m_pPlayer) return false;

		Vector3f32 DistVect = e->m_pPlayer->transform.GetLocalPosition()
			- me->transform.GetLocalPosition();
		return DistVect.Norm() > 12.f;
	}

	static void OnBeginIdle(GameObject* me) { Console::Log("Idle"); }
	static void OnUpdateIdle(GameObject* me) {}
	static void OnEndIdle(GameObject* me) {}

	static void OnBeginChase(GameObject* me) { Console::Log("Chase"); }
	static void OnUpdateChase(GameObject* me) {}
	static void OnEndChase(GameObject* me) {}

	static void OnBeginAttack(GameObject* me) { Console::Log("Attack"); }
	static void OnUpdateAttack(GameObject* me) {}
	static void OnEndAttack(GameObject* me) {}
};



DECLARE_SCRIPT(Enemy, ScriptFlag::Awake | ScriptFlag::Update | ScriptFlag::CollisionEnter)

Health<float>* m_Hp;
StateMachine& m_sm = StateMachine::Create(*m_pOwner);
GameObject* m_pPlayer;

void Awake() override
{
	String idle = "Idle";
	m_sm.AddAction(idle, EnemyStates::OnBeginIdle, EnemyStates::OnUpdateIdle, EnemyStates::OnEndIdle);

	String chase = "Chase";
	m_sm.AddAction(chase, EnemyStates::OnBeginChase, EnemyStates::OnUpdateChase, EnemyStates::OnEndChase);

	String attack = "Attack";
	m_sm.AddAction(attack, EnemyStates::OnBeginAttack, EnemyStates::OnUpdateAttack, EnemyStates::OnEndAttack);


	StateMachine::Condition closePlayerCondition = { EnemyStates::IsPlayerClose };
	Vector<StateMachine::Condition> closePlayerConditions;
	closePlayerConditions.PushBack(closePlayerCondition);
	m_sm.AddTransition(closePlayerConditions, chase);

	StateMachine::Condition veryClosePlayerCondition = { EnemyStates::IsPlayerVeryClose };
	Vector<StateMachine::Condition> veryClosePlayerConditions;
	closePlayerConditions.PushBack(veryClosePlayerCondition);
	m_sm.AddTransition(veryClosePlayerConditions, attack);

	StateMachine::Condition farPlayerCondition = { EnemyStates::IsPlayerFar };
	Vector<StateMachine::Condition> farPlayerConditions;
	closePlayerConditions.PushBack(farPlayerCondition);
	m_sm.AddTransition(farPlayerConditions, idle);
}

void Update() override
{
	if (m_Hp->GetHealth() <= 0.f)
	{
		m_Hp->SetIsAlive(false);
	}

	if (m_Hp->GetIsAlive() == true) return;

	m_pOwner->Destroy();

}

void SetPlayer(GameObject* player)
{
	m_pPlayer = player;
}


virtual void Shoot()
{

}

void CollisionEnter(GameObject* pOther) override
{
	if(pOther->GetScript<BulletRifle>())
	{
		m_Hp->TakeDamage(pOther->GetScript<BulletRifle>()->GetDmgBullet());
		std::cout << m_Hp->GetHealth() << std::endl;
	}
	if (pOther->GetScript<BulletShotgun>())
	{
		m_Hp->TakeDamage(pOther->GetScript<BulletShotgun>()->GetDmgBullet());
		std::cout << m_Hp->GetHealth() << std::endl;
	}
}

END_SCRIPT
#endif
