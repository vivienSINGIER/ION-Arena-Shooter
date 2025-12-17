#ifndef BOSS_HPP_INCLUDED
#define BOSS_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Enemy.hpp"
#include "GameManager.h"
#include "StateMachine/StateMachine.h"
#include "BulletBoss.hpp"
#include "EnergyOrb.hpp"

using namespace gce;

DECLARE_CHILD_SCRIPT(Boss, Enemy, ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::CollisionEnter)

StateMachine* m_pSm = nullptr;


bool isBlocked = false;
Chrono blockedChrono;
float32 blockedToggleTime = 3.00f;

Vector3f32 finalDir;

float32 m_shootingInterval = 7.0f;
float32 m_punchingInterval = 1.5f;
float32 m_deltaTime = 0.0f;

Vector<BulletBoss*> m_pProjectiles;

float32 m_punchUpForce = 5000.f;
float32 m_punchForce = 200000.f;


void Awake() override
{
	Enemy::Awake();
	m_speed = 2.f;
	m_pOwner->SetName("Boss");
	m_Hp = new Health<float>(200.f);

	m_pSm = GameManager::GetStateSystem().CreateStateMachine(m_pOwner);
	String idle = "Idle";
	m_pSm->AddAction(idle, [this]() {this->OnBeginIdle(); }, [this]() {this->OnUpdateIdle(); }, [this]() {this->OnEndIdle(); });

	String punching = "Punching";
	m_pSm->AddAction(punching, [this]() {this->OnBeginPunching(); }, [this]() {this->OnUpdatePunching(); }, [this]() {this->OnEndPunching(); });

	StateMachine::Condition closePlayerCondition = { [this]() { return this->IsPlayerClose() && this->CheckPlayer() == false; } };
	Vector<StateMachine::Condition> closePlayerConditions;
	closePlayerConditions.PushBack(closePlayerCondition);
	m_pSm->AddTransition(closePlayerConditions, idle, punching);

	StateMachine::Condition farPlayerCondition = { [this]() { return this->IsPlayerFar(); } };
	Vector<StateMachine::Condition> farPlayerConditions;
	farPlayerConditions.PushBack(farPlayerCondition);
	m_pSm->AddTransition(farPlayerConditions, punching, idle);

	/*StateMachine::Condition rayCondition = { [this]() { return this->CheckPlayer(); } };
	Vector<StateMachine::Condition> rayConditions;
	rayConditions.PushBack(rayCondition);
	m_pSm->AddTransition(rayConditions, shooting, idle);*/


	for (int i = 0; i < 3; i++)
	{
		GameObject& bullet = GameObject::Create(m_pOwner->GetScene());
		MeshRenderer& meshProjectile = *bullet.AddComponent<MeshRenderer>();
		meshProjectile.pGeometry = SHAPES.CYLINDER;
		bullet.transform.SetWorldPosition({ 10.0f, 0.0f, 0.0f });
		bullet.transform.SetWorldScale({ 2.5f,5.f,1.f });
		bullet.SetName("Boss bullet");

		bullet.AddComponent<SphereCollider>();
		bullet.AddComponent<PhysicComponent>()->SetGravityScale(0.0f);
		m_pProjectiles.PushBack(bullet.AddScript<BulletBoss>());
	}
}

void Update() override
{
	if (m_Hp->GetHealth() <= 0.f)
	{
		m_Hp->SetIsAlive(false);
		m_pOwner->SetActive(false);

		GameObject& energyOrb = GameObject::Create(m_pOwner->GetScene());
		MeshRenderer& meshEnergyOrb = *energyOrb.AddComponent<MeshRenderer>();
		meshEnergyOrb.pGeometry = SHAPES.SPHERE;
		energyOrb.transform.SetWorldScale({ 0.5f,0.5f,0.5f });
		energyOrb.transform.SetWorldPosition(m_pOwner->transform.GetWorldPosition());
		energyOrb.AddScript<EnergyOrb>();
		energyOrb.AddComponent<BoxCollider>();
		energyOrb.AddComponent<PhysicComponent>()->SetMass(1.0f);
	}

	Enemy::Update();
}


void Shoot() override
{

}

bool IsPlayerClose()
{
	GameObject* player = m_pPlayer;
	if (player == nullptr) return false;
	Vector3f32 DistVect = player->transform.GetLocalPosition() - m_pOwner->transform.GetLocalPosition();
	float distance = DistVect.Norm();
	return distance <= 15.f; // Seuil de distance
}


