/****************************************************************************
** Meta object code from reading C++ file 'serveur.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/qt/serveur.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serveur.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Serveur[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,    9,    8,    8, 0x05,
      42,    8,    8,    8, 0x05,
      52,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      64,    8,    8,    8, 0x0a,
      78,    8,    8,    8, 0x0a,
     120,    8,    8,    8, 0x0a,
     132,    8,    8,    8, 0x0a,
     144,  140,    8,    8, 0x0a,
     167,  162,    8,    8, 0x0a,
     181,  162,    8,    8, 0x0a,
     217,  196,    8,    8, 0x0a,
     262,  249,    8,    8, 0x2a,
     286,  140,    8,    8, 0x2a,
     315,  302,    8,    8, 0x0a,
     348,  162,    8,    8, 0x2a,
     373,    8,    8,    8, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_Serveur[] = {
    "Serveur\0\0newPseudo\0pseudoChanged(QString)\0"
    "joinTab()\0tabJoined()\0readServeur()\0"
    "errorSocket(QAbstractSocket::SocketError)\0"
    "connected()\0joins()\0txt\0sendData(QString)\0"
    "chan\0join(QString)\0leave(QString)\0"
    "txt,destChan,msgTray\0"
    "ecrire(QString,QString,QString)\0"
    "txt,destChan\0ecrire(QString,QString)\0"
    "ecrire(QString)\0chan,message\0"
    "updateUsersList(QString,QString)\0"
    "updateUsersList(QString)\0updateUsersList()\0"
};

void Serveur::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Serveur *_t = static_cast<Serveur *>(_o);
        switch (_id) {
        case 0: _t->pseudoChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->joinTab(); break;
        case 2: _t->tabJoined(); break;
        case 3: _t->readServeur(); break;
        case 4: _t->errorSocket((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 5: _t->connected(); break;
        case 6: _t->joins(); break;
        case 7: _t->sendData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->join((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->leave((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->ecrire((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 11: _t->ecrire((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 12: _t->ecrire((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 13: _t->updateUsersList((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 14: _t->updateUsersList((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 15: _t->updateUsersList(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Serveur::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Serveur::staticMetaObject = {
    { &QTcpSocket::staticMetaObject, qt_meta_stringdata_Serveur,
      qt_meta_data_Serveur, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Serveur::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Serveur::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Serveur::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Serveur))
        return static_cast<void*>(const_cast< Serveur*>(this));
    return QTcpSocket::qt_metacast(_clname);
}

int Serveur::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpSocket::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void Serveur::pseudoChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Serveur::joinTab()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Serveur::tabJoined()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
