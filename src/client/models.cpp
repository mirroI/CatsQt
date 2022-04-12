#include "models.h"
#include "mono_sink.h"

StatementRequest::StatementRequest(qint32 apiVersion, qint64 clientTime) {
	this->apiVersion = apiVersion;
	this->clientTime = clientTime;
}

QByteArray StatementRequest::toJson() const {
	QJsonArray compressors;
		foreach(QString compressor, this->compressors) {
			compressors.append(QJsonValue(compressor));
		}

	QJsonObject json{
		{ "api", apiVersion },
		{ "client_time", clientTime },
		{ "scheme_format", schemeFormat },
		{ "compressors", compressors },
		{ "default_compression", defaultCompression }
	};

	return QJsonDocument(json).toJson(QJsonDocument::JsonFormat::Compact);
}

StatementResponse::StatementResponse(const QByteArray& stringJson) {
	QJsonObject json = QJsonDocument::fromJson(stringJson).object();

	serverTime = json.value("server_time").toVariant().toLongLong();
}

CatsAbstractResponseObject::CatsAbstractResponseObject(QObject *parent) : QObject(parent) {
}

qint64 StatementResponse::getServerTime() const {
	return serverTime;
}

void CatsResponseModel::fromJsonDocument(QJsonDocument jsonDocument) {
	this->jsonDocument = jsonDocument;
}

QJsonObject CatsResponseModel::getJson() const {
	return jsonDocument.object();
}

QJsonArray CatsResponseModel::getJsonArray() const {
	return jsonDocument.array();
}

void ErrorModel::fromJsonDocument(QJsonDocument jsonDocument) {
	QJsonObject json = jsonDocument.object();

	error = json.value("error").toObject();

	code = error.value("code").toInt();
	_exception = error.value("exception").toString();
	_message = error.value("message").toString();
}

QJsonObject ErrorModel::getError() const {
	return error;
}

int ErrorModel::getCode() const {
	return code;
}

QString ErrorModel::getException() const {
	return _exception;
}

QString ErrorModel::getMessage() const {
	return _message;
}

const QString& ErrorModel::message() const {
	return _message;
}

CatsData::CatsData(const QByteArray& data) {
	this->data = data;
}

QJsonDocument CatsData::toJsonDocument(const QJsonValue& jsonValue) const {
	if (jsonValue.isArray()) {
		return QJsonDocument(jsonValue.toArray());
	} else if (jsonValue.isObject()) {
		return QJsonDocument(jsonValue.toObject());
	} else {
		//ToDo <mirrol>
	}
}

QByteArray CatsData::getData() const {
	return data;
}

QJsonObject CatsData::jsonObject() const {
	return QJsonDocument::fromJson(data).object();
}

CatsFileInfo::CatsFileInfo(const QJsonObject& jsonObject) {
	key = jsonObject.value("key").toString();
	name = jsonObject.value("name").toString();
	size = jsonObject.value("size").toInt();
	type = jsonObject.value("type").toString();
}

CatsFileInfo::CatsFileInfo(const QString& key, QFile *file) : CatsFileInfo(key,
	QFileInfo(*file).completeBaseName(),
	file) {
}

CatsFileInfo::CatsFileInfo(const QString& key, const QString& name, QFile *file) {
	this->key = key;
	this->name = name;
	this->size = file->size();
	this->type = QFileInfo(*file).suffix();
	this->file = file;
}

qint64 CatsFileInfo::getSize() const {
	return size;
}

QFile *CatsFileInfo::getFile() const {
	return file;
}

QJsonDocument CatsFileInfo::toJsonDocument() const {
	QJsonObject jsonObject;
	jsonObject.insert("key", key);
	jsonObject.insert("name", name);
	jsonObject.insert("size", size);
	jsonObject.insert("type", type);

	return QJsonDocument(jsonObject);
}

CatsFiles::CatsFiles(const QList<CatsFileInfo *>& files) {
		foreach(CatsFileInfo *fileInfo, files) {
			fileInfo->getFile()->open(QIODevice::ReadOnly);
		}

	this->files = files;
}

QList<CatsFileInfo *> CatsFiles::getFiles() const {
	return files;
}

CatsAbstractMessage::CatsAbstractMessage(CatsAbstractHeader *header) {
	this->header = header;
}

CatsAbstractHeader *CatsAbstractMessage::getHeader() const {
	return header;
}

CatsAbstractRequest::CatsAbstractRequest(CatsAbstractHeader *header) : CatsAbstractMessage(header) {
}

CatsAbstractResponse::CatsAbstractResponse(CatsAbstractHeader *header) : CatsAbstractMessage(header) {
}

CatsMessage::CatsMessage(QJsonObject& messageHeader) {
	this->messageHeader = &messageHeader;
}

