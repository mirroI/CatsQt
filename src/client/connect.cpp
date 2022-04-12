#include "connect.h"

CatsConnect::CatsConnect(AsyncTcpSocket *tcpSocket, quint32 apiVersion, QString secretKey, QObject *parent) : QObject(parent) {
	CatsConnect::tcpSocket = tcpSocket;
    this->apiVersion = apiVersion;
    this->secretKey = secretKey;
}

void CatsConnect::ping() {
    writeQueue.append(new CatsSystemRequest(new CatsPingPongHeader()));
}

bool CatsConnect::isConnected() const {
    return status == CatsStatus::CONNECTED;
}

Mono<CatsAbstractResponse> *CatsConnect::sendMessage(CatsAbstractRequest *abstractRequest) {
	Mono<CatsAbstractResponse> *mono = nullptr;

    if (CatsRequest *request = dynamic_cast<CatsRequest *>(abstractRequest)) {
        if (request->getHeader()->getHeaderType() != HeaderType::CHILDREN) {
            request->getHeader()->setMessageId(incrementMessageId);
            incrementMessageId++;

            if (incrementMessageId == ID_LIMIT) {
                incrementMessageId = 0;
            }
        }

		mono = Mono<CatsAbstractResponse>::create([=](MonoSink<CatsAbstractResponse> *monoSink) {
			request->setMonoSink(monoSink);
		});
        waitingResponces.insert(request->getHeader()->getMessageId(), request);
    } else {

	}

    writeQueue.append(abstractRequest);

	return mono;
}

QDateTime CatsConnect::getServerDateTime() const {
    return QDateTime::currentDateTime().addMSecs(timeDeference);
}

void CatsConnect::reconnect() {
    connect(tcpSocket, &QTcpSocket::readyRead, this, &CatsConnect::onServerProtocolVersionRead);

//    tcpSocket->connectToHost(this->hostAddress, this->port);
}

//Убираем из отправленных байт длину заголовка
void CatsConnect::writeInitial(CatsAbstractRequest *request) {
    this->request = request;

    switch (request->getHeader()->getHeaderType()) {
        case HeaderType::BASIC: {
            requestBytesSent = -19;
            break;
        } case HeaderType::STREAM: {
            requestBytesSent = -15;
            break;
        } case HeaderType::CHILDREN: {
            requestBytesSent = -9;
            break;
        } case HeaderType::SPEED_LIMIT: {
            requestBytesSent = -5;
            break;
        } case HeaderType::CANCEL_INPUT: {
            requestBytesSent = -3;
            break;
        } case HeaderType::PING_PONG: {
            requestBytesSent = -9;
            break;
        }
    }
}

void CatsConnect::writeFileChunk() {
    CatsFilesRequest *fileRequest = dynamic_cast<CatsFilesRequest *>(this->request);

    QByteArray byteBuf;
    quint32 availableSize = FILE_MAX_CHUNK_SIZE - byteBuf.size();
    while (availableSize > 0 && fileRequest->getWriteFileId() < fileRequest->getFiles().size()) {
        CatsFileInfo *fileInfo = fileRequest->getFiles().at(fileRequest->getWriteFileId());
        QFile *file = fileInfo->getFile();

        quint32 fileAvailableReadSize = file->size() - file->pos();
        if (fileAvailableReadSize < availableSize) {
            byteBuf.append(file->read(fileAvailableReadSize));
            file->close();
            fileRequest->setWriteFileId(fileRequest->getWriteFileId() + 1);
        } else {
            byteBuf.append(file->read(availableSize));
        }

        availableSize = FILE_MAX_CHUNK_SIZE - byteBuf.size();
    }

    tcpSocket->write(byteBuf);
}

