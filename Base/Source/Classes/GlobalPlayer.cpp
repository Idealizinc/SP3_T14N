#include "GlobalPlayer.h"
#include <fstream>
#include <sstream>
#include "../Misc/LoadEnemyData.h"

// Constructor
GlobalPlayer::GlobalPlayer()
{
	Init(0, 1, 1, false);
}

GlobalPlayer::GlobalPlayer(const int& Spell_Power, const int& CurrentHealth, const int& MaxHealth, const bool& IsInteracting)
{
	Init(Spell_Power, CurrentHealth, MaxHealth, IsInteracting);
}

// Destructor
GlobalPlayer::~GlobalPlayer()
{
	Exit();
}

// Main
void GlobalPlayer::Init(const int& Spell_Power, const int& CurrentHealth, const int& MaxHealth, const bool& IsInteracting)
{
    CurrCamera = nullptr;
    PlayerObj = nullptr;
    currSceneID = "";
	this->Spell_Power = Spell_Power;
	this->CurrentHealth = CurrentHealth;
	this->MaxHealth = MaxHealth;
	this->IsInteracting = IsInteracting;
    whatSave = 0;
	this->MonsterCount = 0;
}

void GlobalPlayer::Update(float dt)
{
	// DO STUFF
}

void GlobalPlayer::Exit()
{
	// SAVE STATS
    //if (whatSave > 0)
    //{
    //    std::ostringstream ss;
    //    ss << "DrivenFiles//PlayerSave" << whatSave << ".csv";
    //    RewritePlayerSave(ss.str());
    //}
    //else
    //    RewritePlayerSave("DrivenFiles//PlayerSave1.csv");
	// CLEAN UP

}

// Getters
int GlobalPlayer::GetSpellPower()
{
	return Spell_Power;
}

int GlobalPlayer::GetCurrentHealth()
{
	return CurrentHealth;
}

int GlobalPlayer::GetMaxHealth()
{
	return MaxHealth;
}

bool GlobalPlayer::GetIsInteracting()
{
	return IsInteracting;
}
int GlobalPlayer::getMonsterCount()
{
	return MonsterCount;
}

// Setters
void GlobalPlayer::SetSpellPower(const int& Spell_Power)
{
	this->Spell_Power = Spell_Power;
}

void GlobalPlayer::SetCurrentHealth(const int& CurrentHealth)
{
	this->CurrentHealth = CurrentHealth;
}

void GlobalPlayer::SetMaxHealth(const int& MaxHealth)
{
	this->MaxHealth = MaxHealth;
}

void GlobalPlayer::SetIsInteracting(const bool& IsInteracting)
{
	this->IsInteracting = IsInteracting;
}

void GlobalPlayer::setMonsterCount(const int& i)
{
	this->MonsterCount = i;
}

