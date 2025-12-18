#ifndef CUSTOMSCENE_H_INCLUDED
#define CUSTOMSCENE_H_INCLUDED

#include "define.h"
#include "Scene.h"
#include "Containers/Vector.hpp"
#include "GameObject.h"

class CustomScene
{
public:
	CustomScene(gce::Scene* pScene);
	~CustomScene();
	gce::GameObject& AddObject();
	gce::Vector<gce::GameObject*>& GetObjects();
	template<typename T> gce::Vector<T*> GetAllScripts();
	void SetActive();
	void SetInactive();
	void Empty(int indexStart);
	virtual void Start();
	
protected:
	virtual void Init();
	
private:
	gce::Vector<gce::GameObject*> m_vObject;
	gce::Scene* m_pScene;
	bool IsActive = false;
	bool isInit = false;

	void CleanVector();
	
	friend class SceneManager;
};

template <typename T>
inline gce::Vector<T*> CustomScene::GetAllScripts()
{
	gce::Vector<T*> vScripts;
	for (gce::GameObject* pObject : m_vObject)
	{
		if (pObject->IsDestroyed())
			continue;
		if(pObject->IsActive() == false)
			continue;

		T* pScript = pObject->GetScript<T>();
		if (pScript != nullptr)
			vScripts.PushBack(pScript);
	}
	return vScripts;
}

#endif

