#include "samplingthread.h"
#include "signaldata.h"
#include <qwt_math.h>
#include <math.h>

#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#endif

SamplingThread::SamplingThread( QObject *parent ):
    QwtSamplingThread( parent ),
    d_frequency( 5.0 ),
    d_amplitude( 20.0 ), iDataLength(1500)
{
	SetupSerialPort();
//	fp = fopen("C:\\Qt\\SensorData.txt", "w");
	
}

// Sets up the serial port on port 16, baud rate = 115200
int SamplingThread::SetupSerialPort()
{
	int success = false;
	Sp = new Serial("\\\\.\\COM16", 115200);    // adjust as needed

	if (Sp->IsConnected())
	{
		printf("We're connected");
		success = true;
	}
	return success;
}

void SamplingThread::setFrequency( double frequency )
{
    d_frequency = frequency;
}

void SamplingThread::setAmplitude( double amplitude )
{
    d_amplitude = amplitude;
}

double SamplingThread::frequency() const
{
    return d_frequency;
}

double SamplingThread::amplitude() const
{
    return d_amplitude;
}

/*
Purpose: parses the packet read from the serial port and fills the data array with the parsed data
Params:
incomingData: Pointer to the incoming data
packetLength: the length of the packet read. This depends on the sampling interval, the packet length will be smaller for a 
smaller sampling interval. If the sampling interval is too small, the data packet can be incomplete, leading to bad readings of the
sensor values
prefixes: In this application, two data streams are being read - the data stream from the mpu and the data stream resulting from
sensor fusion of the accelerometer and gyroscope data. The prefixes array contains the prefixes for the two streams, so the parser
knows which one is which
dataLength: Tells the parser how many data elements follow the prefix. For example for yaw-pitch-roll data, dataLength = 3
data: The sensor values read are stored in the data array. 
*/
bool SamplingThread::GetSensorData(char* incomingData, unsigned int packetLength, char* prefixes[], int dataLength, float data[][4])
{
	printf("%d\n", packetLength);
	int prefixLength[2];
	prefixLength[0] = strlen(prefixes[0]);
	prefixLength[1] = strlen(prefixes[1]);
	bool prefixFound[2] = {false, false};

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < max(0, packetLength-prefixLength[j]); i++)
		{
			if (!strncmp(incomingData + i, prefixes[j], prefixLength[j]))
			{
				int read = 0;
				if (dataLength == 1)
					read = sscanf(incomingData+i+prefixLength[j], "%f\t", data[j]);
				if (dataLength == 2)
					read = sscanf(incomingData+i+prefixLength[j], "%f\t%f\t", data[j], data[j]+1);
				if (dataLength == 3)
					read = sscanf(incomingData+i+prefixLength[j], "%f\t%f\t%f", data[j], data[j]+1, data[j]+2);
				if (dataLength == 4)
					read = sscanf(incomingData+i+prefixLength[j], "%f\t%f\t%f\t%f", data[j], data[j]+1, data[j]+2, data[j]+3);
				// assert (read == dataLength)
				//	fprintf(fp, "%f\n", data[j]);

				//float udata = abs(data[1]);
				//RunningAvg.Accum(udata);
				//float avg = RunningAvg.GetAvg();
				//if (udata > 1.2*avg || udata < 0.8*avg)
				//{
				//	printf("here comes trouble\n");
				//}
				prefixFound[j] = true;
				break;
			}
		}
	}
	if (prefixFound[0] == true && prefixFound[1] == true) return true;
	return false;
}

void SamplingThread::sample( double elapsed )
{
    if ( d_frequency > 0.0 )
    {
		int bytesRead = Sp->ReadData(cIncomingData,iDataLength);
	//	printf("Bytes read: (-1 means no data available) %i\n",readResult);
		if (bytesRead != -1)
		{
			char* prefixes[2];
			float data[2][4];
			prefixes[0] = "mpu"; // hardcoded for now, should match the prefix used in Serial.Print command on the microprocessor
			prefixes[1] = "fusion";
			if (GetSensorData(cIncomingData, bytesRead, prefixes, 3, data))
			{
				const QPointF y1( elapsed, data[0][0]);
				SignalData::instance(yaw, MPU).append( y1 );

				const QPointF r2( elapsed, data[1][0]);
				SignalData::instance(roll, SensorFusion).append( r2 );
					
				const QPointF p1( elapsed, data[0][1]);
				SignalData::instance(pitch, MPU).append( p1);

				const QPointF p2( elapsed, data[1][1]);
				SignalData::instance(pitch, SensorFusion).append( p2);

				const QPointF r1( elapsed, data[0][2]);
				SignalData::instance(roll, MPU).append( r1 );

				const QPointF y2( elapsed, data[1][2]);
				SignalData::instance(yaw, SensorFusion).append( y2 );
			}
		}
		
    }
}

double SamplingThread::value( double timeStamp ) const
{
    const double period = 1.0 / d_frequency;

    const double x = ::fmod( timeStamp, period );
    const double v = d_amplitude * qFastSin( x / period * 2 * M_PI );

    return v;
}

/*
void SamplingThread::run()
{
   
}
*/