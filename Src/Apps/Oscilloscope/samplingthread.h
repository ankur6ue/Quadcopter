#include <qwt_sampling_thread.h>
#include <io.h>
#include "runningaverage.h"
#include "echocommanddef.h"

class DataParser;
class CommandDef;
class Serial;
class Serial_Overlapped;
#define MAX_INCOMING_DATA 5000

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread( QObject *parent = NULL );
	virtual  ~SamplingThread();
	bool GetSensorData(char* incomingData, unsigned int packetLength, char* prefixes[], int dataLength, float data[][4], bool prefixFound[]);
	int SetupSerialPort();
    double frequency() const;
    double amplitude() const;
	bool BlockTillReply(unsigned long timeout, char* ackCmdId);
	bool CheckForAck(char* ackCmdId);
	void SendBeacon();
	void SendCommandAndWaitAck(CommandDef* pcommandDef);
	bool IsRecording() { return bIsRecording;  }
	void RegisterDataParsers(DataParser* pParser);

Q_SIGNALS:
	void signalEchoCommand(EchoCommand*);
	void MotorsOff();
	void logPlaybackOver();

public Q_SLOTS:
    void setAmplitude( double );
    void setFrequency( double );
	void recordToggleClicked();
	void onLogFileSelected(QString);
	void onPlayToggled();

protected:
    virtual void sample( double elapsed );
/*	virtual void run(); */
private:
    virtual double value( double timeStamp ) const;
	double		pfrequency;
    double		pamplitude;
	char		cIncomingData[MAX_INCOMING_DATA];
	char		cIncomingDataAck[MAX_INCOMING_DATA]; // Separate buffers for receiving command acks
	unsigned int iDataLength;

	Serial*		Sp;
	FILE*		fp;
	DataParser* pDataParser;
	DataParser* pDataParser_Playback;
	RunningAverage RunningAvg; 
	int			BytesRead;
	bool		bIsRecording;
	bool		bIsPlaying;
	bool		bRunFlightLog;
};