QJsonObject CatsMessage::getMessageHeader() const {
	return *messageHeader;
}

CatsRequest::CatsRequest(CatsHeader *header, QJsonObject& messageHeader, const QJsonObject& metaData,
	const quint64& timeOut, const std::function<void(const quint32&, const quint32&)> writeProgressHandler,
	const std::function<void(const quint32&, const quint32&)> readProgressHandler)
	: CatsAbstractRequest(header), CatsMessage(messageHeader) {
	this->metaData = metaData;
	this->timeOut = timeOut;
	this->writeProgressHandler = writeProgressHandler;
	this->readProgressHandler = readProgressHandler;
}

CatsHeader *CatsRequest::getHeader() const {
	return static_cast<CatsHeader *>(header);
}

quint64 CatsRequest::getTimeOut() const {
	return timeOut;
}

MonoSink<CatsAbstractResponse> *CatsRequest::getMonoSink() const {
	return _monoSink;
}

void CatsRequest::setMonoSink(MonoSink<CatsAbstractResponse> *monoSink) {
	_monoSink = monoSink;
}

void CatsRequest::emitWriteProgressHandler(const quint32& writeBytes, const quint32& dataLenght) { //ToDO
//    if (writeProgessHandler != nullptr) {
//        if (asyncHandler) {
//            QtConcurrent::run(writeProgressHandler, writeBytes, dataLenght);
//        } else {
//			writeProgressHandler(writeBytes, dataLenght);
//        }
//    }
}

void CatsRequest::emitReadProgressHandler(const quint32& readBytes, const quint32& dataLenght) { //ToDo
//    if (readProgressHandler != nullptr) {
//        if (asyncHandler) {
//            QtConcurrent::run(readProgressHandler, readBytes, dataLenght);
//        } else {
//            readProgressHandler(readBytes, dataLenght);
//        }
//    }
}

CatsResponse::CatsResponse(CatsHeader *header, QJsonObject& messageHeader)
	: CatsAbstractResponse(header), CatsMessage(messageHeader) {
}

CatsHeader *CatsResponse::getHeader() const {
	return static_cast<CatsHeader *>(header);
}

CatsBasicRequest::CatsBasicRequest(CatsHeader *header,
	QJsonObject& messageHeader,
	const QByteArray& data,
	const QJsonObject& metaData,
	const quint64& timeOut,
	const std::function<void(const quint32&, const quint32&)> writeProgressHandler,
	const std::function<void(const quint32&, const quint32&)> readProgressHandler) : CatsData(data),
																					 CatsRequest(header,
																						 messageHeader,
																						 metaData,
																						 timeOut,
																						 writeProgressHandler,
																						 readProgressHandler) {
	if (header->getDataType() == DataType::FILES) {
		throw std::invalid_argument("Basic request cannot send files");
	}
}

CatsInputHeader *CatsBasicRequest::getHeader() const {
	return static_cast<CatsInputHeader *>(this->header);
}

QByteArray CatsBasicRequest::toByteBuf() const {
	CatsInputHeader *header = dynamic_cast<CatsInputHeader *>(this->header);

	QByteArray messageHeaderJson = QJsonDocument(getMessageHeader()).toJson(QJsonDocument::JsonFormat::Compact);
	header->setDataLenght(messageHeaderJson.size() + 2 + data.size());

	QByteArray byteBuf = header->toByteBuf();
	byteBuf.append(messageHeaderJson);
	byteBuf.append(2, (quint8) 0);
	byteBuf.append(data);

	return byteBuf;
}

CatsBasicRequest *CatsBasicRequest::Builder::build(const quint16& handlerId, const QByteArray& data) {
	CatsBasicHeader *header = new CatsBasicHeader(handlerId, DataType::BYTES, CompressionType::NONE);

	return build(header, data);
}

CatsBasicRequest *CatsBasicRequest::Builder::build(const quint16& handlerId, const CatsAbstractRequestModel& model) {
	CatsBasicHeader *header = new CatsBasicHeader(handlerId, DataType::JSON, CompressionType::NONE);

	return build(header, model.toJsonDocument().toJson(QJsonDocument::JsonFormat::Compact));
}

CatsBasicRequest *CatsBasicRequest::Builder::buildInput(CatsRequest *parentRequest, const QByteArray& data) {
	CatsInputHeader *header = new CatsInputHeader(parentRequest->getHeader(), DataType::JSON, CompressionType::NONE);
	return build(header, data);
}

CatsBasicRequest *CatsBasicRequest::Builder::buildInput(CatsRequest *parentRequest,
	const CatsAbstractRequestModel& model) {
	CatsInputHeader *header = new CatsInputHeader(parentRequest->getHeader(), DataType::JSON, CompressionType::NONE);
	return build(header, model.toJsonDocument().toJson(QJsonDocument::JsonFormat::Compact));
}

