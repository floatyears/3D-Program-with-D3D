#include <Windows.h>
#include "GameTimer.h"


GameTimer::GameTimer():mSecondPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), 
  mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondPerCount = 1.0f/(double)countsPerSec;
}

float GameTimer::TotalTime()const
{
	if(mStopped)
	{
		return(float)(((mStopTime - mPausedTime)-mBaseTime)*mSecondPerCount);
	}else
	{
		float time = (float)(((mCurrTime-mPausedTime)-mBaseTime)*mSecondPerCount);
		return time;
	}
}

float GameTimer::DeltaTime()const
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if(mStopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if(!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	if(mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime)*mSecondPerCount;

	mPrevTime = mCurrTime;

	if(mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

GameTimer::~GameTimer()
{
}
