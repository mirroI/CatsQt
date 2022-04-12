/*
CATS Client отвечает отправку сообщений на сервер
*/

#ifndef CATSCONNECT_H
#define CATSCONNECT_H

#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QQueue>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QDataStream>
#include <QThreadPool>
#include <QHostAddress>
#include <QCryptographicHash>

#include "mono.h"

#include "async_tcp_socket.h"
#include "models.h"
#include "../byte_buf.h"

#define CATS_VERSION 0x0002
#define CATS_SERVER_VALID_VERSION 0x0000

#define ID_LIMIT 0x8000
#define PING_AT 90000

#define FILE_MAX_CHUNK_SIZE 65536

class CatsConnect : public QObject {
 Q_OBJECT

 private:
	const QByteArray MESSAGE_HEADER_SPLITTER = QByteArray(2, 0);

	AsyncTcpSocket *tcpSocket = nullptr;
	CatsStatus::Enum status = CatsStatus::DISCONNECTED;

	unsigned int apiVersion;
	QString secretKey;

	qint64 timeDeference;
	quint64 lastWritingAt;

	quint16 mPing;

	QQueue<CatsAbstractRequest *> writeQueue;
	QMap<quint16, CatsRequest *> waitingResponces;

	quint16 incrementMessageId = 0;

	CatsAbstractRequest *request = nullptr;
	qint64 requestBytesSent = 0;
	QTimer *eventLoop = nullptr;

	HeaderType::Enum headerType = HeaderType::EMPTY;
	CatsAbstractHeader *header = nullptr;
	qint32 messageHeaderSize;
	QJsonObject *messageHeader = nullptr;
	CatsByteBuf *catsByteBuf = nullptr;

 public:
	CatsConnect(AsyncTcpSocket *tcpSocket, quint32 apiVersion, QString secretKey, QObject *parent);

	void ping();

 public:
	bool isConnected() const;

	Mono<CatsAbstractResponse> *sendMessage(CatsAbstractRequest *);

	QDateTime getServerDateTime() const;

 private:
	void reconnect();

	void writeInitial(CatsAbstractRequest *);
	void writeFileChunk();

	bool readHeader();
	bool readMessageHeader();
	bool initCatsByteBuf();

	void responseMessageCollected();
	bool streamCollected();

 public Q_SLOTS:
	void writeNextMessage();

	void onConnected();
	void onServerProtocolVersionRead();
	void onStatementResponseRead();
	void onHandshakeResponseRead();
	void onBytesWritten(qint64);
	void onRead();
	void onDisconnected();

 Q_SIGNALS:
	void connected();
	void reconnected();
	void disconnected();

	void broadcast(CatsResponse *);
};

#endif // CATSCONNECT_H
