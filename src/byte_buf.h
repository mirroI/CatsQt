#ifndef CATSBYTEBUF_H
#define CATSBYTEBUF_H

#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QJsonObject>
#include <QDataStream>
#include <QTemporaryFile>

#include "client/models.h"

#define MAX_BYTE_BUF_SIZE 32000

class CatsByteBuf : public QObject {
    Q_OBJECT
protected:
    CompressionType::Enum compressionType;
    bool canWrite = true;
    quint32 emptyLenght = 0;

protected:
    CatsByteBuf(CompressionType::Enum, QObject *parent = nullptr);

public:
    virtual void initial(const qint32 &) = 0;
    virtual quint32 write(QTcpSocket *) = 0;
    virtual QByteArray read() const = 0;

    bool isCanWrite() const;
    quint32 getEmptyLenght() const;

Q_SIGNALS:
    void messageCollected(QByteArray &);
};

class CatsMessageByteBuf : public CatsByteBuf {
    Q_OBJECT
private:
    QTemporaryFile *fileBuf = nullptr;
    QByteArray arrayBuf;

    qint32 bufferSize;

public:
    CatsMessageByteBuf(CompressionType::Enum, QObject *parent = nullptr);

    void initial(const qint32 &);
    quint32 write(QTcpSocket *);
    QByteArray read() const;
};

class CatsStreamByteBuf : public CatsByteBuf {
    Q_OBJECT

private:
    int chunk = -1;
    QList<CatsMessageByteBuf *> chunks;

public:
    CatsStreamByteBuf(CompressionType::Enum, QObject *parent = nullptr);

    void initial(const qint32 &);
    quint32 write(QTcpSocket *);
    QByteArray read() const;
};

#endif // CATSBYTEBUF_H
