#include "Timer.h"
#include <windows.h>   // include important windows stuff
#include <WindowsX.h>
#include <mmsystem.h>

cTimer::cTimer()
{


}
 cTimer::~cTimer()
{


}
void cTimer::Initialize()
{
	m_Fps = 0;
	m_Count = 0;
	m_StartTimeFPS = timeGetTime();

	// Store counts per second
	m_CountsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_CountsPerSec);
	m_SecPerCount = 1.0f / m_CountsPerSec; //store seconds per count
	
	m_PrevTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_PrevTime);
  
	m_DeltaTime = 0;
}

void cTimer::UpdateFPS()
{

	m_Count++;
	// if one sec is passed
	if (timeGetTime() >= (m_StartTimeFPS + 1000))
	{
		m_Fps = m_Count;
		m_Count = 0;
		m_StartTimeFPS = timeGetTime();
	}


}
void cTimer::UpdateTimer()
{
	//Capture current count
	long long curTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
	//Calculate deltaTime
	m_DeltaTime = (curTime - m_PrevTime) * m_SecPerCount;
	m_PrevTime = curTime;
}

float cTimer::GetTime()
{
	return m_DeltaTime;
}
int cTimer::GetFps()
{
	return m_Fps;
}