bool GlobalPlayer::LoadPlayerSave(const std::string &fileName)
{
    std::ifstream file(fileName.c_str());
    if (file.is_open())
    {
        std::string data = "";
        CurrCamera = new Camera3();
        CurrCamera->Init(Vector3(0, 0, 0), Vector3(0, 0, 10), Vector3(0, 1, 0));
        PlayerObj = new PlayerObject();
        PlayerObj->Init("Player", 1, CurrCamera->position - Vector3(0, CurrCamera->PlayerHeight, 0), Vector3(2, 1, 2), Vector3(), CurrCamera->CurrentCameraRotation.y, Vector3(0, 1));
        while (getline(file, data))
        {
            if (data == "")
                continue;
            size_t posOfComman = data.find_first_of(',');
            std::string key = data.substr(0, posOfComman);
            convertStringToUpperCaps(key);
            std::string value = data.substr(posOfComman + 1);
            if (checkWhetherTheWordInThatString("QUESTS", key) == false)
            {
                size_t OtherposOf_Comma = value.find_first_of(',');
                value = value.substr(0, OtherposOf_Comma);
            }
            if (checkWhetherTheWordInThatString("SPELLPOWER", key))
            {
                SetSpellPower(stoi(value));
            }
            else if (checkWhetherTheWordInThatString("CURRENTHEALTH", key))
            {
                SetCurrentHealth(stoi(value));
            }
            else if (checkWhetherTheWordInThatString("MAXHEALTH", key))
            {
                SetMaxHealth(stoi(value));
            }
            else if (checkWhetherTheWordInThatString("SCENENAME", key))
            {
                Scene_System::accessing().SwitchScene(value);
                currSceneID = value;
            }
            else if (checkWhetherTheWordInThatString("CAMERAPOSITIONX", key))
            {
                CurrCamera->defaultPosition.x = CurrCamera->position.x = stof(value);
            }
            else if (checkWhetherTheWordInThatString("CAMERAPOSITIONY", key))
            {
                CurrCamera->defaultPosition.y = CurrCamera->position.y = stof(value);
            }
            else if (checkWhetherTheWordInThatString("CAMERAPOSITIONZ", key))
            {
                CurrCamera->defaultPosition.z = CurrCamera->position.z = stof(value);
            }
            else if (checkWhetherTheWordInThatString("CAMERATARGETX", key))
            {
                CurrCamera->defaultTarget.x = CurrCamera->target.x = stof(value);
            }
            else if (checkWhetherTheWordInThatString("CAMERATARGETY", key))
            {
                CurrCamera->defaultTarget.y = CurrCamera->target.y = stof(value);
            }
            else if (checkWhetherTheWordInThatString("CAMERATARGETZ", key))
            {
                CurrCamera->defaultTarget.z = CurrCamera->target.z = stof(value);
            }
            else if (checkWhetherTheWordInThatString("PLAYERMASS", key))
            {
                PlayerObj->SetMass(stof(value));
            }
            else if (checkWhetherTheWordInThatString("PLAYERPOSX", key))
            {
                PlayerObj->SetPosition(Vector3(stof(value), PlayerObj->GetPosition().y, PlayerObj->GetPosition().z));
            }
            else if (checkWhetherTheWordInThatString("PLAYERPOSY", key))
            {
                PlayerObj->SetPosition(Vector3(PlayerObj->GetPosition().x,stof(value) , PlayerObj->GetPosition().z));
            }
            else if (checkWhetherTheWordInThatString("PLAYERPOSZ", key))
            {
                PlayerObj->SetPosition(Vector3(PlayerObj->GetPosition().x, PlayerObj->GetPosition().y, stof(value)));
            }
            else if (checkWhetherTheWordInThatString("PLAYERYROTATION", key))
            {
                PlayerObj->SetRotationAngle(stof(value));
            }
            else if (checkWhetherTheWordInThatString("QUESTS", key))
            {
                std::map<std::string, int>::iterator it = playerCurrQState.begin();
                std::istringstream iss(value);
                std::string valuesOfQuests = "";

				std::vector<Quest*>::iterator it2 = Scene_System::accessing().QM.allQuests.begin();
				while (getline(iss, valuesOfQuests, ','))
                {
					Quest* temp = (Quest*)*it2;
					temp->setCurrStage(stoi(valuesOfQuests));
                    it->second = stoi(valuesOfQuests);
                    ++it;
					++it2;
                }
				Scene_System::accessing().cSS_PlayerUIManager->UpdateQuestsMenu(0);
            }
        }
        file.close();
        Scene_System::accessing().getCurrScene().onNotify("PLAYER_INFO");
        Scene_System::accessing().cSS_PlayerUIManager->UpdateStats(0);
        return true;
    }
    return false;
}

