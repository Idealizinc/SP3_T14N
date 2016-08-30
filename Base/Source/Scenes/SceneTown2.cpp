#include "SceneTown2.h"
#include <sstream>

#include "Scene_2.h"
#include "SceneTown1.h"
#include "SceneTown3.h"
#include "SceneFreeField.h"

#include "..\\Classes\\GameMap.h"
#include "..\\Classes\\PlayerObject.h"
#include "../Misc/LoadEnemyData.h"

std::string SceneTown2::id_ = "2_Scene";

SceneTown2::SceneTown2()
	: SceneEntity()
{
	framerates = 0;
	setName(id_);
	theInteractiveMap = nullptr;
	Player = nullptr;
}

SceneTown2::~SceneTown2()
{

}

void SceneTown2::Init()
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());

	// Set Terrain Size
	TerrainScale.Set(350.f, 50.f, 350.f);

	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / Scene_System::accessing().cSS_InputManager->cIM_ScreenHeight, 0.1f, 10000.0f);
	projectionStack->LoadMatrix(perspective);

    camera = new Camera3();
	camera->Init(Vector3(0, 5, -5), Vector3(0, 5, 0), Vector3(0, 1, 0));

	// Initiallise Model Specific Meshes Here
	Mesh* newMesh = MeshBuilder::GenerateTerrain("town2", "HeightMapFiles//heightmap_Town2.raw", m_heightMap);
	newMesh->material.kAmbient.Set(0.2f, 0.2f, 0.2f);
	newMesh->textureArray[0] = LoadTGA("Image//RockTex.tga");
	newMesh->textureArray[1] = LoadTGA("Image//GrassStoneTex.tga");
	SceneGraphics->meshList.insert(std::pair<std::string, Mesh*>(newMesh->name, newMesh));

	Application::cA_MinimumTerrainY = TerrainScale.y * ReadHeightMap(m_heightMap, camera->position.x / TerrainScale.x, camera->position.z / TerrainScale.z) + camera->PlayerHeight;
	Application::cA_CurrentTerrainY = Application::cA_MinimumTerrainY;

	theInteractiveMap = new GameMap();
	GameMap *theMap = dynamic_cast<GameMap*>(theInteractiveMap);
	theMap->setName("scene town 2 logic map");
	theMap->LoadMap("DrivenFiles//Town2Layout.csv", m_heightMap, TerrainScale, objVec, BManager);

	//<!> There can only be 1 Player
	Player = new PlayerObject();
	Player->Init("Player", 1, camera->position - Vector3(0, camera->PlayerHeight, 0), Vector3(2, 1, 2), Vector3(), camera->CurrentCameraRotation.y, Vector3(0, 1));
	std::map<std::string, Mesh*>::iterator it = SceneGraphics->meshList.find("cube");
	Player->setName("PLayer 1");
	Player->SetMesh(it->second);

	PlayerObject* PlayerPTR = dynamic_cast<PlayerObject*>(Player);
	PlayerPTR->SetPosition(Vector3(Player->GetPosition().x, camera->PlayerHeight + TerrainScale.y * ReadHeightMap(m_heightMap, (Player->GetPosition().x / TerrainScale.x), (Player->GetPosition().z / TerrainScale.z)), Player->GetPosition().z));
	PlayerPTR->setPlayerBoundaries(objVec);
	camera->position = PlayerPTR->GetPosition();
	//<!> There can only be 1 Player
    transitingSceneName = "";
}

