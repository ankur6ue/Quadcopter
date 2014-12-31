#include <qwt_sampling_thread.h>
#include "Serial.h"
#include <io.h>
#include "runningaverage.h"
#include "echocommanddef.h"

class SensorDataParser;

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread( QObject *parent = NULL );
	bool GetSensorData(char* incomingData, unsigned int packetLength, char* prefixes[], int dataLength, float data[][4], bool prefixFound[]);
	int SetupSerialPort();
    double frequency() const;
    double amplitude() const;

Q_SIGNALS:
	void signalEchoCommand(EchoCommand*);

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
	SensorDataParser* pSensorDataParser;
	RunningAverage RunningAvg; 
};
