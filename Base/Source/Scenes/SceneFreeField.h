/******************************************************************************/
/*!
\file   SceneFreeField.h
\author Ryan Lim Rui An, Lee Sek Heng, Tan Teck Ling, Zheng Qing Ping
\par email: 150577L@mymail.nyp.edu.sg, 150629Z@mymail.nyp.edu.sg, 152063Q@mymail.nyp.edu.sg
\brief
FreeField Scene which displays the overview of Free Field 
*/
/******************************************************************************/

#ifndef _SCENE_FREE_FIELD_H
#define _SCENE_FREE_FIELD_H

#include "..\\Classes\\SceneEntity.h"
#include "..\\Systems\\Scene_System.h"
#include "GraphicsEntity.h"
#include "..\\Classes\\GameObject.h"
#include "..\\Classes\\Camera3.h"
#include "..\\Systems\\BillboardManager.h"

class SceneFreeField : public SceneEntity
{
public:
    static std::string id_;
    SceneFreeField();
    virtual ~SceneFreeField();

    virtual void Init();
    virtual void Update(float dt);
    virtual void Render();
    virtual void Exit();
    virtual bool onNotify(const std::string &theEvent);

    Camera3 *camera;

private:
	// Private Variables
	int CurrentEncounterRateBoost = 0;
	const float EncounterRatio = 0.1f;
	Vector3 PreviousPosition;
	float EncounterTimer = 0;
	float EncounterTimeCheck = 10.;
	int MaxEncounterRate = 1000;
    bool MonsterFound;

    // Shadow GPass
    void RenderPassGPass();
    void RenderPassMain();

    //Heightmap stuff
    std::vector<unsigned char> m_heightMap;
    Vector3 TerrainScale;

    // Additional Calls
    void RenderShadowCasters();
    void RenderTerrain();
    void RenderSkybox();

    const float SkyboxSize = 1000;

    BillboardManager BManager;
    GameObject* Player;

    double framerates;
    GenericEntity *theInteractiveMap;
    std::vector<GameObject*> objVec;
    std::string transitingSceneName;
};

#endif // _SCENE_FREE_FIELD_H