void SceneTown2::Update(float dt)
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
	SceneGraphics->Update(dt);

	//Update Camera's Minimum Possible & Current Y Pos
	Application::cA_MinimumTerrainY = TerrainScale.y * ReadHeightMap(m_heightMap, camera->position.x / TerrainScale.x, camera->position.z / TerrainScale.z) + camera->PlayerHeight;

	if (!(Application::cA_CurrentTerrainY - Application::cA_MinimumTerrainY <= Math::EPSILON && Application::cA_MinimumTerrainY - Application::cA_CurrentTerrainY <= Math::EPSILON))
	{
		float RateofChangeY = (Application::cA_CurrentTerrainY - Application::cA_MinimumTerrainY) * (float)dt * (camera->CameraCurrentWalkSpeed / 3);
		if (Application::cA_CurrentTerrainY - RateofChangeY >= Application::cA_MinimumTerrainY || Application::cA_CurrentTerrainY - RateofChangeY <= Application::cA_MinimumTerrainY)
		{
			Application::cA_CurrentTerrainY -= RateofChangeY;
		}
	}
	Vector3 Center(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 2, Scene_System::accessing().cSS_InputManager->cIM_ScreenHeight / 2, 0);

	framerates = 1 / dt;

	PlayerObject* PlayerPTR = dynamic_cast<PlayerObject*>(Player);
    if (Scene_System::accessing().whatLoadingState == Scene_System::FINISHED_LOADING || Scene_System::accessing().whatLoadingState == Scene_System::NOT_LOADING)
    {
        if (Scene_System::accessing().cSS_InputManager->GetKeyValue('1'))
        {
            Scene_System::accessing().SwitchScene(SceneTown1::id_);
        }
        if (Scene_System::accessing().cSS_InputManager->GetKeyValue('3'))
        {
            Scene_System::accessing().SwitchScene(SceneTown3::id_);
        }
        if (Scene_System::accessing().cSS_InputManager->GetKeyValue('4'))
        {
            Scene_System::accessing().SwitchScene(SceneFreeField::id_);
        }
        if (Scene_System::accessing().cSS_InputManager->GetKeyValue('4'))
        {
            Scene_System::accessing().SwitchScene(Scene_2::id_);
        }
        if (Scene_System::accessing().cSS_InputManager->GetKeyValue('9'))
        {
            Scene_System::accessing().cSS_InputManager->cIM_inMouseMode = false;
            Scene_System::accessing().cSS_InputManager->cIM_CameraPitch = 0;
            Scene_System::accessing().cSS_InputManager->cIM_CameraYaw = 0;
        }
        if (Scene_System::accessing().cSS_InputManager->GetKeyValue('0'))
        {
            Scene_System::accessing().cSS_InputManager->cIM_inMouseMode = true;
        }
	    BManager.UpdateContainer(dt, camera->position);
    }
    else {
        camera->CameraIsLocked = true;
        PlayerPTR->SetVelocity(Vector3(0, 0, 0));
    }

	PlayerPTR->Update(dt);
	PlayerPTR->SetRotationAngle(camera->CurrentCameraRotation.y);
	PlayerPTR->GetBoundary()->ResetValues();

	camera->position = PlayerPTR->GetPosition();
	camera->Update(dt);

	if (Scene_System::accessing().cSS_InputManager->GetKeyValue('R'))
	{
		PlayerPTR->SetPosition(camera->defaultPosition);
	}
    Scene_System::accessing().UpdateLoadingStuff(dt);
    if (transitingSceneName != "" && Scene_System::accessing().whatLoadingState == Scene_System::FINISHED_LOADING)
    {
        camera->CameraIsLocked = false;
        onNotify("TRANSITIONING");
    }
}

void SceneTown2::RenderTerrain()
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
	modelStack->PushMatrix();
	modelStack->Scale(TerrainScale.x, TerrainScale.y, TerrainScale.z);
	SceneGraphics->RenderMesh("town2", true);
	modelStack->PopMatrix();
}

void SceneTown2::RenderShadowCasters()
{
	RenderTerrain();
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());

	//<!> will remove soon <!>
	for (auto it : objVec)
	{
		GameObject *the3DObject = dynamic_cast<GameObject*>(it);
		if (the3DObject && (camera->position - camera->target).Normalize().Dot(the3DObject->GetPosition().Normalized()) < 1.f)
			the3DObject->Render();
	}
	//<!> will remove soon <!>

	for (std::vector<Billboard*>::iterator it = BManager.BillboardContainer.begin(); it != BManager.BillboardContainer.end(); ++it)
	{
		if ((*it)->Active)
		{
			float TimeRatio = 1;
			if ((*it)->GetLifeTime() != -1)
				TimeRatio = 1.1f - (*it)->GetCurrTime() / (*it)->GetLifeTime();
			modelStack->PushMatrix();
			modelStack->Translate((*it)->GetPosition().x, (*it)->GetPosition().y, (*it)->GetPosition().z);
			modelStack->Rotate(Math::RadianToDegree(atan2(camera->position.x - (*it)->GetPosition().x, camera->position.z - (*it)->GetPosition().z)), 0, 1, 0);
			modelStack->Scale(TimeRatio * (*it)->GetDimensions().x, TimeRatio *(*it)->GetDimensions().y, TimeRatio *(*it)->GetDimensions().z);
			if ((*it)->GetLifeTime() == -1)
				SceneGraphics->RenderMesh((*it)->GetMeshName(), true);
			else SceneGraphics->RenderMesh((*it)->GetMeshName(), false);
			modelStack->PopMatrix();
		}
	}

}

