#include "BattleScreenObject.h"
#include "..\\Systems\\Scene_System.h"
#include "..\\Scenes\\GraphicsEntity.h"

BattleScreenObject::BattleScreenObject(const std::string& MeshName, const float& Mass, const Vector3& Position, const Vector3& Dimensions, const Vector3& Acceleration, const float& RotationAngle, const Vector3& RotationAxis)
{
	Init(MeshName, Mass, Position, Dimensions, 0, RotationAngle, RotationAxis);
	SetAcceleration(Acceleration);
}

BattleScreenObject::~BattleScreenObject()
{
	Exit();
}

// Virtual
void BattleScreenObject::Init(const std::string& MeshName, const float& Mass, const Vector3& Position, const Vector3& Dimensions, const Vector3& Acceleration, const float& RotationAngle, const Vector3& RotationAxis)
{
	Active = true;
	Visible = true;
	SetParameters(MeshName, Mass, Position, Dimensions, 0, RotationAngle, RotationAxis);
	SetAcceleration(Acceleration);
}

void BattleScreenObject::Update(double dt)
{
	LifeTimer += (float)dt;
	if (LifeTime != -1)
		if (LifeTimer > LifeTime)
			Active = false;
	if (Active) // Still can update if invisible
	{
		switch (Type)
		{
		case BS_Bullet:
		{
			if (MoveToTarget)
			{
				SetAcceleration(0);
				Vector3 DVec = TargetPoint - GetPosition();
				SetVelocity(DVec);
				if (DVec.LengthSquared() < 10.f)
				{
					Retarget = true;
				}
				if (Retarget && PlayerPosition != 0)
				{
					SetVelocity(0);
					std::swap(TargetPoint, PlayerPosition);
					DVec = TargetPoint - GetPosition();
					SetAcceleration(DVec);
					MoveToTarget = false;
				}
				else if (PlayerPosition == 0) 
					MoveToTarget = false;
			}
			Vector3 StoredVelocity = GetVelocity();
			if (GravityAffected)
				SetVelocity(GetVelocity() + Vector3(0, -9.8f) * (float)dt); // For Gravity
			if (GetMass() > Math::EPSILON)
			{
				SetVelocity(GetVelocity() + Acceleration * (1.f / GetMass()) * (float)dt);
			}
			SetPosition(GetPosition() + (StoredVelocity + GetVelocity()) * 0.5f * (float)dt);
			SetRotationAngle(Math::RadianToDegree(atan2(-GetVelocity().x, GetVelocity().y)));
			break;
		}
		case BS_HRay:
		{
			if (MoveToTarget)
			{
				Vector3 DVec = TargetPoint - GetPosition();
				SetVelocity(DVec);
				if (DVec.LengthSquared() < 10.f)
				{
					MoveToTarget = false;
					SetPosition(AltTargetPoint);
					SetVelocity(0);
				}
			}
			else
			{
				if (GetMass() > Math::EPSILON)
				{
					SetVelocity(GetVelocity() + Acceleration * (1.f / GetMass()) * (float)dt);
				}
			}
			Vector3 StoredVelocity = GetVelocity();
			SetPosition(GetPosition() + (StoredVelocity + GetVelocity()) * 0.5f * (float)dt);
			SetRotationAngle(Math::RadianToDegree(atan2(-GetVelocity().x, GetVelocity().y)));
			break;
		}
		case BS_VRay:
		{
			if (MoveToTarget)
			{
				Vector3 DVec = TargetPoint - GetPosition();
				SetVelocity(DVec);
				if (DVec.LengthSquared() < 10.f)
				{
					MoveToTarget = false;
					SetPosition(AltTargetPoint);
					SetVelocity(0);
				}
			}
			else
			{
				if (GetMass() > Math::EPSILON)
				{
					SetVelocity(GetVelocity() + Acceleration * (1.f / GetMass()) * (float)dt);
				}
			}
			Vector3 StoredVelocity = GetVelocity();
			SetPosition(GetPosition() + (StoredVelocity + GetVelocity()) * 0.5f * (float)dt);
			SetRotationAngle(Math::RadianToDegree(atan2(-GetVelocity().x, GetVelocity().y)));
			break;
		}
		case BS_Blast:
		{
			if (LifeTimer > (LifeTime * 0.25f))
			{
				Visible = true;
				SetRotationAngle(GetRotationAngle() + GetRotationAngle() * (float)dt * (float)dt);
				SetDimensions(GetDimensions() + GetVelocity().LengthSquared() * GetDimensions() * (float)dt  * (float)dt);
			}
			break;
		}
		default:
		{
			Vector3 StoredVelocity = GetVelocity();
			if (GravityAffected)
				SetVelocity(GetVelocity() + Vector3(0, -9.8f) * (float)dt); // For Gravity
			if (GetMass() > Math::EPSILON)
			{
				SetVelocity(GetVelocity() + Acceleration * (1.f / GetMass()) * (float)dt);
			}
			SetPosition(GetPosition() + (StoredVelocity + GetVelocity()) * 0.5f * (float)dt);
			//SetRotationAngle(Math::RadianToDegree(atan2(-GetVelocity().x, GetVelocity().y)));
			break;
		}
		}
	}
}

void BattleScreenObject::Render()
{
	if (Active && Visible)
	{
		GraphicsEntity *SceneGraphics = dynamic_cast<GraphicsEntity*>(&Scene_System::accessing().getGraphicsScene());
		Scene_System::accessing().getCurrScene().modelStack->PushMatrix();
		Scene_System::accessing().getCurrScene().modelStack->Translate(GetPosition().x, GetPosition().y, GetPosition().z);
		Scene_System::accessing().getCurrScene().modelStack->Rotate(GetRotationAngle(), GetRotationAxis().x, GetRotationAxis().y, GetRotationAxis().z);
		Scene_System::accessing().getCurrScene().modelStack->Scale(GetDimensions().x, GetDimensions().y, GetDimensions().z);
		SceneGraphics->RenderMesh(*GetMesh(), false);
		Scene_System::accessing().getCurrScene().modelStack->PopMatrix();
	}
}


void BattleScreenObject::SetAcceleration(const Vector3& A)
{
	Acceleration = A;
}

Vector3 BattleScreenObject::GetAcceleration()
{
	return Acceleration;
}
