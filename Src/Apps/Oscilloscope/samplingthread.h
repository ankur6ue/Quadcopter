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
	bool BlockTillReply(unsigned long timeout, char* ackCmdId);
	bool CheckForAck(char* ackCmdId);

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

    double pfrequency;
    double pamplitude;
	char cIncomingData[2000];
	unsigned int iDataLength;
	Serial* Sp;
	FILE* fp;
	SensorDataParser* pSensorDataParser;
	RunningAverage RunningAvg; 
};