void SceneTown2::RenderSkybox()
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
	//left
	modelStack->PushMatrix();
	modelStack->Rotate(90, 0, 1, 0);
	modelStack->PushMatrix();
	modelStack->Rotate(90, 0, 1, 0);
	modelStack->Translate(0, 0, -SkyboxSize / 2 + 2.f);
	modelStack->Scale(SkyboxSize, SkyboxSize, SkyboxSize);
	SceneGraphics->RenderMesh("SB_Left", false);
	modelStack->PopMatrix();

	modelStack->PushMatrix();
	modelStack->Rotate(-90, 0, 1, 0);
	modelStack->Translate(0, 0, -SkyboxSize / 2 + 2.f);
	modelStack->Scale(SkyboxSize, SkyboxSize, SkyboxSize);
	SceneGraphics->RenderMesh("SB_Right", false);
	modelStack->PopMatrix();

	modelStack->PushMatrix();
	modelStack->Translate(0, 0, -SkyboxSize / 2 + 2.f);
	modelStack->Scale(SkyboxSize, SkyboxSize, SkyboxSize);
	SceneGraphics->RenderMesh("SB_Front", false);
	modelStack->PopMatrix();

	modelStack->PushMatrix();
	modelStack->Rotate(180, 0, 1, 0);
	modelStack->Translate(0, 0, -SkyboxSize / 2 + 2.f);
	modelStack->Scale(SkyboxSize, SkyboxSize, SkyboxSize);
	SceneGraphics->RenderMesh("SB_Back", false);
	modelStack->PopMatrix();

	modelStack->PushMatrix();
	modelStack->Rotate(90, 1, 0, 0);
	modelStack->Translate(0, 0, -SkyboxSize / 2 + 2.f);
	modelStack->Rotate(90, 0, 0, 1);
	modelStack->Scale(SkyboxSize, SkyboxSize, SkyboxSize);
	SceneGraphics->RenderMesh("SB_Top", false);
	modelStack->PopMatrix();

	modelStack->PushMatrix();
	modelStack->Rotate(-90, 1, 0, 0);
	modelStack->Translate(0, 0, -SkyboxSize / 2 + 2.f);
	modelStack->Rotate(-90, 0, 0, 1);
	modelStack->Scale(SkyboxSize, SkyboxSize, SkyboxSize);
	SceneGraphics->RenderMesh("SB_Bottom", false);
	modelStack->PopMatrix();
	modelStack->PopMatrix();

}

void SceneTown2::RenderPassGPass()
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
	SceneGraphics->m_renderPass = GraphicsEntity::RENDER_PASS::RENDER_PASS_PRE;
	SceneGraphics->m_lightDepthFBO.BindForWriting();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(SceneGraphics->m_gPassShaderID);
	//These matrices should change when light position or direction changes
	if (SceneGraphics->lights[0].type == Light::LIGHT_DIRECTIONAL)
	{
		// based on scene size [below]
		SceneGraphics->m_lightDepthProj.SetToOrtho(-1200, 1200, -1200, 1200, 0, 4000);
	}
	else
	{
		SceneGraphics->m_lightDepthProj.SetToPerspective(90, 1.f, 0.1, 20);
	}
	SceneGraphics->m_lightDepthView.SetToLookAt(SceneGraphics->lights[0].position.x, SceneGraphics->lights[0].position.y, SceneGraphics->lights[0].position.z, 0, 0, 0, 0, 1, 0);
	// Things that the light sees and creates shadows from.
	RenderShadowCasters();
}

