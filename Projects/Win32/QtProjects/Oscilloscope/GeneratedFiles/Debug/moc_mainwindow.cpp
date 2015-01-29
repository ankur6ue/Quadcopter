/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../Src/Apps/Oscilloscope/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ArrowPadDef_t {
    QByteArrayData data[6];
    char stringdata[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ArrowPadDef_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ArrowPadDef_t qt_meta_stringdata_ArrowPadDef = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 15),
QT_MOC_LITERAL(2, 28, 0),
QT_MOC_LITERAL(3, 29, 17),
QT_MOC_LITERAL(4, 47, 17),
QT_MOC_LITERAL(5, 65, 18)
    },
    "ArrowPadDef\0ButtonUpClicked\0\0"
    "ButtonDownClicked\0ButtonLeftClicked\0"
    "ButtonRightClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ArrowPadDef[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    0,   35,    2, 0x0a /* Public */,
       4,    0,   36,    2, 0x0a /* Public */,
       5,    0,   37,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ArrowPadDef::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ArrowPadDef *_t = static_cast<ArrowPadDef *>(_o);
        switch (_id) {
        case 0: _t->ButtonUpClicked(); break;
        case 1: _t->ButtonDownClicked(); break;
        case 2: _t->ButtonLeftClicked(); break;
        case 3: _t->ButtonRightClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ArrowPadDef::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ArrowPadDef.data,
      qt_meta_data_ArrowPadDef,  qt_static_metacall, 0, 0}
};


const QMetaObject *ArrowPadDef::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ArrowPadDef::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArrowPadDef.stringdata))
        return static_cast<void*>(const_cast< ArrowPadDef*>(this));
    return QWidget::qt_metacast(_clname);
}

int ArrowPadDef::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[29];
    char stringdata[428];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 16),
QT_MOC_LITERAL(2, 28, 0),
QT_MOC_LITERAL(3, 29, 16),
QT_MOC_LITERAL(4, 46, 21),
QT_MOC_LITERAL(5, 68, 20),
QT_MOC_LITERAL(6, 89, 12),
QT_MOC_LITERAL(7, 102, 11),
QT_MOC_LITERAL(8, 114, 12),
QT_MOC_LITERAL(9, 127, 24),
QT_MOC_LITERAL(10, 152, 23),
QT_MOC_LITERAL(11, 176, 16),
QT_MOC_LITERAL(12, 193, 15),
QT_MOC_LITERAL(13, 209, 14),
QT_MOC_LITERAL(14, 224, 14),
QT_MOC_LITERAL(15, 239, 14),
QT_MOC_LITERAL(16, 254, 14),
QT_MOC_LITERAL(17, 269, 12),
QT_MOC_LITERAL(18, 282, 12),
QT_MOC_LITERAL(19, 295, 12),
QT_MOC_LITERAL(20, 308, 14),
QT_MOC_LITERAL(21, 323, 5),
QT_MOC_LITERAL(22, 329, 14),
QT_MOC_LITERAL(23, 344, 14),
QT_MOC_LITERAL(24, 359, 14),
QT_MOC_LITERAL(25, 374, 11),
QT_MOC_LITERAL(26, 386, 12),
QT_MOC_LITERAL(27, 399, 18),
QT_MOC_LITERAL(28, 418, 9)
    },
    "MainWindow\0amplitudeChanged\0\0"
    "frequencyChanged\0signalIntervalChanged\0"
    "startCtrlInputThread\0setAmplitude\0"
    "textChanged\0speedChanged\0"
    "DefaultPitchSetPtChanged\0"
    "DefaultRollSetPtChanged\0PitchCtrlChanged\0"
    "RollCtrlChanged\0YawCtrlChanged\0"
    "PitchKpChanged\0PitchKiChanged\0"
    "PitchKdChanged\0YawKpChanged\0YawKiChanged\0"
    "YawKdChanged\0FRStateChanged\0state\0"
    "BRStateChanged\0FLStateChanged\0"
    "BLStateChanged\0echoCommand\0EchoCommand*\0"
    "motorToggleClicked\0AxisMoved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  139,    2, 0x06 /* Public */,
       3,    1,  142,    2, 0x06 /* Public */,
       4,    1,  145,    2, 0x06 /* Public */,
       5,    0,  148,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,  149,    2, 0x0a /* Public */,
       7,    1,  152,    2, 0x0a /* Public */,
       8,    1,  155,    2, 0x0a /* Public */,
       9,    1,  158,    2, 0x0a /* Public */,
      10,    1,  161,    2, 0x0a /* Public */,
      11,    1,  164,    2, 0x0a /* Public */,
      12,    1,  167,    2, 0x0a /* Public */,
      13,    1,  170,    2, 0x0a /* Public */,
      14,    1,  173,    2, 0x0a /* Public */,
      15,    1,  176,    2, 0x0a /* Public */,
      16,    1,  179,    2, 0x0a /* Public */,
      17,    1,  182,    2, 0x0a /* Public */,
      18,    1,  185,    2, 0x0a /* Public */,
      19,    1,  188,    2, 0x0a /* Public */,
      20,    1,  191,    2, 0x0a /* Public */,
      22,    1,  194,    2, 0x0a /* Public */,
      23,    1,  197,    2, 0x0a /* Public */,
      24,    1,  200,    2, 0x0a /* Public */,
      25,    1,  203,    2, 0x0a /* Public */,
      27,    0,  206,    2, 0x0a /* Public */,
      28,    4,  207,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, 0x80000000 | 26,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Long, QMetaType::Long, QMetaType::Long, QMetaType::Int,    2,    2,    2,    2,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->amplitudeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->frequencyChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->signalIntervalChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->startCtrlInputThread(); break;
        case 4: _t->setAmplitude((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->speedChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->DefaultPitchSetPtChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->DefaultRollSetPtChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->PitchCtrlChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: _t->RollCtrlChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->YawCtrlChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->PitchKpChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->PitchKiChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->PitchKdChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->YawKpChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->YawKiChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->YawKdChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->FRStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->BRStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->FLStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->BLStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->echoCommand((*reinterpret_cast< EchoCommand*(*)>(_a[1]))); break;
        case 23: _t->motorToggleClicked(); break;
        case 24: _t->AxisMoved((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::amplitudeChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (MainWindow::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::frequencyChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (MainWindow::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::signalIntervalChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::startCtrlInputThread)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::amplitudeChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::frequencyChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainWindow::signalIntervalChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MainWindow::startCtrlInputThread()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
