#ifndef ROOM_MANAGER_HPP_INCLUDED
#define ROOM_MANAGER_HPP_INCLUDED

#include "Engine.h"
#include "define.h"
#include "Room.hpp"

DECLARE_SCRIPT(RoomManager, ScriptFlag::Update | ScriptFlag::Start)

GameObject* pPlayer = nullptr;
CustomScene* pScene = nullptr;

void Start() override
{
    if (pScene == nullptr)
        return;

    GameObject& elevator = pScene->AddObject();
    m_pElevator = elevator.AddScript<Room>();
    m_pElevator->mapPath = RES_PATH"res/Maps/WaitingRoom.json";
    m_pElevator->pPlayer = pPlayer;
    m_pElevator->pScene = pScene;

    m_vRoomPaths.PushBack(RES_PATH"res/Maps/Room1.json");
}

void Update() override
{
    if (m_pCurrRoom == nullptr)
        LoadRoom();
}

void LoadRoom()
{
    if (m_roomIndex >= m_vRoomPaths.Size())
        return;
    
    GameObject& temp = pScene->AddObject();
    m_pCurrRoom = temp.AddScript<Room>();
    m_pCurrRoom->mapPath = RES_PATH"res/Maps/WaitingRoom.json";
    m_pCurrRoom->pPlayer = pPlayer;
    m_pCurrRoom->pScene = pScene;
    m_pCurrRoom->hasLevelGrid = true;
    m_pCurrRoom->hasWaves = true;
}

private:

Room* m_pElevator = nullptr;
Room* m_pCurrRoom = nullptr;
int8 m_roomIndex = 0;

Vector<String> m_vRoomPaths;

END_SCRIPT

#endif