#ifndef MAP_LOADER_HPP_INCLUDED
#define MAP_LOADER_HPP_INCLUDED

#include "Engine.h"
#include <fstream>
#include "Core/nlohmann.hpp"
#include "Core/Maths/Quaternion.h"

using json = nlohmann::json;
using namespace gce;

D12PipelineObject* pso = new D12PipelineObject(
    SHADERS.VERTEX,
    SHADERS.PIXEL,
    SHADERS.HULL,
    SHADERS.DOMAIN_,
    SHADERS.ROOT_SIGNATURE
    );

struct MapLoader
{
    Map<GameObject*, String> objects;
    
    void LoadMap(String const& path, Scene* pScene)
    {
        if (pScene)
        
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Impossible d'ouvrir le fichier JSON\n";
            return geometries;
        }
        json j;
        try {
            file >> j;
        }
        catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing: " << e.what() << "\n";
            return geometries;
        }

        json jObjects = j["objects"];
        for (int i = 0; i < jObjects.size(); ++i)
        {
            json currObject = jObjects[i];
            GameObject& gameObject = GameObject::Create(pScene);
            MeshRenderer& mesh = gameObject.AddComponent<MeshRenderer>();
            mesh.pGeometry = GeometryFactory::LoadJsonGeometry(currObject);
            mesh.pPso = pso;

            String texturePath = RES_PATH;
            texturePath.append("res/Textures/");
            texturePath.append(currObject["texture"]);
            Texture texture(texturePath);
            mesh.pMaterial->useTextureAlbedo = 1;
            mesh.pMaterial->albedoTextureID = texture.GetTextureID();
            mesh.pMaterial->subsurface = 0.2f;
            
            // Pos / Scale / Rot
            {
                Vector3f32 position;
                position.x = currObject["position"][0].get<float>();
                position.y = currObject["position"][2].get<float>();
                position.z = currObject["position"][1].get<float>();
                gameObject.transform.SetLocalPosition(position);

                Vector3f32 scale;
                scale.x = currObject["scale"][0].get<float>();
                scale.y = currObject["scale"][2].get<float>();
                scale.z = currObject["scale"][1].get<float>();
                gameObject.transform.SetLocalScale(scale);
            
                float32 rotX = currObject["rotation"][0].get<float>();
                float32 rotY = currObject["rotation"][2].get<float>();
                float32 rotZ = currObject["rotation"][1].get<float>();
                float32 rotW = currObject["rotation"][3].get<float>();
                Quaternion rotation(rotX, rotY, rotZ, rotW);
                gameObject.transform.SetLocalRotation()(rotation);
            }

            gameObject.AddComponent<BoxCollider>();
            gameObject.SetName(currObject["name"]);
        }
    }
};

#endif