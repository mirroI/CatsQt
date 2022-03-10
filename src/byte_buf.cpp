#include "byte_buf.h"

CatsByteBuf::CatsByteBuf(CompressionType::Enum compressionType, QObject *parent) : QObject(parent) {
    this->compressionType = compressionType;
}

bool CatsByteBuf::isCanWrite() const {
    return canWrite;
}

quint32 CatsByteBuf::getEmptyLenght() const {
    return emptyLenght;
}

CatsMessageByteBuf::CatsMessageByteBuf(CompressionType::Enum compressionType, QObject *parent) : CatsByteBuf(compressionType, parent) {}

void CatsMessageByteBuf::initial(const qint32 &bufferSize) {
    if (bufferSize > MAX_BYTE_BUF_SIZE) {
        fileBuf = new QTemporaryFile("./temp/XXXXXX.cats");
        fileBuf->open();

        fileBuf->resize(bufferSize);
    }

    this->emptyLenght = bufferSize;
    this->bufferSize = bufferSize;
}

quint32 CatsMessageByteBuf::write(QTcpSocket *tcpSocket) {
    if (canWrite) {
        QByteArray byteArray;
        quint32 byteArraySize;

        if (tcpSocket->size() <= emptyLenght) {
            byteArray = tcpSocket->readAll();
        } else {
            byteArray = tcpSocket->read(emptyLenght);
        }

        byteArraySize = byteArray.size();

        if (fileBuf != nullptr) {
            fileBuf->write(byteArray);
        } else {
            arrayBuf.append(byteArray);
        }

        byteArray.clear();
        emptyLenght -= byteArraySize;

        if (emptyLenght == 0) {
            canWrite = false;

            switch (compressionType) {
                case CompressionType::ZLIB: {
                    if (fileBuf != nullptr) {
                        fileBuf->seek(0);
                        QByteArray input = fileBuf->readAll();

                        fileBuf->resize(0);

                        fileBuf->write(qUncompress(input));
                    } else {
                        QByteArray input = arrayBuf;
                        arrayBuf.clear();

                        arrayBuf = qUncompress(input);
                    }

                    break;
                }
            }
        }

        return byteArraySize;
    }

    return 0;
}

QByteArray CatsMessageByteBuf::read() const {
    if (fileBuf != nullptr) {
        fileBuf->seek(0);
        return fileBuf->readAll();
    } else {
        return arrayBuf;
    }
}

CatsStreamByteBuf::CatsStreamByteBuf(CompressionType::Enum compressionType, QObject *parent) : CatsByteBuf(compressionType, parent) {}

void CatsStreamByteBuf::initial(const qint32 &size) {
    if (size > 0) {
        CatsMessageByteBuf *byteBuf = new CatsMessageByteBuf(compressionType, this);
        byteBuf->initial(size);

        emptyLenght += size;
        chunk++;

        chunks.append(byteBuf);
    } else {
        canWrite = false;
    }
}

quint32 CatsStreamByteBuf::write(QTcpSocket *tcpSocket) {
    if (canWrite && emptyLenght > 0) {
        CatsMessageByteBuf *buteByf = chunks.at(chunk);
        quint32 byteArraySize = buteByf->write(tcpSocket);

        emptyLenght -= byteArraySize;

        return byteArraySize;
    }

    return 0;
}

QByteArray CatsStreamByteBuf::read() const {
    QByteArray byteArray;

    foreach(CatsMessageByteBuf *byteBuf, chunks) {
        byteArray.append(byteBuf->read());
    }

    return byteArray;
}
