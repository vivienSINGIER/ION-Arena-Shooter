#ifndef BOSS_HPP_INCLUDED
#define BOSS_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Enemy.hpp"
#include "GameManager.h"
#include "StateMachine/StateMachine.h"
#include "BulletBoss.hpp"
#include "EnergyOrb.h"

using namespace gce;

DECLARE_CHILD_SCRIPT(Boss, Enemy, ScriptFlag::Awake | ScriptFlag::Start | ScriptFlag::Update | ScriptFlag::CollisionEnter)

StateMachine* m_pSm = nullptr;

bool isBlocked = false;
Chrono blockedChrono;
float32 blockedToggleTime = 3.00f;

Vector3f32 finalDir;

float32 m_shootingInterval = 3.0f;
float32 m_punchingInterval = 1.5f;
float32 m_deltaTime = 0.0f;

Vector<BulletBoss*> m_pProjectiles;

float32 m_punchUpForce = 5000.f;
float32 m_punchForce = 200000.f;

SliderComponent* pLifeBar = nullptr;

void Awake() override
{
	Enemy::Awake();
	m_speed = 2.f;
	m_pOwner->SetName("Boss");
	m_Hp = new Health<float>(3000.f);

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
	
}

void Start() override
{
	Geometry* pLaserGeo = GeometryFactory::LoadGeometry(RES_PATH"res/ArenaShooter/Obj/LaserBoss.obj");
	Texture* albedoLaser = new Texture(RES_PATH"res/ArenaShooter/Obj/Laser_BaseColor.png");
	Texture* roughLaser = new Texture(RES_PATH"res/ArenaShooter/Obj/Laser_Roughness.png");
	Texture* aoLaser = new Texture(RES_PATH"res/ArenaShooter/Obj/Laser_AO.png");

	GameObject& pBossBarObj = m_pCustomScene->AddObject();
	pLifeBar = pBossBarObj.AddComponent<SliderComponent>();
	pLifeBar->size = { 800, 60 };
	pLifeBar->min = 0.f;
	pLifeBar->max = 800.f;
	pLifeBar->interval = 1.0f;
	pLifeBar->value = 800.f;
	pLifeBar->handlePosition.x = 2000;
	pLifeBar->isHandleMoved = true;
	pLifeBar->showHandle = false;
	pLifeBar->orientation = SliderComponent::LEFT_TO_RIGHT;
	pLifeBar->sliderShape = SliderComponent::RECTANGLE;
	pLifeBar->useFillImage = TRUE;
	pLifeBar->fillImageBrush = new BitMapBrush("res/ArenaShooter/barre_de_vie_boss.png");
	pLifeBar->useBarImage = TRUE;
	pLifeBar->barImageBrush = new BitMapBrush("res/ArenaShooter/contour_barre_de_vie_boss.png");
	pLifeBar->SetPosition({ 1000, 50 });
	
	for (int i = 0; i < 10; i++)
	{
		GameObject& bullet = m_pCustomScene->AddObject();
		MeshRenderer& meshProjectile = *bullet.AddComponent<MeshRenderer>();
		meshProjectile.pGeometry = pLaserGeo;
		meshProjectile.pMaterial->albedoTextureID = albedoLaser->GetTextureID();
		meshProjectile.pMaterial->useTextureAlbedo = 1;
		meshProjectile.pMaterial->roughnessTextureID = albedoLaser->GetTextureID();
		meshProjectile.pMaterial->useTextureRoughness = 1;
		meshProjectile.pMaterial->ambientTextureID = albedoLaser->GetTextureID();
		meshProjectile.pMaterial->useTextureAmbient = 1;
		bullet.transform.SetWorldPosition({ 10.0f, 0.0f, 0.0f });
		bullet.transform.SetWorldScale({ 0.5f,5.f,0.5f });
		bullet.SetName("Boss bullet");

		bullet.AddComponent<BoxCollider>();
		PhysicComponent* pcTemp = bullet.AddComponent<PhysicComponent>();
		pcTemp->SetGravityScale(0.0f);
		pcTemp->SetIsTrigger(true);
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

	pLifeBar->handlePosition.x = 600.f + m_Hp->GetHealth() / 3000.0f * 800.0f;
	
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

void ShootPattern1(float32 sideOffset)
{
	Vector3f32 shootFromOffset = m_pOwner->transform.GetWorldPosition() + m_pOwner->transform.GetWorldForward() * 1.f;
	shootFromOffset.y += 2;
	
	Vector3f32 shotDirOffset = { m_pPlayer->transform.GetWorldPosition() - m_pOwner->transform.GetWorldPosition() };
	shotDirOffset.y = 0.f;
	shotDirOffset.SelfNormalize();
	
	Vector3f32 right = { -shotDirOffset.z, 0.f, shotDirOffset.x };
	float playerSideOffset = sideOffset;
	float playerSideOffset2 = playerSideOffset * 2.f;
	
	// Cibles d�cal�es
	Vector3f32 targetLeft = m_pPlayer->transform.GetWorldPosition() - right * playerSideOffset;
	Vector3f32 targetRight = m_pPlayer->transform.GetWorldPosition() + right * playerSideOffset;
	Vector3f32 targetLeft2 = m_pPlayer->transform.GetWorldPosition() - right * playerSideOffset2;
	Vector3f32 targetRight2 = m_pPlayer->transform.GetWorldPosition() + right * playerSideOffset2;

	// Directions finales
	Vector3f32 shotDirLeft = (targetLeft - shootFromOffset).SelfNormalize();
	shotDirLeft.y = 0.f;
	Vector3f32 shotDirRight = (targetRight - shootFromOffset).SelfNormalize();
	shotDirRight.y = 0.f;
	Vector3f32 shotDirLeft2 = (targetLeft2 - shootFromOffset).SelfNormalize();
	shotDirLeft2.y = 0.f;
	Vector3f32 shotDirRight2 = (targetRight2 - shootFromOffset).SelfNormalize();
	shotDirRight2.y = 0.f;

	for (BulletBoss* first : m_pProjectiles)
	{
		if (!first->IsActive())
		{
			Vector3f32 shootFrom = shootFromOffset;
			Vector3f32 shotDir = shotDirOffset;
			first->Init(shotDir, shootFrom, 15.f);
			break;
		}
	}

	for (BulletBoss* second : m_pProjectiles)
	{
		if (!second->IsActive())
		{
			Vector3f32 shootFrom = shootFromOffset;
			second->Init(shotDirLeft, shootFrom, 15.f);
			break;
		}
	}
	for (BulletBoss* third : m_pProjectiles)
	{
		if (!third->IsActive())
		{
			Vector3f32 shootFrom = shootFromOffset;
			third->Init(shotDirRight, shootFrom, 15.f);
			break;
		}
	}
	for (BulletBoss* fourth : m_pProjectiles)
	{
		if (!fourth->IsActive())
		{
			Vector3f32 shootFrom = shootFromOffset;
			fourth->Init(shotDirLeft2, shootFrom, 15.f);
			break;
		}
	}
	for (BulletBoss* fifth : m_pProjectiles)
	{
		if (!fifth->IsActive())
		{
			Vector3f32 shootFrom = shootFromOffset;
			fifth->Init(shotDirRight2, shootFrom, 15.f);
			break;
		}
	}
}

void OnBeginIdle() 
{
	m_deltaTime = 0.0f;
	ResetPath();
}
void OnUpdateIdle()
{
	m_deltaTime += GameManager::DeltaTime();
	if (m_deltaTime >= m_shootingInterval)
	{
		ShootPattern1(10.f);
		m_deltaTime = 0.0f;
	}
}
void OnEndIdle() {}

void OnBeginPunching()
{
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
		}
	}

}
void OnEndPunching()
{
}

END_SCRIPT





#endif