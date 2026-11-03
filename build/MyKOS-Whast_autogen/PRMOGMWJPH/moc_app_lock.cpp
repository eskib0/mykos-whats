/****************************************************************************
** Meta object code from reading C++ file 'app_lock.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/core/app_lock.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'app_lock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_AppLock_t {
    uint offsetsAndSizes[20];
    char stringdata0[8];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[7];
    char stringdata5[15];
    char stringdata6[9];
    char stringdata7[7];
    char stringdata8[5];
    char stringdata9[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_AppLock_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_AppLock_t qt_meta_stringdata_AppLock = {
    {
        QT_MOC_LITERAL(0, 7),  // "AppLock"
        QT_MOC_LITERAL(8, 11),  // "passcodeSet"
        QT_MOC_LITERAL(20, 0),  // ""
        QT_MOC_LITERAL(21, 8),  // "unlocked"
        QT_MOC_LITERAL(30, 6),  // "locked"
        QT_MOC_LITERAL(37, 14),  // "verifyPasscode"
        QT_MOC_LITERAL(52, 8),  // "passcode"
        QT_MOC_LITERAL(61, 6),  // "unlock"
        QT_MOC_LITERAL(68, 4),  // "lock"
        QT_MOC_LITERAL(73, 11)   // "setPasscode"
    },
    "AppLock",
    "passcodeSet",
    "",
    "unlocked",
    "locked",
    "verifyPasscode",
    "passcode",
    "unlock",
    "lock",
    "setPasscode"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_AppLock[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x06,    1 /* Public */,
       3,    0,   57,    2, 0x06,    2 /* Public */,
       4,    0,   58,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,   59,    2, 0x0a,    4 /* Public */,
       7,    0,   62,    2, 0x0a,    6 /* Public */,
       8,    0,   63,    2, 0x0a,    7 /* Public */,
       9,    1,   64,    2, 0x0a,    8 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject AppLock::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AppLock.offsetsAndSizes,
    qt_meta_data_AppLock,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_AppLock_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AppLock, std::true_type>,
        // method 'passcodeSet'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unlocked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'locked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'verifyPasscode'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'unlock'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'lock'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setPasscode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void AppLock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AppLock *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->passcodeSet(); break;
        case 1: _t->unlocked(); break;
        case 2: _t->locked(); break;
        case 3: { bool _r = _t->verifyPasscode((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->unlock(); break;
        case 5: _t->lock(); break;
        case 6: _t->setPasscode((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AppLock::*)();
            if (_t _q_method = &AppLock::passcodeSet; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AppLock::*)();
            if (_t _q_method = &AppLock::unlocked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AppLock::*)();
            if (_t _q_method = &AppLock::locked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *AppLock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppLock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AppLock.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppLock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void AppLock::passcodeSet()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AppLock::unlocked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AppLock::locked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
