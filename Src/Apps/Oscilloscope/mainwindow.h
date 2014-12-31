#include <qwidget.h>

class Plot;
class Knob;
class WheelBox;
class QLineEdit;
class QPushButton;
struct EchoCommand;

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
	void textChanged(const QString &);
	void speedChanged(double);
	void echoCommand(EchoCommand*);
	void motorToggleClicked();

private:

	// Widget pointers. 
    Knob		*d_frequencyKnob;
    Knob		*d_amplitudeKnob;
    WheelBox	*d_timerWheel;
    WheelBox	*d_intervalWheel;
	WheelBox	*d_speedWheel;
	QPushButton *d_motorToggle;
	QLineEdit	*d_inputTextField;
	QLineEdit	*d_escState;

	// Widget state variables
	bool		bMotorToggle;
    Plot *y_plot;
	Plot *p_plot;
	Plot *r_plot;

	double d_amplitude;
};
