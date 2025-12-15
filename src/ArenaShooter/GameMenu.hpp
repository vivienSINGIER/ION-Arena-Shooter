#ifndef GAMEMENU_HPP_INCLUDED
#define GAMEMENU_HPP_INCLUDED

#include "define.h"
#include "Engine.h"
#include "SceneManager.h"
#include "CustomScene.h"
#include "MapLoader.hpp"
#include "Player.hpp"
#include "PlayerController.hpp"
#include "Kamikaze.hpp"
#include "LevelGrid.h"
#include "WaveManager.hpp"

using namespace gce;

void InitMenuGame(CustomScene* menu, WindowParam* windowParam, D12PipelineObject* pso)
{
    MapProperties mapProperties = MapLoader::LoadMap(RES_PATH"res/Maps/blockout.json", menu, pso);

    std::srand(timeGetTime());
    
    LevelGrid* grid = new LevelGrid();
    grid->Init(SceneName::GAME, mapProperties, {4.0f, 4.0f, 4.0f});
    
    grid->Reset();
    
    GameObject& player = menu->AddObject();
    player.transform.SetWorldPosition({ 0,10,0 });
    player.transform.SetWorldScale({ 1.f, 1.f, 1.f });
    player.AddComponent<BoxCollider>();
    player.AddComponent<PhysicComponent>()->SetMass(80.0f);
    player.GetComponent<PhysicComponent>()->SetBounciness(0.0f);
    player.SetName("Player");

    player.AddScript<Player>();
    player.AddScript<PlayerController>();
    
    GameObject& waveManager = menu->AddObject();
    WaveManager* wManagerComponent = waveManager.AddComponent<WaveManager>();
    wManagerComponent->grid = grid;
    wManagerComponent->player = &player;
    wManagerComponent->currScene = menu;
}

#endif // !MAINMENU_HPP_INCLUDED
