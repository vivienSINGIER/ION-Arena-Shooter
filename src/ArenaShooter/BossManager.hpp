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

enum EnemyCost
{
    KAMIKAZE = 1,
    DRONE = 2,
    TANK = 3
};

DECLARE_SCRIPT(BossManager, ScriptFlag::Update)

int8 maxWaveCount = 3;
int8 floorFactor = 0;
int8 waveValue = 0;
int8 remainingWaveValue = 0;

bool isSpawningBoss = false;
bool isFightingBoss = false;

int8 currentWave = 0;
bool isSpawningWave = false;
bool isFightingWave = false;
Vector<Spawn> spawns;

Chrono waveSpawnChrono;
float32 waveSpawnDelay = 0.5f;

Chrono waveIntervalChrono;
float32 waveInterval = 1.5f;

Event<BossManager*> onEnemyDeathEvent;

CustomScene* currScene = nullptr;
GameObject* player = nullptr;
LevelGrid* grid = nullptr;

Enemy* boss;
Vector<Enemy*> vEnemy;

void OnInit() 
{
    GameObject* newEnemy = &currScene->AddObject();
    MeshRenderer& mesh = *newEnemy->AddComponent<MeshRenderer>();
    mesh.pGeometry = SHAPES.CUBE;
    newEnemy->transform.SetWorldScale({ 1.f,1.f,1.f });
    
    Boss* tempScript = newEnemy->AddScript<Boss>();
    tempScript->SetGrid(grid);
    tempScript->SetPlayer(player);

    newEnemy->AddComponent<BoxCollider>();
    PhysicComponent* newEnemyPC = newEnemy->AddComponent<PhysicComponent>();
    newEnemyPC->SetGravityScale(0.0f);
    newEnemyPC->SetIsTrigger(true);
    newEnemy->SetActive(false);

    boss = dynamic_cast<Enemy*>(tempScript);
}

void OnStart()
{
    boss->m_pOwner->SetActive(false);

    //waveIntervalChrono.Start();
}

/*int8 GetEnemyCount()
{
    int8 count = 0;
    for (Enemy* pEnemy : vEnemy)
    {
        if (pEnemy->m_pOwner->IsActive() == true)
            count++;
    }
    return count;
}*/

/*template <typename T>
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
}*/


/*void StartWave()
{
    currentWave++;
    waveValue = 5 + currentWave * 3 + floorFactor * 3;
    remainingWaveValue = waveValue;
    isSpawningWave = true;
}*/

void SpawnBoss(Spawn selectedSpawn)
{
    if (currScene == nullptr)
    {
        std::cerr << "Unvalid Parameters : Unset scene for boss creation" << std::endl;
        return;
    }
    
    selectedSpawn.startPos.y = 0.f;
    selectedSpawn.endPos.y = 0.f;
    boss->m_pOwner->transform.SetWorldPosition(selectedSpawn.startPos);
    boss->GoToPosition(selectedSpawn.endPos, boss->m_speed);
    boss->m_pOwner->SetActive(true);
    Console::Log("Spawned Boss");
    
}

/*void SpawnSubWave()
{
    if (waveSpawnChrono.GetElapsedTime() < waveSpawnDelay)
        return;

    waveSpawnChrono.Reset();
    
    for (Spawn currSpawn : spawns)
        SpawnEnemy(currSpawn);
}*/

void Update() override
{
    for (Spawn currSpawn : spawns)
        SpawnBoss(currSpawn);
    if (isSpawningBoss)
    {
        isSpawningBoss = false;
        isFightingBoss = true;
    }

    if (isFightingBoss)
    {
        if (boss == nullptr)
        {
            isFightingBoss = false;
            //waveIntervalChrono.Reset();
            //waveIntervalChrono.Start();
        }
    }

    /*if (isFightingBoss == false && isSpawningBoss == false)
    {
        if (waveIntervalChrono.GetElapsedTime() > waveInterval)
        {
            if (currentWave < maxWaveCount)
                StartWave();   
        }
    }*/
    
}

END_SCRIPT

#endif