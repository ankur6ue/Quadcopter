#include <qwt_sampling_thread.h>
#include "Serial.h"
#include <io.h>

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

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread( QObject *parent = NULL );
	bool GetSensorData(char* incomingData, unsigned int packetLength, char* prefixes[], int dataLength, float data[][4]);
	int SetupSerialPort();
    double frequency() const;
    double amplitude() const;

public Q_SLOTS:
    void setAmplitude( double );
    void setFrequency( double );

protected:
    virtual void sample( double elapsed );
/*	virtual void run(); */
private:
    virtual double value( double timeStamp ) const;

    double d_frequency;
    double d_amplitude;
	char cIncomingData[2000];
	unsigned int iDataLength;
	Serial* Sp;
	FILE* fp;
	RunningAverage RunningAvg; 
};
