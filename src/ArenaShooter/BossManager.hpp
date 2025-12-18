#ifndef BOSS_MANAGER_HPP_INCLUDED
#define BOSS_MANAGER_HPP_INCLUDED

#include "define.h"
#include "MapLoader.hpp"
#include "Script.h"
#include "Utils.h"
#include "Event.hpp"
#include "Boss.hpp"
#include "Kamikaze.hpp"
#include "Tank.hpp"
#include "Player.hpp"



DECLARE_SCRIPT(BossManager, ScriptFlag::Update)

int8 maxWaveCount = 3;
int8 floorFactor = 0;
int8 waveValue = 0;
int8 remainingWaveValue = 0;

bool bossAlreadySpawned = false;
bool isSpawningBoss = false;
bool isFightingBoss = false;

int8 currentWave = 0;
bool isSpawningWave = false;
bool isFightingWave = false;

bool kamikazeFirstPhaseStarted = false;
bool kamikazeSecondPhaseStarted = false;

Vector<Spawn> spawns;

Spawn BossSpawn = { { 35.f,1.f,0.f }, { 20.f,1.f,0.f } };
Spawn KamikazeSpawn1 = { { 25.f,4.f,0.f }, { 25.f,8.f,-4.f } };
Spawn KamikazeSpawn2 = { { 25.f,4.f,0.f }, { 25.f,8.f,4.f } };

Chrono waveSpawnChrono;
float32 waveSpawnDelay = 10.f;

Event<BossManager*> onEnemyDeathEvent;

CustomScene* currScene = nullptr;
GameObject* player = nullptr;
LevelGrid* grid = nullptr;

Enemy* boss;
Vector<Enemy*> vEnemy;

void OnInit() 
{
    Geometry* pBossGeo = GeometryFactory::LoadGeometry(RES_PATH"res/ArenaShooter/Obj/boss.obj");
    Texture* albedoBoss = new Texture(RES_PATH"res/ArenaShooter/Obj/Boss_BaseColor.png");
    Texture* roughBoss = new Texture(RES_PATH"res/ArenaShooter/Obj/Boss_Roughness.png");
    Texture* aoBoss = new Texture(RES_PATH"res/ArenaShooter/Obj/Boss_AO.png");

    Geometry* pKamikazeGeo = GeometryFactory::LoadGeometry(RES_PATH"res/ArenaShooter/Obj/Kamikaze.obj");
    Texture* albedoKamikaze = new Texture(RES_PATH"res/ArenaShooter/Obj/Kamikaze_color.png");
    Texture* roughKamikaze = new Texture(RES_PATH"res/ArenaShooter/Obj/Kamikaze_rough.png");
    Texture* aoKamikaze = new Texture(RES_PATH"res/ArenaShooter/Obj/Kamikaze_ao.png");
    
    GameObject* bossObj = &currScene->AddObject();
    MeshRenderer& mesh = *bossObj->AddComponent<MeshRenderer>();
    mesh.pGeometry = pBossGeo;
    mesh.pMaterial->albedoTextureID = albedoBoss->GetTextureID();
    mesh.pMaterial->useTextureAlbedo = 1;
    mesh.pMaterial->roughnessTextureID = roughBoss->GetTextureID();
    mesh.pMaterial->useTextureRoughness = 1;
    mesh.pMaterial->ambientTextureID = aoBoss->GetTextureID();
    mesh.pMaterial->useTextureAmbient = 1;
    bossObj->transform.SetWorldScale({ 1.f,1.f,1.f });
    
    Boss* bossScript = bossObj->AddScript<Boss>();
    bossScript->SetGrid(grid);
    bossScript->SetPlayer(player);
    bossScript->m_pCustomScene = currScene;

    bossObj->AddComponent<BoxCollider>(); 
    PhysicComponent* bossPC = bossObj->AddComponent<PhysicComponent>();
    bossPC->SetGravityScale(0.0f);
    bossPC->SetIsTrigger(true);
    bossObj->SetActive(false);

    boss = dynamic_cast<Enemy*>(bossScript);
    
    for (int i = 0; i < 10; i++)
    {
        GameObject* newEnemy = &currScene->AddObject();
        MeshRenderer& meshE = *newEnemy->AddComponent<MeshRenderer>();
        meshE.pGeometry = pKamikazeGeo;
        meshE.pMaterial->albedoTextureID = albedoKamikaze->GetTextureID();
        meshE.pMaterial->useTextureAlbedo = 1;
        meshE.pMaterial->roughnessTextureID = roughKamikaze->GetTextureID();
        meshE.pMaterial->useTextureRoughness = 1;
        meshE.pMaterial->ambientTextureID = aoKamikaze->GetTextureID();
        meshE.pMaterial->useTextureAmbient = 1;
        newEnemy->transform.SetWorldScale({ 1.3f,1.3f,1.3f });

        newEnemy->SetName("Kamikaze");
        Kamikaze* tempScript = newEnemy->AddScript<Kamikaze>();
        tempScript->SetGrid(grid);
        tempScript->SetPlayer(player);
        tempScript->m_pCustomScene = currScene;
        newEnemy->AddComponent<BoxCollider>();
        PhysicComponent* newEnemyPC = newEnemy->AddComponent<PhysicComponent>();
        newEnemyPC->SetGravityScale(0.0f);
        newEnemyPC->SetIsTrigger(true);
        newEnemy->SetActive(false);

        Enemy* enemyScript = dynamic_cast<Enemy*>(tempScript);
        vEnemy.PushBack(enemyScript);
    }
}

