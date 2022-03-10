//
// Created by Алексей Сиротин on 17.01.2022.
//

#include "async_tcp_socket.h"

AsyncTcpSocket::AsyncTcpSocket(QObject * parent) : QTcpSocket(parent) {}

void AsyncTcpSocket::onConnect(const QHostAddress &address, quint16 port) {
	connectToHost(address, port);
}