void SceneTown2::RenderPassMain()
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
	SceneGraphics->m_renderPass = GraphicsEntity::RENDER_PASS::RENDER_PASS_MAIN;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLsizei)Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth, (GLsizei)Scene_System::accessing().cSS_InputManager->cIM_ScreenHeight);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(SceneGraphics->m_programID);
	//pass light depth texture
	SceneGraphics->m_lightDepthFBO.BindForReading(GL_TEXTURE8);
	glUniform1i(SceneGraphics->m_parameters[SceneGraphics->U_SHADOW_MAP], 8);

	if (SceneGraphics->lights[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(SceneGraphics->lights[0].position.x, SceneGraphics->lights[0].position.y, SceneGraphics->lights[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack->Top() * lightDir;
		glUniform3fv(SceneGraphics->m_parameters[SceneGraphics->U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (SceneGraphics->lights[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack->Top() * SceneGraphics->lights[0].position;
		glUniform3fv(SceneGraphics->m_parameters[SceneGraphics->U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack->Top() * SceneGraphics->lights[0].spotDirection;
		glUniform3fv(SceneGraphics->m_parameters[SceneGraphics->U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack->Top() * SceneGraphics->lights[0].position;
		glUniform3fv(SceneGraphics->m_parameters[SceneGraphics->U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}
	SceneGraphics->SetHUD(false);

	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	//perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
	projectionStack->LoadMatrix(perspective);

	// Camera matrix
	viewStack->LoadIdentity();
	viewStack->LookAt(
		camera->position.x, camera->position.y, camera->position.z,
		camera->target.x, camera->target.y, camera->target.z,
		camera->up.x, camera->up.y, camera->up.z
		);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack->LoadIdentity();

	//RenderTerrain();
	//RenderSkyplane();
	RenderSkybox();
	RenderShadowCasters();

	SceneGraphics->RenderMesh("reference", false);

	SceneGraphics->SetHUD(true);
	Scene_System::accessing().cSS_PlayerUIManager->Render();

    if (Scene_System::accessing().theLoadingEffect)
        Scene_System::accessing().RenderLoadingStuff();
    std::ostringstream ss;
	ss.str("");
	ss << "Scene 1 - FPS:" << framerates;
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 25);

	ss.str("");
	ss << "CVel:" << camera->CameraVelocity;
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 50);

	ss.str("");
	ss << "Mouse Position:" << Scene_System::accessing().cSS_InputManager->GetMousePosition();
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 75);
	//<!> Removing soon
	ss.str("");
	ss << "CPos:" << camera->position;
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 125);
	//<!> Removing soon

	ss.str("9, 0 - Toggle Mouse Modes");
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 75);
	
	SceneGraphics->SetHUD(false);
}

void SceneTown2::Render()
{
	//*********************************
	//		PRE RENDER PASS
	//*********************************
	RenderPassGPass();
	//*********************************
	//		MAIN RENDER PASS
	//*********************************
	RenderPassMain();
}

void SceneTown2::Exit()
{
	if (theInteractiveMap)
		delete theInteractiveMap;
	for (auto it : objVec)
	{
		if (it)
			delete it;
	}
	if (Player)
		delete Player;
    if (camera)
        delete camera;
}

bool SceneTown2::onNotify(const std::string &theEvent)
{
    if (checkWhetherTheWordInThatString("PLAYER_INFO", theEvent))
    {
        if (Scene_System::accessing().gPlayer->CurrCamera)
        {
            delete camera;
            camera = Scene_System::accessing().gPlayer->CurrCamera;
        }
        if (Scene_System::accessing().gPlayer->PlayerObj)
        {
            delete Player;
            Player = Scene_System::accessing().gPlayer->PlayerObj;
            PlayerObject *PlayerPTR = dynamic_cast<PlayerObject*>(Player);
            PlayerPTR->SetPosition(Vector3(Player->GetPosition().x, camera->PlayerHeight + TerrainScale.y * ReadHeightMap(m_heightMap, (Player->GetPosition().x / TerrainScale.x), (Player->GetPosition().z / TerrainScale.z)), Player->GetPosition().z));
            PlayerPTR->setPlayerBoundaries(objVec);
        }
        return true;
    }
    else if (checkWhetherTheWordInThatString("TRANSITIONING", theEvent))
    {
        Scene_System::accessing().SwitchScene(transitingSceneName);
        std::ostringstream ss;
        ss << "SWITCHING_" << id_;
        transitingSceneName = "";
        Scene_System::accessing().getCurrScene().onNotify(ss.str());
        return true;
    }
    else if (checkWhetherTheWordInThatString("LOADING", theEvent))
    {
        Scene_System::accessing().SetLoadingTime(3.0);
        size_t posOfUnderScore = theEvent.find_first_of('_');
        transitingSceneName = theEvent.substr(posOfUnderScore + 1);
        return true;
    }
    else if (checkWhetherTheWordInThatString("SWITCHING", theEvent))
    {
        PlayerObject *PlayerPTR = Scene_System::accessing().gPlayer->PlayerObj = dynamic_cast<PlayerObject*>(Player);
        PlayerPTR->SetVelocity(Vector3(0, 0, 0));
        Scene_System::accessing().gPlayer->CurrCamera = camera;
        size_t posOfUnderScore = theEvent.find_first_of('_');
        std::string preSceneId = theEvent.substr(posOfUnderScore + 1);
        for (std::vector<GameObject*>::iterator it = objVec.begin(), end = objVec.end(); it != end; ++it)
        {
            if (checkWhetherTheWordInThatString(preSceneId, (*it)->getName()))
            {
                Vector3 theGatePos = (*it)->GetPosition();
                Vector3 theDirectionalPosBetweenPlayerGate = (PlayerPTR->GetPosition() - theGatePos).Normalize();
                theDirectionalPosBetweenPlayerGate *= (((*it)->GetDimensions().x + (*it)->GetDimensions().y) * 0.5f);
                PlayerPTR->SetPosition(theGatePos + theDirectionalPosBetweenPlayerGate);
                break;
            }
        }
        Scene_System::accessing().gPlayer->currSceneID = id_;
        return true;
    }
    return false;
}