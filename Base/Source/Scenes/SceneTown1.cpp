#include "SceneTown1.h"
#include <sstream>

#include "Scene_2.h"
#include "SceneTown2.h"
#include "SceneTown3.h"
#include "SceneFreeField.h"

#include "..\\Classes\\GameMap.h"
#include "..\\Classes\\PlayerObject.h"
#include "..\\Systems\\UI_System.h"
#include "../Misc/LoadEnemyData.h"

std::string SceneTown1::id_ = "1_Scene";

SceneTown1::SceneTown1()
	: SceneEntity()
{
	framerates = 0;
	setName(id_);
	theInteractiveMap = nullptr;
	Player = nullptr;
}

SceneTown1::~SceneTown1()
{

}

void SceneTown1::Init()
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());

	// Set Terrain Size
	TerrainScale.Set(400.f, 100.f, 400.f);

	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / Scene_System::accessing().cSS_InputManager->cIM_ScreenHeight, 0.1f, 10000.0f);
	projectionStack->LoadMatrix(perspective);

	camera.Init(Vector3(0, 5, -5), Vector3(0, 5, 0), Vector3(0, 1, 0));
	CenterPosition.Set(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth * 0.5f, Scene_System::accessing().cSS_InputManager->cIM_ScreenHeight * 0.5f, 0);

	// Initiallise Model Specific Meshes Here
	Mesh* newMesh = MeshBuilder::GenerateTerrain("Town 1", "HeightMapFiles//heightmap_Town1.raw", m_heightMap);
	newMesh->textureArray[0] = LoadTGA("Image//RockTex.tga");
	newMesh->textureArray[1] = LoadTGA("Image//GrassStoneTex.tga");
	SceneGraphics->meshList.insert(std::pair<std::string, Mesh*>(newMesh->name, newMesh));

	Application::cA_MinimumTerrainY = TerrainScale.y * ReadHeightMap(m_heightMap, camera.position.x / TerrainScale.x, camera.position.z / TerrainScale.z) + camera.PlayerHeight;
	Application::cA_CurrentTerrainY = Application::cA_MinimumTerrainY;

	theInteractiveMap = new GameMap();
	GameMap *theMap = dynamic_cast<GameMap*>(theInteractiveMap);
	theMap->setName("scene town 1 logic map");
	theMap->LoadMap("DrivenFiles//Town1Layout.csv", m_heightMap, TerrainScale, objVec, BManager);

	//<!> There can only be 1 Player
	Player = new PlayerObject();
	Player->Init("Player", 1, camera.position - Vector3(0, camera.PlayerHeight, 0), Vector3(2, 1, 2), Vector3(), camera.CurrentCameraRotation.y, Vector3(0, 1));
	std::map<std::string, Mesh*>::iterator it = SceneGraphics->meshList.find("cube");
	Player->setName("PLayer 1");
	Player->SetMesh(it->second);

	PlayerObject* PlayerPTR = dynamic_cast<PlayerObject*>(Player);
	//PlayerPTR->cameraObject = &camera;
	PlayerPTR->SetVelocity(Vector3(10.f, 0.f, 0.f));
	PlayerPTR->SetPosition(Vector3(Player->GetPosition().x, camera.PlayerHeight + TerrainScale.y * ReadHeightMap(m_heightMap, (Player->GetPosition().x / TerrainScale.x), (Player->GetPosition().z / TerrainScale.z)), Player->GetPosition().z));
	PlayerPTR->setPlayerBoundaries(objVec);
	camera.position = PlayerPTR->GetPosition();
	//<!> There can only be 1 Player

	int temp = 1;
	for (auto it : Scene_System::accessing().NM.allNPCs)
	{
		it->Init(it->getName(), 1, Vector3(temp * 20.f, 0, temp * 20.f), Vector3(10, 10, 10), Vector3(0, 0, 0), 0.f, Vector3(0, 1, 0));

		it->SetPosition(
			Vector3(it->GetPosition().x,
			TerrainScale.y * ReadHeightMap(m_heightMap, (it->GetPosition().x / TerrainScale.x), (it->GetPosition().z / TerrainScale.x)) + it->GetDimensions().y * 0.5f, it->GetPosition().z
			));

		it->SetBounds();
		objVec.push_back(it);
		temp++;
	}

	UI_Sys = new UI_System();
	UI_Sys->Init();
	InitChatUI();
}

