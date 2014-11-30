#include <qwidget.h>

class Plot;
class Knob;
class WheelBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow( QWidget * = NULL );

    void start();

    double amplitude() const;
    double frequency() const;
    double signalInterval() const;

Q_SIGNALS:
    void amplitudeChanged( double );
    void frequencyChanged( double );
    void signalIntervalChanged( double );

public Q_SLOTS:
    void setAmplitude( double );

private:
    Knob *d_frequencyKnob;
    Knob *d_amplitudeKnob;
    WheelBox *d_timerWheel;
    WheelBox *d_intervalWheel;

    Plot *y_plot;
	Plot *p_plot;
	Plot *r_plot;

	double d_amplitude;
};
