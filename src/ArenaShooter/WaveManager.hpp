#ifndef WAVE_MANAGER_HPP_INCLUDED
#define WAVE_MANAGER_HPP_INCLUDED

#include "define.h"
#include "MapLoader.hpp"
#include "Script.h"
#include "Utils.h"
#include "Event.hpp"
#include "Kamikaze.hpp"

enum EnemyCost
{
    KAMIKAZE = 1,
    DRONE = 2,
    TANK = 3
};

DECLARE_SCRIPT(WaveManager, ScriptFlag::Start | ScriptFlag::Update)

int8 floorFactor = 0;
int8 waveValue = 0;
int8 remainingWaveValue = 0;

int8 currentWave = 0;
bool isSpawningWave = false;

Vector<Spawn> spawns;

int8 remainingEnemies = 0;

Chrono waveSpawnChrono;
float32 waveSpawnDelay = 0.2f;

Event<void> onEnemyDeathEvent;

CustomScene* currScene = nullptr;

void OnEnemyDeath() {remainingEnemies--;}

void Start() override
{
    onEnemyDeathEvent.AddListener(OnEnemyDeath());
}

void StartWave()
{
    currentWave++;
    waveValue = 5 + currentWave * 3 + floorFactor * 3;
    remainingWaveValue = waveValue;
}

void SpawnEnemy(Spawn selectedSpawn)
{
    if (currScene == nullptr)
    {
        std::cerr << "Unvalid Parameters : Unset scene for wave creation" << std::endl;
        return;
    }
    
    Vector<EnemyCost> options;

    if (remainingValue >= KAMIKAZE) options.PushBack(KAMIKAZE);
    if (remainingValue >= DRONE) options.PushBack(DRONE);
    if (remainingValue >= TANK) options.PushBack(TANK);

    EnemyCost chosenEnemy = RandomFrom(options);

    GameObject* newEnemy = currScene->AddObject();
    if (chosenEnemy == KAMIKAZE)
    {
        MeshRenderer& mesh = *newEnemy->AddComponent<MeshRenderer>();
        mesh.pGeometry = SHAPES.CUBE;
        newEnemy->transform.SetWorldPosition(selectedSpawn.startPos);
        newEnemy->transform.SetWorldScale({ 1.f,1.f,1.f });
        Kamikaze* tempScript = newEnemy->AddScript<Kamikaze>();
        tempScript->GoToPosition(selectedSpawn.endPos, tempScript->m_speed);
        newEnemy->AddComponent<BoxCollider>();
        PhysicComponent* newEnemyPC = newEnemy.AddComponent<PhysicComponent>();
        newEnemyPC->SetGravityScale(0.0f);
        newEnemyPC->SetIsTrigger(true);
    }
    else if (chosenEnemy == DRONE) {}
    else if (chosenEnemy == TANK) {}
    
}

void SpawnSubWave()
{
    for (Spawn currSpawn : spawns)
    {
        // TODO
        // Timer handling
    }
}

void Update() override
{
    
}

END_SCRIPT

#endif