void SceneTown1::InitChatUI()
{
	ChatLayer = new UI_Layer();
	// Name
	Vector3 DefaultPos(CenterPosition.x  * 0.25f, CenterPosition.y * 0.7f, 0);
	NPC_Name = new UI_Element(UI_Element::UI_BUTTON_B_TO_SCRN, "TFB_Button", DefaultPos, DefaultPos, Vector3(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 5, Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 20, 1), DefaultPos, "Sek Heng");
	ChatLayer->cUI_Layer.push_back(NPC_Name);

	// Text Box
	DefaultPos.Set(CenterPosition.x, CenterPosition.y * 0.3f, 0);
	NPC_TextBox = new UI_Element(UI_Element::UI_BUTTON_B_TO_SCRN, "UI_ChatBox", DefaultPos, DefaultPos, Vector3(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth * 0.95f, Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 7, 1), DefaultPos, "Text Wrapping Test Text Wrapping Test Text Wrapping Test");
	ChatLayer->cUI_Layer.push_back(NPC_TextBox);

	// Quest Buttons
	DefaultPos.Set(CenterPosition.x * 1.75f, CenterPosition.y * 0.7f, 0);
	NPC_QuestButtons.push_back(new UI_Element(UI_Element::UI_BUTTON_B_TO_SCRN, "TFB_Button", DefaultPos, DefaultPos, Vector3(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 5, Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 20, 1), DefaultPos, "Exit"));
	ChatLayer->cUI_Layer.push_back(NPC_QuestButtons.back());

	DefaultPos.Set(CenterPosition.x * 1.75f, CenterPosition.y * 1.3f, 0);
	NPC_QuestButtons.push_back(new UI_Element(UI_Element::UI_BUTTON_B_TO_SCRN, "TFB_Button", DefaultPos, DefaultPos, Vector3(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 5, Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 20, 1), DefaultPos, "Q1"));
	ChatLayer->cUI_Layer.push_back(NPC_QuestButtons.back());

	DefaultPos.Set(CenterPosition.x * 1.75f, CenterPosition.y * 1.1f, 0);
	NPC_QuestButtons.push_back(new UI_Element(UI_Element::UI_BUTTON_B_TO_SCRN, "TFB_Button", DefaultPos, DefaultPos, Vector3(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 5, Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 20, 1), DefaultPos, "Q2"));
	ChatLayer->cUI_Layer.push_back(NPC_QuestButtons.back());

	DefaultPos.Set(CenterPosition.x * 1.75f, CenterPosition.y * 0.9f, 0);
	NPC_QuestButtons.push_back(new UI_Element(UI_Element::UI_BUTTON_B_TO_SCRN, "TFB_Button", DefaultPos, DefaultPos, Vector3(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 5, Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 20, 1), DefaultPos, "Q3"));
	ChatLayer->cUI_Layer.push_back(NPC_QuestButtons.back());

	// Chat Layer Settings
	ChatLayer->LayerCenterPosition.y = -Scene_System::accessing().cSS_InputManager->cIM_ScreenHeight;
	ChatLayer->LayerOriginalPosition = 0;
	ChatLayer->LayerTargetPosition.y = -Scene_System::accessing().cSS_InputManager->cIM_ScreenHeight;

	UI_Sys->cUIS_LayerContainer.push_back(ChatLayer);
}

int SceneTown1::HandleChatUIInput(float dt)
{
	for (std::vector<UI_Element*>::iterator it = NPC_QuestButtons.begin(); it != NPC_QuestButtons.end(); ++it)
	{
		if ((*it)->Active)
		{
			(*it)->BoundsActive = true;
			bool ClickDetection = false;
			if ((*it)->BoundsActive)
			{
				(*it)->Update(dt, Scene_System::accessing().cSS_InputManager->GetMousePosition(), ClickDetection);
				if (ClickDetection)
				{
					(*it)->BoundsActive = false;
					if ((*it)->UI_Text == "Exit")
					{
						return 0;
					}
				}
			}
		}
	}
	return -1;
}

