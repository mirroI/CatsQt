#include "client.h"

CatsClient::CatsClient(quint32 apiVersion, QString secretKey, QString ip, quint16 port, QObject *parent) : QObject(parent) {
	CatsClient::hostAddress = QHostAddress(ip);
	CatsClient::port = port;

	AsyncTcpSocket *tcpSocket = new AsyncTcpSocket(nullptr);
	catsConnect = new CatsConnect(tcpSocket, apiVersion, secretKey, nullptr);

	QThread *thread1 = new QThread;
	QThread *thread2 = new QThread;
	tcpSocket->moveToThread(thread1);
	catsConnect->moveToThread(thread1);
	thread1->start();
	thread2->start();

	QObject::connect(tcpSocket, &QTcpSocket::connected, catsConnect, &CatsConnect::onConnected);
	QObject::connect(tcpSocket, &QTcpSocket::bytesWritten, catsConnect, &CatsConnect::onBytesWritten);
	QObject::connect(tcpSocket, &QTcpSocket::readyRead, catsConnect, &CatsConnect::onServerProtocolVersionRead);
	QObject::connect(tcpSocket, &QTcpSocket::disconnected, catsConnect, &CatsConnect::onDisconnected);

	QObject::connect(this, &CatsClient::connect, tcpSocket, &AsyncTcpSocket::onConnect);
	Q_EMIT connect(CatsClient::hostAddress, CatsClient::port);
}

Mono<CatsAbstractResponse> *CatsClient::sendMessage(CatsAbstractRequest *request) {
	return catsConnect->sendMessage(request);
}

void CatsClient::cancelInput(CatsRequest *canceledRequest) {
	qint16 messageId = canceledRequest->getHeader()->getMessageId();
	CatsSystemRequest *request = new CatsSystemRequest(new CatsImputCancellingHeader(messageId));

	catsConnect->sendMessage(request);
}
