#include "GateBoundary.h"
#include "..//Systems/Scene_System.h"
#include "..//Scenes/GraphicsEntity.h"

GateBoundary::GateBoundary()
{
	questToCheck = nullptr;
	stageToCheck = nullptr;
}

GateBoundary::~GateBoundary()
{
}

float GateBoundary::GetOverlappingDistance()const
{
	return this->OverlappingDistance * 75.f;
}

bool GateBoundary::CheckCollision(const Vector3 &point)
{
	SetOverlappingDistance(0.f);
	SetOverlappingAxis(Vector3(0, 0, 0));
	float overlap = 9999.f;
	Vector3 overlappedAxis;
	for (int i = 0; i < VerticeNo; i++)
	{
		Projection point1 = SetProjection(this->Axis[i], this->Vertices);
		Projection point2 = SetProjectionPoint(this->Axis[i], point);
		if (!point1.DetermineCollision(point2))
			return false;
		else
		{
			float value = point1.GetOverlappingDistance(point2);
			if (overlap > value)
			{
				overlap = value;
				overlappedAxis = this->Axis[i];
			}
		}
	}
	SetOverlappingDistance(overlap);
	SetOverlappingAxis(overlappedAxis);
	
	QuestCleared = CheckQuest();

	if (BossGate && !QuestCleared)
	{
		std::ostringstream ss;
		ss << "BOSSMONSTER_" << name_;
		Scene_System::accessing().getCurrScene().onNotify(ss.str());
	}
	else if (!BossGate || QuestCleared)
	{
		std::ostringstream ss;
		ss << "LOADING_" << name_;
		Scene_System::accessing().getCurrScene().onNotify(ss.str());
	}
	else if (!questToCheck && !stageToCheck)
	{
		std::ostringstream ss;
		ss << "LOADING_" << name_;
		Scene_System::accessing().getCurrScene().onNotify(ss.str());
	}
	return true;
}

void GateBoundary::TransitScene(const std::string &ID)
{
	Scene_System::accessing().SwitchScene(ID);
    Scene_System::accessing().getCurrScene().onNotify("TRANSITIONING");
}

void GateBoundary::SetQuestData(const std::string &name, const int &stage)
{
	questStage = stage;
	questName = name;
	InitQuest();
}

void GateBoundary::InitQuest()
{
	for (std::vector<Quest*>::iterator CurrQuest = Scene_System::accessing().QM.allQuests.begin(); CurrQuest != Scene_System::accessing().QM.allQuests.end(); ++CurrQuest)
	{
		std::string Text = (*CurrQuest)->getName();
		if (Text == questName)
		{
			questToCheck = *CurrQuest;
			break;
		}
	}
	for (std::vector<QuestStage*>::iterator CurrQuestStage = questToCheck->qStages.begin(); CurrQuestStage != questToCheck->qStages.end(); ++CurrQuestStage)
	{
		int stage = (*CurrQuestStage)->getStageNO();
		if (stage == questStage)
		{
			stageToCheck = *CurrQuestStage;
			break;
		}
	}
}

bool GateBoundary::CheckQuest()
{
	if (questToCheck && stageToCheck)
	{
		return (stageToCheck->getComplete() && stageToCheck->getStageNO() < questToCheck->getCurrentStage());
	}
	return false;
}

void GateBoundary::SetBossGate(const bool &state)
{
	this->BossGate = state;
}

void GateBoundary::SetBossOnly(const bool &state)
{
	this->BossOnly = state;
}

void GateBoundary::Render()
{	
	if (Active && Visible)
	{
		QuestCleared = CheckQuest();
		GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
		if (QuestCleared || !BossGate)
		{
		Scene_System::accessing().getCurrScene().modelStack->PushMatrix();
		Scene_System::accessing().getCurrScene().modelStack->Translate(GetPosition().x, GetPosition().y, GetPosition().z);
		Scene_System::accessing().getCurrScene().modelStack->Rotate(GetRotationAngle() - 90.f, GetRotationAxis().x, GetRotationAxis().y, GetRotationAxis().z);
		Scene_System::accessing().getCurrScene().modelStack->Scale(GetDimensions().x, GetDimensions().y, GetDimensions().z);

			SceneGraphics->RenderMesh(*this->GetMesh(), true);
		}
		else if (!QuestCleared || BossOnly)
		{
			Scene_System::accessing().getCurrScene().modelStack->PushMatrix();
			Scene_System::accessing().getCurrScene().modelStack->Translate(GetPosition().x, GetPosition().y, GetPosition().z);
			Scene_System::accessing().getCurrScene().modelStack->Rotate(GetRotationAngle(), GetRotationAxis().x, GetRotationAxis().y, GetRotationAxis().z);
			Scene_System::accessing().getCurrScene().modelStack->Scale(GetDimensions().z, GetDimensions().y, GetDimensions().x);

			Mesh* temp = Scene_System::accessing().EnemyData.find(EnemyID)->second->EnemyMesh;
			Scene_System::accessing().EnemyData.find(EnemyID);
			SceneGraphics->RenderMesh(*(Scene_System::accessing().EnemyData.find(EnemyID)->second->EnemyMesh), true);
		}
			
		Scene_System::accessing().getCurrScene().modelStack->PopMatrix();
	}
}