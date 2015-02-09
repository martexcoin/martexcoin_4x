/****************************************************************************
** Meta object code from reading C++ file 'chatwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/qt/chatwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chatwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ChatWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      24,   11,   11,   11, 0x0a,
      39,   11,   11,   11, 0x0a,
      50,   11,   11,   11, 0x0a,
      67,   61,   11,   11, 0x0a,
      83,   11,   11,   11, 0x0a,
      95,   11,   11,   11, 0x0a,
     108,   11,   11,   11, 0x0a,
     131,   61,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChatWindow[] = {
    "ChatWindow\0\0changeTab()\0sendCommande()\0"
    "connecte()\0closeTab()\0index\0tabChanged(int)\0"
    "tabJoined()\0tabJoining()\0"
    "disconnectFromServer()\0tabClosing(int)\0"
};

void ChatWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChatWindow *_t = static_cast<ChatWindow *>(_o);
        switch (_id) {
        case 0: _t->changeTab(); break;
        case 1: _t->sendCommande(); break;
        case 2: _t->connecte(); break;
        case 3: _t->closeTab(); break;
        case 4: _t->tabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->tabJoined(); break;
        case 6: _t->tabJoining(); break;
        case 7: _t->disconnectFromServer(); break;
        case 8: _t->tabClosing((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ChatWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChatWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ChatWindow,
      qt_meta_data_ChatWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChatWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChatWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChatWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChatWindow))
        return static_cast<void*>(const_cast< ChatWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int ChatWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void ChatWindow::changeTab()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