void SceneTown1::Update(float dt)
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
	SceneGraphics->Update(dt);

	//Update Camera's Minimum Possible & Current Y Pos
	Application::cA_MinimumTerrainY = TerrainScale.y * ReadHeightMap(m_heightMap, camera.position.x / TerrainScale.x, camera.position.z / TerrainScale.z) + camera.PlayerHeight;

	if (!(Application::cA_CurrentTerrainY - Application::cA_MinimumTerrainY <= Math::EPSILON && Application::cA_MinimumTerrainY - Application::cA_CurrentTerrainY <= Math::EPSILON))
	{
		float RateofChangeY = (Application::cA_CurrentTerrainY - Application::cA_MinimumTerrainY) * (float)dt * (camera.CameraCurrentWalkSpeed / 3);
		if (Application::cA_CurrentTerrainY - RateofChangeY >= Application::cA_MinimumTerrainY || Application::cA_CurrentTerrainY - RateofChangeY <= Application::cA_MinimumTerrainY)
		{
			Application::cA_CurrentTerrainY -= RateofChangeY;
		}
	}
	Vector3 Center(Scene_System::accessing().cSS_InputManager->cIM_ScreenWidth / 2, Scene_System::accessing().cSS_InputManager->cIM_ScreenHeight / 2, 0);

	framerates = 1 / dt;

	if (Scene_System::accessing().cSS_InputManager->GetKeyValue('2'))
	{
		Scene_System::accessing().SwitchScene(SceneTown2::id_);
	}
	if (Scene_System::accessing().cSS_InputManager->GetKeyValue('3'))
	{
		Scene_System::accessing().SwitchScene(SceneTown3::id_);
	}
	if (Scene_System::accessing().cSS_InputManager->GetKeyValue('4'))
	{
		Scene_System::accessing().SwitchScene(SceneFreeField::id_);
	}
	if (Scene_System::accessing().cSS_InputManager->GetKeyValue('5'))
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

	BManager.UpdateContainer(dt, camera.position);

	PlayerObject* PlayerPTR = dynamic_cast<PlayerObject*>(Player);
	PlayerPTR->Update(dt);
	PlayerPTR->SetRotationAngle(camera.CurrentCameraRotation.y);

	camera.position = PlayerPTR->GetPosition();
	camera.Update(dt);

	ChatLayer->Update((float)dt);

	for (std::vector<NPC*>::iterator it = Scene_System::accessing().NM.allNPCs.begin(); it != Scene_System::accessing().NM.allNPCs.end(); ++it)
	{
		NPC* CurrentNPC = *it;

		// Update and rotate the NPC in accordance to the player[camera]'s position.
		CurrentNPC->setTarget(camera.position);
					
		CurrentNPC->Update((float)dt);
		float DistanceCheck = (camera.position - CurrentNPC->GetPosition()).LengthSquared();
		if (DistanceCheck < CurrentNPC->GetDetectionRadiusSquared() && Scene_System::accessing().cSS_InputManager->GetKeyValue('Q') && !CurrentNPC->getInteracting())
		{
			CurrentNPC->setInteracting(true);
			NPC_Name->UI_Text = CurrentNPC->getName();
			NPC_TextBox->UI_Text = CurrentNPC->getFText();
			NPC_TextBox->WrapText();
			ChatLayer->SwapOriginalWithTarget();
			break;
		}
		// The NPC has interacted with the player successfully.
		if (CurrentNPC->getInteracting())
		{
			// Enable the mouse.
			Scene_System::accessing().cSS_InputManager->cIM_inMouseMode = true;
			camera.CameraIsLocked = true;
			
			// Set the player's target to face the NPC
			camera.target = Vector3(CurrentNPC->GetPosition().x, camera.PlayerHeight, CurrentNPC->GetPosition().z);
			camera.CurrentCameraRotation.x = 0;

			// Interacting with NPC: Check UI Key Press
			switch (HandleChatUIInput((float)dt))
			{
				case(0) : // Exit
				{
					camera.CameraIsLocked = false;
					ChatLayer->SwapOriginalWithTarget();
					CurrentNPC->setInteracting(false);
					Scene_System::accessing().cSS_InputManager->SetMousePosition(CenterPosition);
					Scene_System::accessing().cSS_InputManager->cIM_inMouseMode = false;
				}
				case(1) : // Q1
				{
					// Set Stuffs
				}
			}
			break;
		}
	}
}

void SceneTown1::RenderTerrain()
{
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
	modelStack->PushMatrix();
	modelStack->Scale(TerrainScale.x, TerrainScale.y, TerrainScale.z);
	SceneGraphics->RenderMesh("Town 1", true);
	modelStack->PopMatrix();
}