bool GlobalPlayer::RewritePlayerSave(const std::string &fileName)
{
    std::ifstream file(fileName.c_str());
    if (file.is_open())
    {
        std::vector<std::string> allThoseLines;
        std::string data = "";
        while (getline(file, data))
            allThoseLines.push_back(data);
        file.close();

        std::ofstream writeFile(fileName.c_str());
        for (std::vector<std::string>::iterator it = allThoseLines.begin(), end = allThoseLines.end(); it != end; ++it)
        {
            std::string thatSpecificLine = (*it);
            size_t posOfComman = thatSpecificLine.find_first_of(',');
            std::string key = (*it).substr(0, posOfComman + 1);
            convertStringToUpperCaps(thatSpecificLine);
            std::ostringstream ss;
            if (checkWhetherTheWordInThatString("SPELLPOWER", thatSpecificLine))
            {
                ss << key << Spell_Power;
                writeFile << ss.str() << std::endl;
            }
            else if (checkWhetherTheWordInThatString("CURRENTHEALTH", thatSpecificLine))
            {
                ss << key << CurrentHealth;
                writeFile << ss.str() << std::endl;
            }
            else if (checkWhetherTheWordInThatString("MAXHEALTH", thatSpecificLine))
            {
                ss << key << MaxHealth;
                writeFile << ss.str() << std::endl;
            }
            else if (checkWhetherTheWordInThatString("SCENENAME", thatSpecificLine) && currSceneID != "")
            {
                ss << key << currSceneID;
                writeFile << ss.str() << std::endl;
            }
            else if (checkWhetherTheWordInThatString("CAMERA", thatSpecificLine) && CurrCamera)
            {
                if (checkWhetherTheWordInThatString("CAMERAPOSITIONX", thatSpecificLine))
                {
                    ss << key << CurrCamera->position.x;
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("CAMERAPOSITIONY", thatSpecificLine))
                {
                    ss << key << CurrCamera->position.y;
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("CAMERAPOSITIONZ", thatSpecificLine))
                {
                    ss << key << CurrCamera->position.z;
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("CAMERATARGETX", thatSpecificLine))
                {
                    ss << key << CurrCamera->target.x;
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("CAMERATARGETY", thatSpecificLine))
                {
                    ss << key << CurrCamera->target.y;
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("CAMERATARGETZ", thatSpecificLine))
                {
                    ss << key << CurrCamera->target.z;
                    writeFile << ss.str() << std::endl;
                }
            }
            else if (checkWhetherTheWordInThatString("PLAYER", thatSpecificLine) && PlayerObj)
            {
                if (checkWhetherTheWordInThatString("PLAYERMASS", thatSpecificLine))
                {
                    ss << key << PlayerObj->GetMass();
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("PLAYERPOSX", thatSpecificLine))
                {
                    ss << key << PlayerObj->GetPosition().x;
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("PLAYERPOSY", thatSpecificLine))
                {
                    ss << key << PlayerObj->GetPosition().y;
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("PLAYERPOSZ", thatSpecificLine))
                {
                    ss << key << PlayerObj->GetPosition().z;
                    writeFile << ss.str() << std::endl;
                }
                else if (checkWhetherTheWordInThatString("PLAYERYROTATION", thatSpecificLine))
                {
                    ss << key << PlayerObj->GetRotationAngle();
                    writeFile << ss.str() << std::endl;
                }
            }
            else if (checkWhetherTheWordInThatString("QUESTS", thatSpecificLine))
            {
                ss << key;
                for (std::map<std::string, int>::iterator it = playerCurrQState.begin(), end = playerCurrQState.end(); it != end; ++it)
                {
                    ss << it->second << ',';
                }
                std::string theEntireLine = ss.str();
                if (theEntireLine[theEntireLine.size() - 1] == ',')
                    theEntireLine.erase(theEntireLine.end() - 1);
                writeFile << theEntireLine << std::endl;
            }
            else
                writeFile << (*it) << std::endl;
        }
        writeFile.close();
        return true;
    }
    return false;
}

void GlobalPlayer::SetCurrCam(Camera3 &theCam)
{
    CurrCamera = &theCam;
}

void GlobalPlayer::settingTheFileToSave(const int &whatFile)
{
    whatSave = whatFile;
}

bool GlobalPlayer::automaticallyLoadFile()
{
    if (whatSave > 0)
    {
        std::ostringstream ss;
        ss << "DrivenFiles//PlayerSave" << whatSave << ".csv";
        return LoadPlayerSave(ss.str());
    }
    return false;
}

bool GlobalPlayer::automaticallySaveFile()
{
    if (whatSave > 0)
    {
        std::ostringstream ss;
        ss << "DrivenFiles//PlayerSave" << whatSave << ".csv";
        return RewritePlayerSave(ss.str());
    }
    return false;
}