void OnStart()
{
    boss->m_pOwner->SetActive(false);
}

int8 GetEnemyCount()
{
    int8 count = 0;
    for (Enemy* pEnemy : vEnemy)
    {
        if (pEnemy->m_pOwner->IsActive() == true)
            count++;
    }
    return count;
}

template <typename T>
T* GetFirstAvailableEnemy()
{
    for (Enemy* pEnemy : vEnemy)
    {
        if (pEnemy->m_pOwner->IsActive() == true) continue;
        T* pResult = pEnemy->m_pOwner->GetScript<T>();
        if (pResult == nullptr) continue;

        return pResult;
    }

    return nullptr;
}

void BossSpawned()
{
    bossAlreadySpawned = true;
}

void StartBoss()
{
    isSpawningBoss = true;
}

void SpawnBoss(Spawn selectedSpawn)
{
    if (currScene == nullptr)
    {
        std::cerr << "Unvalid Parameters : Unset scene for boss creation" << std::endl;
        return;
    }

    BossSpawned();
    boss->m_pOwner->transform.SetWorldPosition(selectedSpawn.startPos);
    boss->GoToPosition(selectedSpawn.endPos, boss->m_speed);
    boss->m_pOwner->SetActive(true);
}

void StartWave()
{
    isSpawningWave = true;
}

void SpawnEnemy(Spawn selectedSpawn)
{
    if (currScene == nullptr)
    {
        std::cerr << "Unvalid Parameters : Unset scene for wave creation" << std::endl;
        return;
    }

    Kamikaze* tempScript = GetFirstAvailableEnemy<Kamikaze>();
    if (tempScript == nullptr) return;
    tempScript->m_pOwner->transform.SetWorldPosition(selectedSpawn.startPos);
    tempScript->GoToPosition(selectedSpawn.endPos, tempScript->m_speed);
    tempScript->m_pOwner->SetActive(true);
}

void SpawnSubWave()
{
    if (waveSpawnChrono.GetElapsedTime() < waveSpawnDelay)
        return;

    waveSpawnChrono.Reset();

    SpawnEnemy(KamikazeSpawn1);
    SpawnEnemy(KamikazeSpawn2);
}

void Update() override
{
    if (bossAlreadySpawned == false && isSpawningBoss == true)
    {
        SpawnBoss(BossSpawn);
        return;
    }
    else if (isSpawningBoss == true)
    {
        isSpawningBoss = false;
        isFightingBoss = true;
    }
    
    if (isFightingBoss)
    {
        if (boss->GetOwner()->IsActive() == false)
        {
            isFightingBoss = false;
            SceneManager::GetInstance()->GetCurrentScene()->GetAllScripts<Player>().Front()->Die();
        }
    }

    if (boss->m_Hp->GetHealth() <= 2000 && !kamikazeFirstPhaseStarted)
    {
        kamikazeFirstPhaseStarted = true;
    }
    if (boss->m_Hp->GetHealth() <= 1000 && !kamikazeSecondPhaseStarted)
    {
        kamikazeSecondPhaseStarted = true;
        waveSpawnChrono.Reset();
    }



    if (kamikazeFirstPhaseStarted)
    {
        SpawnSubWave();
    }

    if (isFightingBoss == false && isSpawningBoss == false)
    {
        StartBoss();
    }
    
}

END_SCRIPT

#endif