bool CatsConnect::readHeader() {
    QDataStream in(tcpSocket);

   if (headerType == HeaderType::BASIC && tcpSocket->size() >= 18) {
       header = new CatsBasicHeader(in);
   } else if(headerType == HeaderType::STREAM && tcpSocket->size() >= 14) {
       header = new CatsStreamingHeader(in);
   } else if (headerType == HeaderType::CHILDREN && tcpSocket->size() >= 8) {
       header = new CatsInputHeader(in);
   } else if (this->headerType == HeaderType::SPEED_LIMIT && tcpSocket->size() >= 4) {
       header = new CatsSpeedLimiterHeader(in);
   } else if (this->headerType == HeaderType::CANCEL_INPUT && tcpSocket->size() >= 2) {
       header = new CatsImputCancellingHeader(in);
   } else if (this->headerType == HeaderType::PING_PONG && tcpSocket->size() >= 8) {
       header = new CatsPingPongHeader(in);
   }

   return header != nullptr;
}

bool CatsConnect::readMessageHeader() {
    if (this->headerType == HeaderType::BASIC || this->headerType == HeaderType::CHILDREN) {
        CatsInputHeader *inputHeader = dynamic_cast<CatsInputHeader *>(header);
        QByteArray byteArray = tcpSocket->peek(inputHeader->getDataLenght());

        messageHeaderSize = byteArray.indexOf(MESSAGE_HEADER_SPLITTER);
        if (messageHeaderSize >= 0) {
            messageHeader = new QJsonObject(QJsonDocument::fromJson(tcpSocket->read(messageHeaderSize)).object());
            tcpSocket->read(MESSAGE_HEADER_SPLITTER.size());

            return true;
        }
    } else {
        if (tcpSocket->size() >= 4) {
            messageHeaderSize = tcpSocket->peek(4).toInt() + 4;

            if (tcpSocket->size() >= messageHeaderSize) {
                tcpSocket->read(4);
                messageHeader = new QJsonObject(QJsonDocument::fromJson(tcpSocket->read(messageHeaderSize)).object());

                return true;
            }
        }
    }

    return false;
}

bool CatsConnect::initCatsByteBuf() {
    if (this->headerType == HeaderType::BASIC || this->headerType == HeaderType::CHILDREN) {
        CatsInputHeader *header = dynamic_cast<CatsInputHeader *>(this->header);

        catsByteBuf = new CatsMessageByteBuf(header->getCompressionType(), this);
        catsByteBuf->initial(header->getDataLenght() - messageHeaderSize - MESSAGE_HEADER_SPLITTER.size());

        return true;
    } else if (this->headerType == HeaderType::STREAM && tcpSocket->size() >= 4) {
        CatsStreamingHeader *header = dynamic_cast<CatsStreamingHeader *>(this->header);
        QDataStream in(tcpSocket);

        quint32 chunkDataLenght;
        in >> chunkDataLenght;

        catsByteBuf = new CatsStreamByteBuf(header->getCompressionType(), this);
        catsByteBuf->initial(chunkDataLenght);

        return true;
    }

    return false;
}

void CatsConnect::responseMessageCollected() {
    CatsAbstractResponse *response;

    if (this->headerType == HeaderType::BASIC || this->headerType == HeaderType::CHILDREN || this->headerType == HeaderType::STREAM) {
        if (CatsHeader *header = dynamic_cast<CatsHeader *>(this->header)) {
            if (header->getDataType() == DataType::FILES) {

            } else {
                QByteArray byteBuf;
                if (catsByteBuf != nullptr) {
                    byteBuf = catsByteBuf->read();
                }

                response = new CatsBasicResponse(header, *messageHeader, byteBuf);
            }

            if (waitingResponces.contains(header->getMessageId())) {
                waitingResponces.value(header->getMessageId())->getMonoSink()->success(response);
            } else if (header->getMessageId() > ID_LIMIT) {
                qDebug() << QString("CATS broadcast %0 %1")
                            .arg(QString("0x%0").arg(static_cast<CatsBasicHeader *>(response->getHeader())->getHandlerId(), 4, 16, QLatin1Char('0')).toUpper())
                            .arg(static_cast<CatsBasicResponse *>(response)->getData());
                Q_EMIT broadcast(static_cast<CatsResponse *>(response));
            }
        }
    } else if (headerType == HeaderType::PING_PONG) {
        if (CatsPingPongHeader *header = dynamic_cast<CatsPingPongHeader *>(this->header)) {
            qDebug() << header->getTime();
        }
    }

    delete catsByteBuf;

    headerType = HeaderType::EMPTY;
    header = nullptr;
    messageHeader = nullptr;
    catsByteBuf = nullptr;
}

