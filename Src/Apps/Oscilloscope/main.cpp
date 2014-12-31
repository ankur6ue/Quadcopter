#include <qapplication.h>
#include "mainwindow.h"
#include "samplingthread.h"
#include "Serial.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    app.setPalette( Qt::darkGray );

    MainWindow window;
    window.resize( 800, 400 );

    SamplingThread samplingThread;
    samplingThread.setAmplitude( window.amplitude() );
    samplingThread.setInterval( window.signalInterval() );

    window.connect( &window, SIGNAL( amplitudeChanged( double ) ),
        &window, SLOT( setAmplitude( double ) ) );
    window.connect( &window, SIGNAL( signalIntervalChanged( double ) ),
        &samplingThread, SLOT( setInterval( double ) ) );

	// signals emitted from the sampling thread, when commands from the arduino arrive
	samplingThread.connect(&samplingThread, SIGNAL(signalEchoCommand(EchoCommand*)), &window, SLOT (echoCommand(EchoCommand*)));

    window.show();

    samplingThread.start();
    window.start();

    bool ok = app.exec();

    samplingThread.stop();
    samplingThread.wait( 1000 );

    return ok;
}
