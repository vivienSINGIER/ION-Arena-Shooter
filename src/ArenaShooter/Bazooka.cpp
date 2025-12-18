#include "Bazooka.h"
#include "Kamikaze.hpp"
#include "Tank.hpp"
#include "Drone.hpp"
#include "SceneManager.h"

void Bazooka::Start()
{ 
	Weapon::Start();

	Geometry* pGeo = GeometryFactory::LoadGeometry(RES_PATH"res/ArenaShooter/Obj/missile.obj");
	Texture* albedo = new Texture(RES_PATH"res/ArenaShooter/Obj/missile_color.png");
	Texture* rough = new Texture(RES_PATH"res/ArenaShooter/Obj/missile_rough.png");
	Texture* ao = new Texture(RES_PATH"res/ArenaShooter/Obj/missile_ao.png");
	Texture* metal = new Texture(RES_PATH"res/ArenaShooter/Obj/missile_metal.png");

	for (int i = 0; i < 2; i++) 
	{ 
		GameObject& bullet = m_pCustomScene->AddObject();
		MeshRenderer& meshProjectile = *bullet.AddComponent<MeshRenderer>();
		meshProjectile.pGeometry = pGeo;
		meshProjectile.pMaterial->albedoTextureID = albedo->GetTextureID();
		meshProjectile.pMaterial->roughnessTextureID = rough->GetTextureID();
		meshProjectile.pMaterial->ambientTextureID = ao->GetTextureID();
		meshProjectile.pMaterial->metalnessTextureID = metal->GetTextureID();
		meshProjectile.pMaterial->useTextureAlbedo = 1.f;
		meshProjectile.pMaterial->useTextureRoughness = 1.f;
		meshProjectile.pMaterial->useTextureAmbient = 1.f;
		meshProjectile.pMaterial->useTextureMetalness = 1.f;

		bullet.transform.SetWorldPosition({ 0.0f, 0.0f, 0.0f }); 
		bullet.transform.SetWorldScale({ 0.3f,0.3f,0.3f });
		bullet.SetName("Bazooka bullet");
		bullet.AddComponent<SphereCollider>();
		bullet.AddComponent<PhysicComponent>()->SetGravityScale(0.0f);
		bullet.GetComponent<PhysicComponent>()->SetIsTrigger(true);
		m_pProjectiles.PushBack(bullet.AddScript<BulletBazooka>());
	} 
} 
void Bazooka::Update()
{
	Weapon::Update();
}

bool Bazooka::Shoot()
{
	if (m_isShooting) return false; 
	
	float32 minDot = -1.0f;
	Enemy* targetEnemy = nullptr; 

	Vector<Kamikaze*> m_vectEnemyK = SceneManager::GetInstance()->GetCurrentScene()->GetAllScripts<Kamikaze>(); 
	Vector<Tank*> m_vectEnemyT = SceneManager::GetInstance()->GetCurrentScene()->GetAllScripts<Tank>(); 
	Vector<Drone*> m_vectEnemyD = SceneManager::GetInstance()->GetCurrentScene()->GetAllScripts<Drone>(); 
	Vector3f32 direction = m_pOwner->transform.GetWorldForward(); 

	for (int i = 0; i < m_vectEnemyK.Size(); i++) 
	{
		Vector3f32 toEnemy = m_vectEnemyK[i]->GetOwner()->transform.GetWorldPosition() - m_pOwner->transform.GetWorldPosition(); 
		toEnemy.SelfNormalize(); 
		float32 dot = direction.DotProduct(toEnemy); 
		if (dot > 0.6f) 
		{
			if (dot > minDot)
			{ 
				minDot = dot; 
				targetEnemy = m_vectEnemyK[i]; 
			}
		}
	} 
	for (int i = 0; i < m_vectEnemyT.Size(); i++) 
	{
		Vector3f32 toEnemy = m_vectEnemyT[i]->GetOwner()->transform.GetWorldPosition() - m_pOwner->transform.GetWorldPosition();
		toEnemy.SelfNormalize(); 
		float32 dot = direction.DotProduct(toEnemy); 
		if (dot > 0.6f) 
		{
			if (dot > minDot)
			{
				minDot = dot; 
				targetEnemy = m_vectEnemyT[i];
			}
		}
	}
	
	for (int i = 0; i < m_vectEnemyD.Size(); i++) 
	{
		Vector3f32 toEnemy = m_vectEnemyD[i]->GetOwner()->transform.GetWorldPosition() - m_pOwner->transform.GetWorldPosition(); 
		toEnemy.SelfNormalize(); 
		float32 dot = direction.DotProduct(toEnemy); 
		if (dot > 0.6f) 
		{
			if (dot > minDot) 
			{
				minDot = dot; targetEnemy = m_vectEnemyD[i]; 
			}
		}
	}


	
	Projectile* proj = GetFirstAvailableProjectile();
	BulletBazooka* bulletBazooka = dynamic_cast<BulletBazooka*>(proj); 
	if (proj == nullptr)
		return false;

	if (bulletBazooka == nullptr)
		bulletBazooka->m_target = nullptr;


	if (bulletBazooka && targetEnemy) 
		bulletBazooka->m_target = targetEnemy->GetOwner();
		bulletBazooka->Init(m_pOwner->transform.GetWorldForward(), m_pOwner->transform.GetWorldPosition() + m_pOwner->transform.GetWorldForward() * 2.2f, 20.f); 
	
	LockMyself();
	return true;
}

void Bazooka::LockMyself()
{
	if (m_pWeaponController == nullptr) return;
	m_pWeaponController->LockWeapon(m_weaponIndex);
	m_pWeaponController->EquipFirstUnlocked();
}