void CatsConnect::writeNextMessage() {
    if (this->request == nullptr && tcpSocket->isValid() && this->status == CatsStatus::CONNECTED) {
        if (writeQueue.isEmpty() && QDateTime::currentMSecsSinceEpoch() - lastWritingAt > PING_AT) {
            return ping();
        }

        if (!writeQueue.isEmpty()) {
            CatsAbstractRequest *request = writeQueue.dequeue();
            CatsAbstractHeader *header = request->getHeader();

            switch (header->getHeaderType()) {
                case HeaderType::BASIC: {
                    static_cast<CatsBasicHeader *>(header)->setTime(getServerDateTime().currentMSecsSinceEpoch());
                    break;
                } case HeaderType::STREAM: {
                    static_cast<CatsStreamingHeader *>(header)->setTime(getServerDateTime().currentMSecsSinceEpoch());
                    break;
                } case HeaderType::PING_PONG: {
                    static_cast<CatsPingPongHeader *>(header)->setTime(getServerDateTime().currentMSecsSinceEpoch());
                    break;
                }
            }

            writeInitial(request);
            tcpSocket->write(request->toByteBuf());
        }
    }
}

void CatsConnect::onConnected() {
    QByteArray byteBuf;
    QDataStream out(&byteBuf, QIODevice::WriteOnly);

    out << (quint32) CATS_VERSION;

    tcpSocket->write(byteBuf);
}

void CatsConnect::onServerProtocolVersionRead() {
    quint32 serverCatsVersion;

    QDataStream in(tcpSocket);
    in >> serverCatsVersion;

    if (serverCatsVersion == CATS_SERVER_VALID_VERSION) {
        disconnect(tcpSocket, &QTcpSocket::readyRead, this, &CatsConnect::onServerProtocolVersionRead);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &CatsConnect::onStatementResponseRead);


        QByteArray byteBuf;
        QDataStream out(&byteBuf, QIODevice::WriteOnly);
        StatementRequest statement(apiVersion, QDateTime::currentMSecsSinceEpoch());

        out << statement.toJson();

        tcpSocket->write(byteBuf);
    } else {
        qDebug() << "PROTOCOL VERSION NOT VALID";
    }
}

void CatsConnect::onStatementResponseRead() {

    quint32 statementLength = 0;

    if (tcpSocket->size() >= 4) {
        QDataStream in(tcpSocket);
        in >> statementLength;
    }

    if (statementLength > 0 && tcpSocket->size() >= statementLength) {
        disconnect(tcpSocket, &QTcpSocket::readyRead, this, &CatsConnect::onStatementResponseRead);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &CatsConnect::onHandshakeResponseRead);

        StatementResponse statement(tcpSocket->read(statementLength));
        this->timeDeference = QDateTime::currentDateTime().msecsTo(QDateTime::fromMSecsSinceEpoch(statement.getServerTime()));

        qint64 time = getServerDateTime().toSecsSinceEpoch() / 10 * 10;
        QByteArray handshake = QCryptographicHash::hash(secretKey.toUtf8() + QByteArray::number(time), QCryptographicHash::Sha256);
        tcpSocket->write(handshake);
    }
}

void CatsConnect::onHandshakeResponseRead() {
    disconnect(tcpSocket, &QTcpSocket::readyRead, this, &CatsConnect::onHandshakeResponseRead);

    bool handshakeStatus;

    QDataStream in(tcpSocket);
    in >> handshakeStatus;

    qInfo() << "CATS Handshake status: " << handshakeStatus;

    if (handshakeStatus) {
        if (this->status == CatsStatus::RECONNECTION) {
            Q_EMIT reconnected();
        } else {
            Q_EMIT connected();
        }

        this->status = CatsStatus::CONNECTED;
        connect(tcpSocket, &QTcpSocket::readyRead, this, &CatsConnect::onRead);

        lastWritingAt = QDateTime::currentMSecsSinceEpoch();
        eventLoop = new QTimer(this);
        connect(eventLoop, &QTimer::timeout, this, &CatsConnect::writeNextMessage);
        eventLoop->start(100);
    }
}

