#include <qwidget.h>

#define PITCH_CTRL_RANGE	45
#define ROLL_CTRL_RANGE		45
#define YAW_CTRL_RANGE		45

class Plot;
class Knob;
class WheelBox;
class QLineEdit;
class QPushButton;
class QCheckBox;
struct EchoCommand;
class MainWindow;
class Joystick;
class QThread;

class ArrowPadDef: public QWidget
{
	Q_OBJECT
public:
	ArrowPadDef(MainWindow* pmainWindow );

	QPushButton *pButtonUp;
	QPushButton *pButtonDown;
	QPushButton *pButtonRight;
	QPushButton *pButtonLeft;
	
	void ConnectSignals();
	void ResetSetPoint();
public Q_SLOTS:
	void ButtonUpClicked();
	void ButtonDownClicked();
	void ButtonLeftClicked();
	void ButtonRightClicked();

public:
	MainWindow* pMainWindow;
	int PitchSetPoint;
	int RollSetPoint;
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow( QWidget * = NULL );

	typedef struct PIDParamsDef
	{
		float fKp;
		float fKi;
		float fKd;
	};
	
    void	start();
    double	amplitude() const;
    double	frequency() const;
    double	signalInterval() const;
	void	ReadPIDParams(PIDParamsDef& yparams, PIDParamsDef& pparams, PIDParamsDef& rparams);
	void	ResetSetPoint();
	// To set up the thread that runs the game controller input
	void	SetupCtrlInput();
	// Create different UI windows and manage layouts
	void CreatePlots();
	void CreatePlotControls();
	void CreatePIDControls();
	void CreateQuadStatePanel();
	void CreateQuadControlPanel();
	void ManageLayout();

Q_SIGNALS:
    void amplitudeChanged( double );
    void frequencyChanged( double );
    void signalIntervalChanged( double );
	void startCtrlInputThread();

public Q_SLOTS:
    void setAmplitude( double );
	void textChanged(const QString &);
	void speedChanged(double);
	void DefaultPitchSetPtChanged(double);
	void DefaultRollSetPtChanged(double);
	void PitchCtrlChanged(double);
	void RollCtrlChanged(double);
	void YawCtrlChanged(double);
	void PitchKpChanged(double);
	void PitchKiChanged(double);
	void PitchKdChanged(double);
	void YawKpChanged(double);
	void YawKiChanged(double);
	void YawKdChanged(double);
	void FRStateChanged( int state);
	void BRStateChanged( int state);
	void FLStateChanged( int state);
	void BLStateChanged( int state);
	void echoCommand(EchoCommand*);
	void motorToggleClicked();
	// Handle game controller axis movement
	void AxisMoved(long, long, long, int);
	
private:

	// Widget pointers. 
    Knob		*pFrequencyKnob;
    Knob		*pAmplitudeKnob;
    WheelBox	*pTimerWheel;
    WheelBox	*pIntervalWheel;
	WheelBox	*pPitchSetPtWheel;
	WheelBox	*pRollSetPtWheel;
	WheelBox	*pPitchCtrlWheel;
	WheelBox	*pRollCtrlWheel;
	WheelBox	*pYawCtrlWheel;
	WheelBox	*pSpeedWheel;
	WheelBox	*pPitchKp;
	WheelBox	*pPitchKi;
	WheelBox	*pPitchKd;
	WheelBox	*pYawKp;
	WheelBox	*pYawKi;
	WheelBox	*pYawKd;
	QPushButton *pMotorToggle;
	QLineEdit	*pQuadSpeed;
	QLineEdit	*pQuadPower;
	QLineEdit	*pQuadPKi; // Quad Pitch PID coefficients
	QLineEdit	*pQuadPKp;
	QLineEdit	*pQuadPKd;
	QLineEdit	*pQuadRKi; // Quad Roll PID coefficients
	QLineEdit	*pQuadRKp;
	QLineEdit	*pQuadRKd;
	QLineEdit	*pQuadYKi; // Quad Yaw PID coefficients
	QLineEdit	*pQuadYKp;
	QLineEdit	*pQuadYKd;

	QCheckBox	*pFR, *pFL, *pBR, *pBL;

	ArrowPadDef* pArrowPad;
	// Widget state variables
	bool		bMotorToggle;
    Plot *y_plot;
	Plot *p_plot;
	Plot *r_plot;

	// Joystick pointer
	Joystick* pJoystick;
	// Pointer to the thread that runs the joystick
	QThread* pThread;
	
	double pamplitude;

	PIDParamsDef PitchPIDParams;
	PIDParamsDef RollPIDParams;
	PIDParamsDef YawPIDParams;

public:
	// These are the set points we revert to when we reset the motors.
	int DefaultPitchSetPoint;
	int DefaultRollSetPoint;

};
