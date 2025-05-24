#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include "masTime.h"




struct EngC_TimeData
{
	double Frequency;
	double StartTime;
	double FrameBegin;
	double DeltaTime;
};
static EngC_TimeData Time = {};

void masTime_Init()
{
	LARGE_INTEGER L = {};
	QueryPerformanceFrequency(&L);
	Time.Frequency = 1.f / L.QuadPart;

	L = {};
	QueryPerformanceCounter(&L);
	Time.StartTime  = Time.Frequency * L.QuadPart;
	Time.FrameBegin = Time.StartTime;
}

void masTime_Update()
{
	double End = masTime();
	Time.DeltaTime  = End - Time.FrameBegin;
	Time.FrameBegin = End;
}

double masTime()
{
	LARGE_INTEGER L = {};
	QueryPerformanceCounter(&L);
	double TimeStamp = Time.Frequency * L.QuadPart;
	double AppTime   = TimeStamp - Time.StartTime;
	return AppTime;
}

double masTime_DeltaTime()
{
	return Time.DeltaTime;
}