#ifndef CATS_ENUMS_H
#define CATS_ENUMS_H

#include <QObject>

class DataType : public QObject {
public:
    enum Enum {
        BYTES = 0x0,
        JSON = 0x1,
        FILES = 0x2
    };

    Q_OBJECT
    Q_ENUM(Enum)
};

class CompressionType : public QObject {
public:
    enum Enum {
        NONE = 0x0,
        ZLIB = 0x2
    };

    Q_OBJECT
    Q_ENUM(Enum)
};

class HeaderType : public QObject {
public:
    enum Enum {
        EMPTY = -1,
        BASIC = 0x0,
        STREAM = 0x1,
        CHILDREN = 0x2,
        SPEED_LIMIT = 0x5,
        CANCEL_INPUT = 0x6,
        PING_PONG = 0xFF
    };

    Q_OBJECT
    Q_ENUM(Enum)
};

class CatsStatus : public QObject {
public:
    enum Enum {
        DISCONNECTED,
        CONNECTED,
        RECONNECTION
    };

    Q_OBJECT
    Q_ENUM(Enum)
};

#endif // CATS_ENUMS_H
