/**************************************************************************

Filename    :   main.cpp
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include <qapplication.h>
#include "mainwindow.h"
#include "samplingthread.h"
#include "Serial.h"
#include "qthread.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    app.setPalette( Qt::darkGray );

    MainWindow window;
    window.resize( 900, 200 );

    SamplingThread samplingThread;
    samplingThread.setAmplitude( window.amplitude() );
    samplingThread.setInterval( window.signalInterval() );

    window.connect( &window, SIGNAL( amplitudeChanged( double ) ),
        &window, SLOT( setAmplitude( double ) ) );
    window.connect( &window, SIGNAL( signalIntervalChanged( double ) ),
        &samplingThread, SLOT( setInterval( double ) ) );

	// signals emitted from the sampling thread, when commands from the arduino arrive
	samplingThread.connect(&samplingThread, SIGNAL(signalEchoCommand(EchoCommand*)), &window, SLOT (echoCommand(EchoCommand*)));
	samplingThread.connect(&samplingThread, SIGNAL(MotorsOff()), &window, SLOT (MotorsOff()));

    window.show();

    samplingThread.start();
    window.start();

    bool ok = app.exec();

    samplingThread.stop();
    samplingThread.wait( 1000 );

    return ok;
}
