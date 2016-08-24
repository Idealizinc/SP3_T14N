#include "NPC_GuardCapt.h"
#include "..\\Systems\\Scene_System.h"

GuardCapt::GuardCapt()
{
}

GuardCapt::~GuardCapt()
{
}

void GuardCapt::Init()
{
	name_ = "NPC_guardcaptain";
}

void GuardCapt::Update(double dt)
{
	static float currentAngle = 0.f;
	float desiredAngle = Math::RadianToDegree(atan2(getTarget().x - GetPosition().x, getTarget().z - GetPosition().z));
	if ((getTarget() - GetPosition()).LengthSquared() < 900)
	{
		float Speed = 100.f;

		if (currentAngle + Speed * dt < desiredAngle + Math::EPSILON)
		{
			currentAngle += Speed * (float)dt;
		}
		if (currentAngle - Speed * dt > desiredAngle - Math::EPSILON)
		{
			currentAngle -= Speed * (float)dt;
		}

		SetRotationAngle(currentAngle);

		if (Scene_System::accessing().cSS_InputManager->GetKeyValue('Q'))
		{
			interacting = true;
		}
	}
}