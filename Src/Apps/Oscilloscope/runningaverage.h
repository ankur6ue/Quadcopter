#ifndef __RUNNINGAVERAGE
#define __RUNNINGAVERAGE

class RunningAverage
{
public:
	int iDataSize;
	float fSum;
	float fAverage;

	RunningAverage()
	{
		fSum = 0; fAverage = 0; iDataSize = 0;
	}

	void Accum(float newData)
	{
		fSum += newData;
		iDataSize++;
	}
	float GetAvg()
	{
		return fSum/iDataSize;
	}
};
#endif