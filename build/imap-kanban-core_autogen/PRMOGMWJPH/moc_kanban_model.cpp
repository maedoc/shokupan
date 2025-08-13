/****************************************************************************
** Meta object code from reading C++ file 'kanban_model.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/core/kanban_model.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kanban_model.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_KanbanModel_t {
    uint offsetsAndSizes[40];
    char stringdata0[12];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[13];
    char stringdata4[6];
    char stringdata5[8];
    char stringdata6[17];
    char stringdata7[15];
    char stringdata8[8];
    char stringdata9[10];
    char stringdata10[4];
    char stringdata11[12];
    char stringdata12[10];
    char stringdata13[12];
    char stringdata14[12];
    char stringdata15[16];
    char stringdata16[19];
    char stringdata17[20];
    char stringdata18[12];
    char stringdata19[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_KanbanModel_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_KanbanModel_t qt_meta_stringdata_KanbanModel = {
    {
        QT_MOC_LITERAL(0, 11),  // "KanbanModel"
        QT_MOC_LITERAL(12, 9),  // "connected"
        QT_MOC_LITERAL(22, 0),  // ""
        QT_MOC_LITERAL(23, 12),  // "disconnected"
        QT_MOC_LITERAL(36, 5),  // "error"
        QT_MOC_LITERAL(42, 7),  // "message"
        QT_MOC_LITERAL(50, 16),  // "mailboxesChanged"
        QT_MOC_LITERAL(67, 14),  // "mailboxUpdated"
        QT_MOC_LITERAL(82, 7),  // "mailbox"
        QT_MOC_LITERAL(90, 9),  // "cardMoved"
        QT_MOC_LITERAL(100, 3),  // "uid"
        QT_MOC_LITERAL(104, 11),  // "fromMailbox"
        QT_MOC_LITERAL(116, 9),  // "toMailbox"
        QT_MOC_LITERAL(126, 11),  // "cardDeleted"
        QT_MOC_LITERAL(138, 11),  // "cardUpdated"
        QT_MOC_LITERAL(150, 15),  // "onImapConnected"
        QT_MOC_LITERAL(166, 18),  // "onImapDisconnected"
        QT_MOC_LITERAL(185, 19),  // "onImapAuthenticated"
        QT_MOC_LITERAL(205, 11),  // "onImapError"
        QT_MOC_LITERAL(217, 18)   // "onAutoRefreshTimer"
    },
    "KanbanModel",
    "connected",
    "",
    "disconnected",
    "error",
    "message",
    "mailboxesChanged",
    "mailboxUpdated",
    "mailbox",
    "cardMoved",
    "uid",
    "fromMailbox",
    "toMailbox",
    "cardDeleted",
    "cardUpdated",
    "onImapConnected",
    "onImapDisconnected",
    "onImapAuthenticated",
    "onImapError",
    "onAutoRefreshTimer"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_KanbanModel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x06,    1 /* Public */,
       3,    0,   93,    2, 0x06,    2 /* Public */,
       4,    1,   94,    2, 0x06,    3 /* Public */,
       6,    0,   97,    2, 0x06,    5 /* Public */,
       7,    1,   98,    2, 0x06,    6 /* Public */,
       9,    3,  101,    2, 0x06,    8 /* Public */,
      13,    2,  108,    2, 0x06,   12 /* Public */,
      14,    2,  113,    2, 0x06,   15 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    0,  118,    2, 0x08,   18 /* Private */,
      16,    0,  119,    2, 0x08,   19 /* Private */,
      17,    0,  120,    2, 0x08,   20 /* Private */,
      18,    1,  121,    2, 0x08,   21 /* Private */,
      19,    0,  124,    2, 0x08,   23 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   10,   11,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   10,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   10,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject KanbanModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KanbanModel.offsetsAndSizes,
    qt_meta_data_KanbanModel,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_KanbanModel_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<KanbanModel, std::true_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'mailboxesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mailboxUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'cardMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'cardDeleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'cardUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onImapConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImapDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImapAuthenticated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImapError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onAutoRefreshTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void KanbanModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KanbanModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->error((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->mailboxesChanged(); break;
        case 4: _t->mailboxUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->cardMoved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 6: _t->cardDeleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->cardUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->onImapConnected(); break;
        case 9: _t->onImapDisconnected(); break;
        case 10: _t->onImapAuthenticated(); break;
        case 11: _t->onImapError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->onAutoRefreshTimer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KanbanModel::*)();
            if (_t _q_method = &KanbanModel::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KanbanModel::*)();
            if (_t _q_method = &KanbanModel::disconnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KanbanModel::*)(const QString & );
            if (_t _q_method = &KanbanModel::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (KanbanModel::*)();
            if (_t _q_method = &KanbanModel::mailboxesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (KanbanModel::*)(const QString & );
            if (_t _q_method = &KanbanModel::mailboxUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (KanbanModel::*)(const QString & , const QString & , const QString & );
            if (_t _q_method = &KanbanModel::cardMoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (KanbanModel::*)(const QString & , const QString & );
            if (_t _q_method = &KanbanModel::cardDeleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (KanbanModel::*)(const QString & , const QString & );
            if (_t _q_method = &KanbanModel::cardUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject *KanbanModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KanbanModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KanbanModel.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KanbanModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void KanbanModel::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KanbanModel::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void KanbanModel::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KanbanModel::mailboxesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void KanbanModel::mailboxUpdated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void KanbanModel::cardMoved(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void KanbanModel::cardDeleted(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void KanbanModel::cardUpdated(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