//Метод срабатывает при получении отправленного пакета сервером, если запрос состоял из байтов или json`а, метод освободит поток.
//Если запрос состоял из заголовка файлов, то начнут отправлятся файлы.
void CatsConnect::onBytesWritten(qint64 bytesSent) {
    requestBytesSent += bytesSent;
    lastWritingAt = QDateTime::currentMSecsSinceEpoch();

    if (this->request != nullptr) {
        HeaderType::Enum headerType = this->request->getHeader()->getHeaderType();

        if (headerType == HeaderType::BASIC || headerType == HeaderType::CHILDREN) {
            if (CatsRequest *request = dynamic_cast<CatsRequest *>(this->request)) {
                CatsInputHeader *header = static_cast<CatsInputHeader *>(request->getHeader());

                request->emitWriteProgressHandler(requestBytesSent, header->getDataLenght());

                if (header->getDataType() == DataType::FILES) {
                    writeFileChunk();
                }

                if (this->requestBytesSent == header->getDataLenght()) {
                    this->requestBytesSent = 0;
                    this->request = nullptr;
                }
            }
        } else if (headerType == HeaderType::SPEED_LIMIT || headerType == HeaderType::CANCEL_INPUT || headerType == HeaderType::PING_PONG) {
            if (this->requestBytesSent == 0) {
                this->request = nullptr;
            }
        }
    }
}

void CatsConnect::onRead() {
    QDataStream in(tcpSocket);

    if (tcpSocket->size() >= 1) {
        if (this->headerType == HeaderType::EMPTY) {
            quint8 headerType;
            in >> headerType;

            this->headerType = static_cast<HeaderType::Enum>(headerType);
        }

        if (this->header == nullptr) {
            if (readHeader()) {
                if (this->headerType == HeaderType::SPEED_LIMIT || this->headerType == HeaderType::CANCEL_INPUT || this->headerType == HeaderType::PING_PONG) {
                    responseMessageCollected();
                    return;
                }
            } else {
                return;
            }
        }

        if (this->messageHeader == nullptr) {
            if (!readMessageHeader()) {
                return;
            }

            if (this->headerType == HeaderType::BASIC || this->headerType == HeaderType::CHILDREN) {
                CatsInputHeader *header = static_cast<CatsInputHeader *>(this->header);

                if (header->getDataLenght() - this->messageHeaderSize - MESSAGE_HEADER_SPLITTER.size() == 0) {
                    return responseMessageCollected();
                }
            }
        }

        if (catsByteBuf == nullptr) {
            if (!initCatsByteBuf()) {
                return;
            }
        }

        if (catsByteBuf->isCanWrite()) {
            if (catsByteBuf->getEmptyLenght() > 0) {
                catsByteBuf->write(tcpSocket);

                if (!catsByteBuf->isCanWrite()) {
                    responseMessageCollected();
                }
            } else if (tcpSocket->size() >= 4) {
                quint32 chunkDataLenght;
                in >> chunkDataLenght;

                catsByteBuf->initial(chunkDataLenght);
                if (!catsByteBuf->isCanWrite()) {
                    qDebug() << "Cats end read stream" << catsByteBuf->read();
                    //stream collected
                }
            }
        } else {

        }
    }

    if (tcpSocket->size() > 0) {
        onRead();
    }
}

void CatsConnect::onDisconnected() {
    if (status == CatsStatus::CONNECTED) {
        status = CatsStatus::RECONNECTION;
    }

    disconnect(tcpSocket, &QTcpSocket::readyRead, this, &CatsConnect::onRead);

    if (eventLoop != nullptr) {
        eventLoop->stop();
        delete eventLoop;
        eventLoop = nullptr;
    }

    QTimer::singleShot(20000, [=]() {
        reconnect();
    });
}
