#ifndef MUSIC_SYSTEM_H
#define MUSIC_SYSTEM_H

#include "GenericSystem.h"
//#include "..\\Classes\\SingletonTemplate.h"
#include "../Classes/MusicEntity2D.h"

class MusicSystem : public GenericSystem
{
public:
    MusicSystem();
    virtual ~MusicSystem();

    virtual void Init();
    virtual void Update(double dt);
    virtual void onNotify(const std::string &theMessage);

private:
    ISoundEngine *musicEngine;
    bool beginLoadingMusic(const std::string &fileName);
};

#endif