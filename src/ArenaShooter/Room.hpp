#ifndef ROOM_HPP_INCLUDED
#define ROOM_HPP_INCLUDED

#include "Engine.h"
#include "define.h"
#include "WaveManager.hpp"
#include "LevelGrid.h"

using namespace gce;

DECLARE_SCRIPT(Room, ScriptFlag::Update | ScriptFlag::Start)

WaveManager* pWaveManager = nullptr;
bool hasWaves = false;

LevelGrid* pLevelGrid = nullptr;
bool hasLevelGrid = false;

GameObject* pPlayer = nullptr;
CustomScene* pScene = nullptr;
String mapPath;
MapProperties roomProperties;

int8 floor = 0;

void Start() override
{
    roomProperties = MapLoader::LoadMap(mapPath, pScene, m_vObjects);

    if (roomProperties.hasPlayerSpawn == true)
        pPlayer->transform.SetWorldPosition(roomProperties.playerSpawn);
    
    if ( hasLevelGrid )
    {
        pLevelGrid = new LevelGrid();
        pLevelGrid->Init(pScene, roomProperties, {4.0f, 4.0f, 4.0f});
    }
    
    if ( hasWaves )
    {
        pWaveManager->grid = pLevelGrid;
        pWaveManager->UpdateGrid();
        pWaveManager->currScene = pScene;
        pWaveManager->spawns = roomProperties.vSpawns;
        pWaveManager->floorFactor = floor;
        pWaveManager->currentWave = 0;
    }
}

GameObject* GetFromName(String name)
{
    for (GameObject* pObject : m_vObjects)
    {
        if (name == pObject->GetName())
            return pObject;
    }
    return nullptr;
}

template <typename T>
T* GetFromScript()
{
    for (GameObject* pObject : m_vObjects)
    {
        T* casted = pObject->GetScript<T>();
        if (casted != nullptr)
            return casted;
    }
    return nullptr;
}

float32 DistanceFromRoomBorder()
{
    Vector3f32 playerPos = pPlayer->transform.GetWorldPosition();

    Vector3f32 halfSize = roomProperties.size * 0.5f;

    Vector3f32 d = playerPos - roomProperties.pos;

    Vector3f32 outside;
    outside.x = max(abs(d.x) - halfSize.x, 0.0f);
    outside.y = max(abs(d.y) - halfSize.y, 0.0f);
    outside.z = max(abs(d.z) - halfSize.z, 0.0f);

    return sqrt(
        outside.x * outside.x +
        outside.y * outside.y +
        outside.z * outside.z
    );
}

float32 DistanceFromRoomCenter()
{
    Vector3f32 playerPos = pPlayer->transform.GetWorldPosition();
    Vector3f32 d = playerPos - roomProperties.pos;

    return d.Norm();
}

void EmptyRoom()
{
    for (GameObject* pObject : m_vObjects)
        pObject->Destroy();

    m_vObjects.Clear();

    m_pOwner->Destroy();
}

void Update() override
{
    
}

private:

Vector<GameObject*> m_vObjects;

END_SCRIPT

#endif