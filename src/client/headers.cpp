#include "headers.h"

HeaderType::Enum CatsAbstractHeader::getHeaderType() const {
    return headerType;
}

CatsHeader::CatsHeader(DataType::Enum dataType, CompressionType::Enum compressionType) {
    this->dataType = dataType;
    this->compressionType = compressionType;
}

quint16 CatsHeader::getMessageId() const {
    return messageId;
}

void CatsHeader::setMessageId(quint16 value) {
    messageId = value;
}

DataType::Enum CatsHeader::getDataType() const {
    return dataType;
}

CompressionType::Enum CatsHeader::getCompressionType() const {
    return compressionType;
}

CatsInputHeader::CatsInputHeader(DataType::Enum dataType, CompressionType::Enum compressionType) : CatsHeader(dataType, compressionType) {}

CatsInputHeader::CatsInputHeader(CatsHeader *parentHeader, DataType::Enum dataType, CompressionType::Enum compressionType) : CatsHeader(dataType, compressionType) {
    headerType = HeaderType::CHILDREN;

    if (parentHeader != nullptr) {
        this->messageId = parentHeader->getMessageId();
    }
}

CatsInputHeader::CatsInputHeader(QDataStream &in) {
    quint8 dataType, compressionType;
    headerType = HeaderType::CHILDREN;

    in >> messageId >> dataType >> compressionType >> dataLenght;

    this->dataType = static_cast<DataType::Enum>(dataType);
    this->compressionType = static_cast<CompressionType::Enum>(compressionType);
}

quint32 CatsInputHeader::getDataLenght() const {
    return dataLenght;
}

void CatsInputHeader::setDataLenght(const quint32 &value) {
    dataLenght = value;
}

QByteArray CatsInputHeader::toByteBuf() {
    QByteArray byteBuf;
    QDataStream out(&byteBuf, QDataStream::WriteOnly);

    out << (quint8) headerType << messageId << (quint8) dataType << (quint8) compressionType << dataLenght;

    return byteBuf;
}

CatsBasicHeader::CatsBasicHeader(quint16 handlerId, DataType::Enum dataType, CompressionType::Enum compressionType) : CatsInputHeader(dataType, compressionType) {
    headerType = HeaderType::BASIC;
    this->handlerId = handlerId;
}

CatsBasicHeader::CatsBasicHeader(QDataStream &in) {
    quint8 dataType, compressionType;
    headerType = HeaderType::BASIC;

    in >> handlerId >> messageId >> time >> dataType >> compressionType >> dataLenght;

    this->dataType = static_cast<DataType::Enum>(dataType);
    this->compressionType = static_cast<CompressionType::Enum>(compressionType);
}

quint16 CatsBasicHeader::getHandlerId() const {
    return handlerId;
}

quint64 CatsBasicHeader::getTime() const {
    return time;
}

void CatsBasicHeader::setTime(const quint64 &value) {
    time = value;
}

QByteArray CatsBasicHeader::toByteBuf() {
    QByteArray byteBuf;
    QDataStream out(&byteBuf, QDataStream::WriteOnly);

    out << (quint8) headerType << handlerId << messageId << time << (quint8) dataType << (quint8) compressionType << dataLenght;

    return byteBuf;
}

CatsStreamingHeader::CatsStreamingHeader(quint16 handlerId, DataType::Enum dataType, CompressionType::Enum compressionType) : CatsHeader(dataType, compressionType) {
    headerType = HeaderType::STREAM;
    this->handlerId = handlerId;
}

CatsStreamingHeader::CatsStreamingHeader(QDataStream &in) {
    quint8 dataType, compressionType;
    headerType = HeaderType::STREAM;

    in >> handlerId >> messageId >> time >> dataType >> compressionType;

    this->dataType = static_cast<DataType::Enum>(dataType);
    this->compressionType = static_cast<CompressionType::Enum>(compressionType);
}

quint16 CatsStreamingHeader::getHandlerId() const {
    return handlerId;
}

quint64 CatsStreamingHeader::getTime() const {
    return time;
}

void CatsStreamingHeader::setTime(const quint64 &value) {
    time = value;
}

QByteArray CatsStreamingHeader::toByteBuf() {
    QByteArray byteBuf;
    QDataStream out(&byteBuf, QDataStream::WriteOnly);

    out << (quint8) headerType << handlerId << messageId << time << (quint8) getDataType() << (quint8) getCompressionType();

    return byteBuf;
}
CatsSpeedLimiterHeader::CatsSpeedLimiterHeader(quint32 bytes) {
    headerType = HeaderType::SPEED_LIMIT;
    this->bytes = bytes;
}

CatsSpeedLimiterHeader::CatsSpeedLimiterHeader(QDataStream &in) {
    in >> bytes;
}

QByteArray CatsSpeedLimiterHeader::toByteBuf() {
    QByteArray byteBuf;
    QDataStream out(&byteBuf, QDataStream::WriteOnly);

    out << (quint8) headerType << bytes;

    return byteBuf;
}

CatsImputCancellingHeader::CatsImputCancellingHeader(quint16 handlerId) {
    headerType = HeaderType::CANCEL_INPUT;
    this->handlerId = handlerId;
}

CatsImputCancellingHeader::CatsImputCancellingHeader(QDataStream &in) {
    in >> handlerId;
}

QByteArray CatsImputCancellingHeader::toByteBuf() {
    QByteArray byteBuf;
    QDataStream out(&byteBuf, QDataStream::WriteOnly);

    out << (quint8) headerType << handlerId;

    return byteBuf;
}

CatsPingPongHeader::CatsPingPongHeader() {
    headerType = HeaderType::PING_PONG;
}

CatsPingPongHeader::CatsPingPongHeader(QDataStream &in) {
    in >> time;
}

quint64 CatsPingPongHeader::getTime() const {
    return time;
}

void CatsPingPongHeader::setTime(quint64 newTime) {
    time = newTime;
}

QByteArray CatsPingPongHeader::toByteBuf() {
    QByteArray byteBuf;
    QDataStream out(&byteBuf, QDataStream::WriteOnly);

    out << (quint8) headerType << time;

    return byteBuf;
}
