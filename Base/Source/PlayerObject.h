#pragma once

#include "GameObject.h"

class PlayerObject : public GameObject
{
public:
    PlayerObject();
    virtual ~PlayerObject();

    virtual void Update(double dt);

    virtual void SetJump(const float &speed, const float &max_speed, const float &accel);
    virtual void setVel(const Vector3 &theSpeed);
    virtual void setAccel(const Vector3 &theacceleration);

protected:
    virtual void walkDirection(const float &degree, const float &byHowMuch);

    Vector3 vel_, accel_, MovementValues;
    float JumpVel;
    float JUMPMAXSPEED, JUMPACCEL;
    bool m_bJumping;
    double m_ElapsedTime;
};