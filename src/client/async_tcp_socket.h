//
// Created by Алексей Сиротин on 17.01.2022.
//

#ifndef CATS_SRC_CLIENT_ASYNC_TCP_SOCKET_H
#define CATS_SRC_CLIENT_ASYNC_TCP_SOCKET_H

#include <QTcpSocket>

class AsyncTcpSocket : public QTcpSocket {
 public:
	AsyncTcpSocket(QObject *parent);

 public Q_SLOTS:
	void onConnect(const QHostAddress &address, quint16 port);
};

#endif //CATS_SRC_CLIENT_ASYNC_TCP_SOCKET_H
