#ifndef _WHEELBOX_H_
#define _WHEELBOX_H_

#include <qwidget.h>

class QwtWheel;
class QLabel;
class QLCDNumber;

class WheelBox: public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QColor theme READ theme WRITE setTheme )

public:
    WheelBox( const QString &title,
        double min, double max, double stepSize,
        QWidget *parent = NULL );

    void setTheme( const QColor & );
    QColor theme() const;

    void setUnit( const QString & );
    QString unit() const;

    void setValue( double value );
    double value() const;

Q_SIGNALS:
    double valueChanged( double );

private:
    QLCDNumber *pnumber;
    QwtWheel *pwheel;
    QLabel *plabel;

    QString d_unit;
};

#endif