void SceneTown1::RenderShadowCasters()
{
	RenderTerrain();
	GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
	for (std::vector<Billboard*>::iterator it = BManager.BillboardContainer.begin(); it != BManager.BillboardContainer.end(); ++it)
	{
		if ((*it)->Active)
		{
			float TimeRatio = 1;
			if ((*it)->GetLifeTime() != -1)
				TimeRatio = 1.1f - (*it)->GetCurrTime() / (*it)->GetLifeTime();
			modelStack->PushMatrix();
			modelStack->Translate((*it)->GetPosition().x, (*it)->GetPosition().y, (*it)->GetPosition().z);
			modelStack->Rotate(Math::RadianToDegree(atan2(camera.position.x - (*it)->GetPosition().x, camera.position.z - (*it)->GetPosition().z)), 0, 1, 0);
			modelStack->Scale(TimeRatio * (*it)->GetDimensions().x, TimeRatio *(*it)->GetDimensions().y, TimeRatio *(*it)->GetDimensions().z);
			SceneGraphics->RenderMesh((*it)->GetMeshName(), false);
			modelStack->PopMatrix();
		}
	}

	//<!> will remove soon <!>
	for (auto it : objVec)
	{
		GameObject *the3DObject = dynamic_cast<GameObject*>(it);
		if (the3DObject && (camera.position - camera.target).Normalize().Dot(the3DObject->GetPosition().Normalized()) < 1.f)
			the3DObject->Render();
	}
	//<!> will remove soon <!>

	/*for (auto it : Scene_System::accessing().Scene_System::accessing().NM.getContainer())
	{
		it->Render();
	}*/
	Scene_System::accessing().NM.allNPCs.at(0)->Render();
}

void SceneTown1::RenderSkybox()
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

void SceneTown1::RenderPassGPass()
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

void SceneTown1::RenderPassMain()
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
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack->LoadIdentity();

	//RenderTerrain();
	//RenderSkyplane();
	RenderSkybox();
	RenderShadowCasters();

	SceneGraphics->RenderMesh("reference", false);

	SceneGraphics->SetHUD(true);

	/*for (std::vector<UI_Element*>::iterator it = UI_Sys->cUIS_LayerContainer[0]->cUI_Layer.begin(); it != UI_Sys->cUIS_LayerContainer[0]->cUI_Layer.end(); ++it)
	{
		(*it)->Render(Vector3());
	}*/

	ChatLayer->Render();

	if (Scene_System::accessing().cSS_InputManager->cIM_inMouseMode)
	{
		SceneGraphics->RenderMeshIn2D("TFB_Gem", false, 100, 100, Scene_System::accessing().cSS_InputManager->GetMousePosition().x, Scene_System::accessing().cSS_InputManager->GetMousePosition().y);
	}

	std::ostringstream ss;
	ss.str("");
	ss << "Scene 1 - FPS:" << framerates;
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 25);

	ss.str("");
	ss << "CVel:" << camera.CameraVelocity;
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 50);

	ss.str("");
	ss << "Yaw:" << 0;// camera.Yaw_Velocity;
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 75);
	//<!> Removing soon
	ss.str("");
	ss << "CPos:" << camera.position;
	ss.precision(3);
	SceneGraphics->RenderTextOnScreen("text", ss.str(), Color(0, 1, 0), 25, 25, 125);
	//<!> Removing soon

	SceneGraphics->SetHUD(false);
}

void SceneTown1::Render()
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

void SceneTown1::Exit()
{
	if (theInteractiveMap)
		delete theInteractiveMap;
	for (auto it : objVec)
	{
		NPC *theNPC = dynamic_cast<NPC*>(it);
		if (!theNPC)
			delete it;
	}
	objVec.clear();
	if (Player)
		delete Player;
    if (ChatLayer)
        delete ChatLayer;
    //if (UI_Sys)
    //    delete UI_Sys;
    //if (NPC_TextBox)
    //    delete NPC_TextBox;
    //if (NPC_Name)
    //    delete NPC_Name;
    //for (auto it : NPC_QuestButtons)
    //    delete it;
    //NPC_QuestButtons.clear();
}

bool SceneTown1::onNotify(const std::string &theEvent)
{
    if (checkWhetherTheWordInThatString("PLAYER_INFO", theEvent))
    {
        return true;
    }
    return false;
}