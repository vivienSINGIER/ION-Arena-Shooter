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
#include "Drone.hpp"
#include "Tank.hpp"
#include "EnergyOrb.hpp"

using namespace gce;

class GameScene : public CustomScene
{
public:
    GameScene(Scene* pScene) : CustomScene(pScene) {}
    
    void Init() override
    {
        CustomScene::Init();

        player = &AddObject();
        player->transform.SetWorldPosition({ 0,10,0 });
        player->transform.SetWorldScale({ 1.f, 1.f, 1.f });
        player->AddComponent<BoxCollider>();
        player->AddComponent<PhysicComponent>()->SetMass(80.0f);
        player->GetComponent<PhysicComponent>()->SetBounciness(0.0f);
        player->SetName("Player");

        player->AddScript<Player>();
        player->AddScript<PlayerController>();

    }

    void Start() override
    {
        CustomScene::Start();
        player->GetScript<Player>()->m_health->Heal(5);
        player->transform.SetWorldPosition({ 0,10,0 });
        MapProperties mapProperties = MapLoader::LoadMap(RES_PATH"res/Maps/blockout.json", this);

        std::srand(timeGetTime());

        LevelGrid* grid = new LevelGrid();
        grid->Init(SceneName::GAME, mapProperties, { 4.0f, 4.0f, 4.0f });

        grid->Reset();

		GameObject* energyOrb = &AddObject();
		MeshRenderer& meshEnergyOrb = *energyOrb->AddComponent<MeshRenderer>();
		meshEnergyOrb.pGeometry = SHAPES.SPHERE;
		energyOrb->transform.SetWorldScale({ 0.5f,0.5f,0.5f });
		energyOrb->transform.SetWorldPosition({ 2.f, 5.f, 2.f });
		energyOrb->AddScript<EnergyOrb>();
		energyOrb->AddComponent<BoxCollider>();

        GameObject* energyOrb2 = &AddObject();
        MeshRenderer& meshEnergyOrb2 = *energyOrb2->AddComponent<MeshRenderer>();
        meshEnergyOrb2.pGeometry = SHAPES.SPHERE;
        energyOrb2->transform.SetWorldScale({ 0.5f,0.5f,0.5f });
        energyOrb2->transform.SetWorldPosition({ -2.f, 5.f, -2.f });
        energyOrb2->AddScript<EnergyOrb>();
        energyOrb2->AddComponent<BoxCollider>();


        GameObject& waveManager = AddObject();
        WaveManager* wManagerComponent = waveManager.AddScript<WaveManager>();
        wManagerComponent->grid = grid;
        wManagerComponent->player = player;
        wManagerComponent->currScene = this;
        wManagerComponent->spawns = mapProperties.vSpawns;
        wManagerComponent->OnInit();
        pWaveManager = wManagerComponent;
        pWaveManager->OnStart();
    }

private:
    WaveManager* pWaveManager = nullptr;
    GameObject* player = nullptr;
};


#endif // !MAINMENU_HPP_INCLUDED
