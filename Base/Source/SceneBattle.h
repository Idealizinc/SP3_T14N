#pragma once

#include "SceneEntity.h"
#include "Scene_System.h"
#include "GraphicsEntity.h"
#include "GameObject.h"
#include "Camera3.h"
#include "BillboardManager.h"

class SceneBattle : public SceneEntity
{
public:
    static std::string id_;
    SceneBattle();
    virtual ~SceneBattle();

    virtual void Init();
    virtual void Update(float dt);
    virtual void Render();
    virtual void Exit();

    Camera3 camera;

private:
    // Shadow GPass
    void RenderPassGPass();
    void RenderPassMain();

    // Additional Calls
    void RenderShadowCasters();

    BillboardManager BManager;

    double framerates;
    GenericEntity *theInteractiveMap;
    std::vector<GameObject*> objVec;
};