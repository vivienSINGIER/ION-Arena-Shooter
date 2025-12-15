#ifndef GAMEOVERCONTROLLER_HPP_INCLUDED
#define GAMEOVERCONTROLLER_HPP_INCLUDED

#include "define.h"
#include "Script.h"
#include "GameObject.h"
#include "GameManager.h"
#include "InputsMethods.h"
#include "SceneManager.h"

using namespace gce;


DECLARE_SCRIPT(GameoverController, ScriptFlag::Start | ScriptFlag::Update)



void Start() override
{

}

void Update() override
{

}

static void GoToGame()
{
	SceneManager::GetInstance()->ChangeScene(GAME);
}


static void Quit()
{

}

END_SCRIPT

#endif