CatsBasicRequest::Builder *CatsBasicRequest::Builder::getMember() {
	return this;
}

CatsBasicRequest *CatsBasicRequest::Builder::build(CatsHeader *header, const QByteArray& data) {
	return new CatsBasicRequest{
		header,
		pMessageHeader,
		data,
		pMetaData,
		pTimeOut,
		pWriteProgressHandler,
		pReadProgressHandler };
}

CatsBasicRequest::Builder *CatsBasicRequest::builder() {
	return new CatsBasicRequest::Builder();
}

CatsBasicResponse::CatsBasicResponse(CatsHeader *header, QJsonObject& messageHeader, const QByteArray& data) : CatsData(
	data),
																											   CatsResponse(
																												   header,
																												   messageHeader) {
}

CatsFilesRequest::CatsFilesRequest(CatsHeader *header,
	QJsonObject& messageHeader,
	const QList<CatsFileInfo *>& files,
	const QJsonObject& metaData,
	const quint64& timeOut,
	const std::function<void(const quint32&, const quint32&)> writeProgressHandler,
	const std::function<void(const quint32&, const quint32&)> readProgressHandler) : CatsFiles(files),
																					 CatsRequest(header,
																						 messageHeader,
																						 metaData,
																						 timeOut,
																						 writeProgressHandler,
																						 readProgressHandler) {
	if (header->getDataType() != DataType::FILES) {
		throw std::invalid_argument("Files request can only send files");
	}
}

quint32 CatsFilesRequest::getWriteFileId() const {
	return writeFileId;
}

void CatsFilesRequest::setWriteFileId(const quint32& value) {
	writeFileId = value;
}

QByteArray CatsFilesRequest::toByteBuf() const {
	CatsInputHeader *header = dynamic_cast<CatsInputHeader *>(this->header);
	header->setDataLenght(0);

	QJsonArray jsonArray;
		foreach(CatsFileInfo *fileInfo, files) {
			header->setDataLenght(header->getDataLenght() + fileInfo->getSize());
			jsonArray.append(fileInfo->toJsonDocument().object());
		}

	messageHeader->insert("Files", QJsonValue(jsonArray));
	qDebug() << getMessageHeader();

	QByteArray messageHeaderJson = QJsonDocument(getMessageHeader()).toJson(QJsonDocument::JsonFormat::Compact);
	header->setDataLenght(header->getDataLenght() + messageHeaderJson.size() + 2);

	QByteArray byteBuf = header->toByteBuf();
	byteBuf.append(messageHeaderJson);
	byteBuf.append(2, (quint8) 0);

	return byteBuf;
}

CatsFilesRequest *CatsFilesRequest::Builder::build(const quint16& handlerId, const QList<CatsFileInfo *>& files) {
	CatsBasicHeader *header = new CatsBasicHeader(handlerId, DataType::FILES, CompressionType::NONE);
	return build(header, files);
}

CatsFilesRequest *CatsFilesRequest::Builder::buildInput(CatsRequest *parentRequest,
	const QList<CatsFileInfo *>& files) {
	CatsInputHeader *header = new CatsInputHeader(parentRequest->getHeader(), DataType::FILES, CompressionType::NONE);
	return build(header, files);
}

CatsFilesRequest::Builder *CatsFilesRequest::Builder::getMember() {
	return this;
}

CatsFilesRequest *CatsFilesRequest::Builder::build(CatsHeader *header, const QList<CatsFileInfo *>& files) {
	return new CatsFilesRequest{
		header,
		pMessageHeader,
		files,
		pMetaData,
		pTimeOut,
		pWriteProgressHandler,
		pReadProgressHandler };
}

CatsFilesRequest::Builder *CatsFilesRequest::builder() {
	return new CatsFilesRequest::Builder();
}

CatsFilesResponse::CatsFilesResponse(CatsHeader *header, QJsonObject& messageHeader) : CatsResponse(header,
	messageHeader) {
	if (!this->messageHeader->contains("Files")) {
		throw std::invalid_argument("");
	}

		foreach(QJsonValue jsonValue, this->messageHeader->value("Files").toArray()) {
			files.append(new CatsFileInfo(jsonValue.toObject()));
		}
}

quint32 CatsFilesResponse::getReadFileId() const {
	return readFileId;
}

void CatsFilesResponse::setReadFileId(const quint32& value) {
	readFileId = value;
}

CatsSystemRequest::CatsSystemRequest(CatsAbstractHeader *header) : CatsAbstractRequest(header) {
}

QByteArray CatsSystemRequest::toByteBuf() const {
	return header->toByteBuf();
}
