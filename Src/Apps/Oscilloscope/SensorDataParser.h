
#include "signaldata.h"
#include "qmath.h"
class SamplingThread;

class SensorDataParserImpl
{
public:
	SensorDataParserImpl(SamplingThread* samplingThread, const char* _prefix, int dataLength)
	{
		if (_prefix)
		{
			strcpy_s(Prefix, 100, _prefix);
			PrefixLength = strlen(Prefix);
		}
		else
		{
			PrefixLength = -1;
		}
		DataLength = dataLength;
		RadToDegree = 180/3.14;
		pSamplingThread = samplingThread;
	}

	virtual void Plot(double elapsed)
	{
	}

	virtual float GetData()
	{
		return 0.0;
	}

	virtual bool Parse(char* incomingData, int packetLength, char* commandId = NULL)
	{
		bPrefixFound = false;
		if (PrefixLength == -1) return false;

		for (int i = 0; i < max(0, packetLength-PrefixLength); i++)
		{
			if (!strncmp(incomingData + i, Prefix, PrefixLength))
			{
				int read = 0;
				if (DataLength == 1)
					read = sscanf(incomingData+i+PrefixLength, "\t%f\t", Data);
				if (DataLength == 2)
					read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f", Data, Data+1);
				if (DataLength == 3)
					read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f", Data, Data+1, Data+2);
				if (DataLength == 4)
					read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f\t%f", Data, Data+1, Data+2, Data+3);
				
				bPrefixFound = true;
				break;
			}
		}
		return bPrefixFound;
	}

	char	Prefix[100];
	int		PrefixLength;
	float	Data[4];
	float	RadToDegree;
	int		DataLength;
	bool	bPrefixFound;
	SamplingThread* pSamplingThread;
};

class SensorDataParser
{
public:
	SensorDataParser(){}

	QList<SensorDataParserImpl*> DataParsers;
	QList<SensorDataParserImpl*> AckParsers;

	void RegisterDataParser(SensorDataParserImpl* dataParser)
	{
		DataParsers.push_back(dataParser);
	}

	void RegisterAckParser(SensorDataParserImpl* ackParser)
	{
		AckParsers.push_back(ackParser);
	}

	void RemoveDataParser(SensorDataParserImpl* dataParser)
	{
		DataParsers.removeOne(dataParser);
	}

	void RemoveAckParser(SensorDataParserImpl* ackParser)
	{
		AckParsers.removeOne(ackParser);
	}

	bool ParseData(char* incomingData, int packetLength)
	{
		bool isParsed = false;
		for (int i = 0; i < DataParsers.length(); i++)
		{
			isParsed = DataParsers[i]->Parse(incomingData, packetLength);
		}
		return isParsed;
	}

	bool ParseAck(char* incomingData, int packetLength, char* commandId)
	{
		bool isParsed = false;
		for (int i = 0; i < AckParsers.length(); i++)
		{
			isParsed = AckParsers[i]->Parse(incomingData, packetLength, commandId);
		}
		return isParsed;
	}

	void Plot(double elapsed)
	{
		for (int i = 0; i < DataParsers.length(); i++)
		{
			DataParsers[i]->Plot(elapsed);
		}
	}
};



class SensorDataParserImplYpr: public SensorDataParserImpl
{
public:
	SensorDataParserImplYpr(SamplingThread* psamplingThread, char* _prefix, int dataLength): SensorDataParserImpl(psamplingThread, _prefix, dataLength){};

	void Plot(double elapsed)
	{
		if (bPrefixFound)
		{
			const QPointF y1( elapsed, RadToDegree*Data[0]);
			SignalData::instance(yaw, MPU).append( y1 );

			const QPointF p1( elapsed, RadToDegree*Data[1]);
			SignalData::instance(pitch, MPU).append( p1);

			const QPointF r1( elapsed, RadToDegree*Data[2]);
			SignalData::instance(roll, MPU).append( r1 );
		}
	}
};

class SensorDataParserImplPID: public SensorDataParserImpl
{
public:
	SensorDataParserImplPID(SamplingThread* psamplingThread, char* _prefix, int dataLength): SensorDataParserImpl(psamplingThread, _prefix, dataLength){};

	void Plot(double elapsed)
	{
		if (bPrefixFound)
		{
			const QPointF y1( elapsed, 10*Data[0]);
			SignalData::instance(yaw, PID).append( y1 );

			const QPointF p1( elapsed, 10*Data[1]);
			SignalData::instance(pitch, PID).append( p1);

			const QPointF r1( elapsed, 10*Data[2]);
			SignalData::instance(roll, PID).append( r1 );
		}
	}
};

class SensorDataParserImplFusion: public SensorDataParserImpl
{
public:
	SensorDataParserImplFusion(SamplingThread* psamplingThread, char* _prefix, int dataLength): SensorDataParserImpl(psamplingThread, _prefix, dataLength){};

	void Plot(double elapsed)
	{
		if (bPrefixFound)
		{
			const QPointF r2( elapsed, RadToDegree*Data[0]);
			SignalData::instance(roll, SensorFusion).append( r2 );

			const QPointF p2( elapsed, RadToDegree*Data[1]);
			SignalData::instance(pitch, SensorFusion).append( p2);

			const QPointF y2( elapsed, RadToDegree*Data[2]);
			SignalData::instance(yaw, SensorFusion).append( y2 );
		}
	}
};

class SensorDataParserImplCommands: public SensorDataParserImpl
{
public:
	SensorDataParserImplCommands(SamplingThread* psamplingThread): SensorDataParserImpl(psamplingThread, "QuadState", 0){};

	virtual bool Parse(char* incomingData, int packetLength, char* commandId = NULL);

	void Plot(double elapsed)
	{
		if (bPrefixFound)
		{
			int k = 0;
		}
	}
};

class SensorDataParserImplAck: public SensorDataParserImpl
{
public:
	SensorDataParserImplAck(SamplingThread* psamplingThread): SensorDataParserImpl(psamplingThread, "Ack", 1){};

	virtual bool Parse(char* incomingData, int packetLength, char* commandId);

	float GetData()
	{
		return Data[0];
	}
};