#pragma  once
#include <stdint.h>
 

class cTimer
{
public:
	cTimer();
	~cTimer();

	void  Initialize();
	void  UpdateFPS();
	void  UpdateTimer();
	int   GetFps();
	float GetTime();
private:
	int m_Fps, m_Count;
	unsigned long m_StartTimeFPS;
 
	long long m_CountsPerSec;
	long long m_PrevTime;
	float m_SecPerCount;
	long long m_StartTimeTimer;
	float m_DeltaTime;
};


