#ifndef CATS_HEADERS_H
#define CATS_HEADERS_H

#include <QDebug>
#include <QDataStream>

#include "enums.h"

class CatsAbstractHeader {
protected:
    HeaderType::Enum headerType;

public:
    HeaderType::Enum getHeaderType() const;

    virtual QByteArray toByteBuf() = 0;
};

class CatsHeader : public CatsAbstractHeader {
protected:
    quint16 messageId;
    DataType::Enum dataType;
    CompressionType::Enum compressionType;

    CatsHeader() {};
    CatsHeader(DataType::Enum, CompressionType::Enum);

public:
    quint16 getMessageId() const;
    void setMessageId(quint16 value);
    DataType::Enum getDataType() const;
    CompressionType::Enum getCompressionType() const;
};

class CatsInputHeader : public CatsHeader {
protected:
    quint32 dataLenght;

    CatsInputHeader() {};
    CatsInputHeader(DataType::Enum, CompressionType::Enum);

public:
    CatsInputHeader(CatsHeader *, DataType::Enum, CompressionType::Enum);
    CatsInputHeader(QDataStream &);

    quint32 getDataLenght() const;
    void setDataLenght(const quint32 &value);

    QByteArray toByteBuf() override;
};

class CatsBasicHeader : public CatsInputHeader {
private:
    quint16 handlerId;
    quint64 time;

public:
    CatsBasicHeader(quint16, DataType::Enum, CompressionType::Enum);
    CatsBasicHeader(QDataStream &);

    quint16 getHandlerId() const;
    quint64 getTime() const;
    void setTime(const quint64 &value);

    QByteArray toByteBuf() override;
};

class CatsStreamingHeader : public CatsHeader {
protected:
    quint16 handlerId;
    quint64 time;

public:
    CatsStreamingHeader(quint16, DataType::Enum, CompressionType::Enum);
    CatsStreamingHeader(QDataStream &);

    quint16 getHandlerId() const;
    quint64 getTime() const;
    void setTime(const quint64 &value);

    QByteArray toByteBuf() override;
};

class CatsSpeedLimiterHeader : public CatsAbstractHeader {
private:
    quint32 bytes;

public:
    CatsSpeedLimiterHeader(quint32);
    CatsSpeedLimiterHeader(QDataStream &);

    QByteArray toByteBuf() override;
};

class CatsImputCancellingHeader : public CatsAbstractHeader {
private:
    quint16 handlerId;

public:
    CatsImputCancellingHeader(quint16);
    CatsImputCancellingHeader(QDataStream &);

    QByteArray toByteBuf() override;
};

class CatsPingPongHeader : public CatsAbstractHeader {
private:
    quint64 time;

public:
    CatsPingPongHeader();
    CatsPingPongHeader(QDataStream &);

    quint64 getTime() const;
    void setTime(quint64 newTime);

    QByteArray toByteBuf() override;
};

#endif // CATS_HEADERS_H
