#ifndef ENEMY_HPP_INCLUDED
#define ENEMY_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "Health.h"
#include "Projectile.hpp"
#include "LevelGrid.h"

using namespace gce;

struct Target 
{
	Vector3f32 position;
	float32 distance;
	bool isSet;
	bool isReached;
};

struct Path
{
	bool isLoop = false;
	int8 index = 0;
	std::vector<Vector3i32> vPositions;
};

DECLARE_SCRIPT(Enemy, ScriptFlag::Awake | ScriptFlag::Update | ScriptFlag::CollisionEnter)

Health<float>* m_Hp;
GameObject* m_pPlayer;

Target m_target;
float32 m_speed = 0.0f;
Vector3f32 m_direction;

LevelGrid* m_pLevelGrid = nullptr;

void Awake() override
{
	
}

void Update() override
{
	float32 dt = GameManager::DeltaTime();
	float32 distance = dt * m_speed;
	if (distance > m_target.distance)
		distance = m_target.distance;
	Vector3f32 translation = m_direction * distance;
	m_pOwner->transform.WorldTranslate(translation);
	
	if (m_target.isSet) 
	{
		m_target.distance = m_pOwner->transform.GetWorldPosition().Distance(m_target.position);

		if (m_target.distance <= 0.0001f)
		{
			m_pOwner->transform.SetWorldPosition(m_target.position);
			m_direction = gce::Vector3f32({ 0.f, 0.f, 0.f });
			m_target.isSet = false;
			m_target.isReached = true;
		}
	}

	if (m_pLevelGrid == nullptr)
		return;
}

void SetPlayer(GameObject* player)
{
	m_pPlayer = player;
}

virtual void Shoot()
{

}

void SetGrid(LevelGrid* pGrid) { m_pLevelGrid = pGrid; }

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

void GoToPosition(Vector3f32 const& pos, float32 speed)
{
	Vector3f32 position = m_pOwner->transform.GetWorldPosition();
	Vector3f32 direction = Vector3f32(pos.x - position.x, pos.y - position.y, pos.z - position.z);

	float32 norm = direction.Norm();
	if (norm == 0.0f) return;
	
	direction.SelfNormalize();

	if (speed > 0.0f)
		m_speed = speed;

	m_target.position = position;
	m_target.distance = position.Distance(pos);
	m_target.isSet = true;
	m_target.isReached = false;
}

Node* GetNextNode()
{
	LevelGrid* grid = m_pLevelGrid();
	if (grid == nullptr) return nullptr;
	Node* node = grid->GetNode(grid->GetTilePosition(m_pOwner->transform.GetWorldPosition()));
	
	Vector3f32 worldPos = m_pOwner->transform.GetWorldPosition();
	Vector3f32 scale = m_pOwner->transform.GetWorldScale();
	Vector3f32 nextTilePos = worldPos + Vector3f32(m_direction.x * scale.x, m_direction.y * scale.y, m_direction.z * scale.z);
	Vector3i32 tileP = grid->GetTilePosition(nextTilePos);

	Node* nextNode = grid->GetNode(tileP);
	if (node == nextNode) return nullptr;

	return nextNode;
}

END_SCRIPT


#endif