bool IsPlayerFar()
{
	GameObject* player = m_pPlayer;
	if (player == nullptr) return false;
	Vector3f32 DistVect = player->transform.GetLocalPosition() - m_pOwner->transform.GetLocalPosition();
	float distance = DistVect.Norm();
	return distance > 15.f; // Seuil de distance
}

bool CheckPlayer()
{
	Vector3f32 direction = m_pPlayer->transform.GetWorldPosition() - m_pOwner->transform.GetWorldPosition();
	direction.SelfNormalize();
	Ray ray;
	ray.origin = m_pOwner->transform.GetWorldPosition();
	ray.direction = direction;

	RaycastHit hitInfo;
	bool hit = PhysicSystem::IntersectRay(ray, hitInfo, 25.f);

	return hit && hitInfo.pGameObject != nullptr && hitInfo.pGameObject->GetName() != "Player" && hitInfo.pGameObject->GetName() != "Boss bullet" && hitInfo.pGameObject->GetName() != "Rifle bullet" && hitInfo.pGameObject->GetName() != "Shotgun bullet" && hitInfo.pGameObject->GetName() != "Handgun bullet" && hitInfo.pGameObject != m_pOwner;
}


void OnBeginIdle() 
{
	Console::Log("Boss Idle");
	m_deltaTime = 0.0f;
	ResetPath();
}
void OnUpdateIdle()
{
	Vector3f32 direction = m_pPlayer->transform.GetWorldPosition() - m_pOwner->transform.GetWorldPosition();
	direction.SelfNormalize();
	m_deltaTime += GameManager::DeltaTime();
	if (m_deltaTime >= m_shootingInterval)
	{
		BulletBoss* first = m_pProjectiles[0];
		if (!first->IsActive())
		{
			Vector3f32 shootFrom = m_pOwner->transform.GetWorldPosition() + m_pOwner->transform.GetWorldForward() * 1.f;
			shootFrom.x += 2.f;
			shootFrom.y += 2.f;
			Vector3f32 shotDir = m_pOwner->transform.GetWorldForward();
			first->Init(shotDir, shootFrom, 10.f);
			m_deltaTime = 0.0f;
			Console::Log("Boss Shot");
		}
		BulletBoss* second = m_pProjectiles[1];
		if (!second->IsActive())
		{
			Vector3f32 shootFrom = m_pOwner->transform.GetWorldPosition() + m_pOwner->transform.GetWorldForward() * 1.f;
			shootFrom.x += 2.f;
			shootFrom.y += 2.f; 
			shootFrom.z += 5.f;
			Vector3f32 shotDir = m_pOwner->transform.GetWorldForward();
			//shotDir.y += 15.f;
			second->Init(shotDir, shootFrom, 10.f);
			m_deltaTime = 0.0f;
			Console::Log("Boss Shot");
		}
		BulletBoss* third = m_pProjectiles[2];
		if (!third->IsActive())
		{
			Vector3f32 shootFrom = m_pOwner->transform.GetWorldPosition() + m_pOwner->transform.GetWorldForward() * 1.f;
			shootFrom.x += 2.f;
			shootFrom.y += 2.f;
			shootFrom.z -= 5.f;
			Vector3f32 shotDir = m_pOwner->transform.GetWorldForward();
			//shotDir.y -= 15.f;
			third->Init(shotDir, shootFrom, 10.f);
			m_deltaTime = 0.0f;
			Console::Log("Boss Shot");
		}
	}
}
void OnEndIdle() {}

void OnBeginPunching()
{
	Console::Log("Boss Punching");
	m_deltaTime = 0.0f;
	ResetPath();
}
void OnUpdatePunching()
{
	Vector3f32 direction = m_pPlayer->transform.GetWorldPosition() - m_pOwner->transform.GetWorldPosition();
	direction.SelfNormalize();
	direction.y = 0.f; 
	m_deltaTime += GameManager::DeltaTime();
	if (m_deltaTime >= m_punchingInterval)
	{
		Force punchUp;
		punchUp.direction = Vector3f32(0.f, 1.f, 0.f); // Add some upward force
		punchUp.norm = m_punchUpForce;
		m_pPlayer->GetComponent<PhysicComponent>()->AddForce(punchUp);

		m_pPlayer->GetScript<Player>()->GettingPunched();

		if (m_pPlayer->GetScript<Player>()->IsGrounded() == false)
		{
			Force punch;
			punch.direction = direction;
			punch.norm = m_punchForce;
			m_pPlayer->GetComponent<PhysicComponent>()->AddForce(punch);
			m_deltaTime = 0.0f;
			Console::Log("Boss Punched");
		}
	}

}
void OnEndPunching()
{
}

END_SCRIPT





#endif