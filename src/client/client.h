/*
CATS Client отвечает за обработку сообщений с сервера, и является основным классом CATS
*/

#ifndef CATSCLIENT_H
#define CATSCLIENT_H

//#include "async_tcp_socket.h"
//#include "connect.h"
//#include "models.h"
//#include "../byte_buf.h"

#include <QObject>

class CatsClient : public QObject {
 Q_OBJECT

 private:
//	QHostAddress hostAddress;
//	quint16 port;
//
//	CatsConnect *catsConnect;

 public:
	explicit CatsClient(quint32 apiVersion, QString secretKey, QString ip, quint16 port, QObject *parent = nullptr);

//	void sendMessage(CatsAbstractRequest *abstractRequest);
//	void cancelInput(CatsRequest *canceledRequest);
//
// Q_SIGNALS:
//	void connect(const QHostAddress& address, quint16 port);
};

#endif // CATSCLIENT_H
