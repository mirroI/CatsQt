#ifndef CATS_MODELS_H
#define CATS_MODELS_H

#include <QFile>
#include <QList>
#include <QDebug>
#include <QObject>
#include <QFileInfo>
#include <QIODevice>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonValue>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QtConcurrent/QtConcurrentRun>

#include "mono_sink.h"

#include "enums.h"
#include "headers.h"

class StatementRequest {
 private:
	qint32 apiVersion;
	qint64 clientTime;
	QString schemeFormat = "JSON";
	QStringList compressors = QStringList() << "zlib";
	QString defaultCompression = "zlib";

 public:
	StatementRequest(qint32, qint64);
	QByteArray toJson() const;
};

class StatementResponse {
 private:
	qint64 serverTime;

 public:
	StatementResponse(const QByteArray&);
	qint64 getServerTime() const;
};

class CatsAbstractRequestModel {
 public:
	virtual QJsonDocument toJsonDocument() const = 0;
};

class CatsAbstractResponseModel {
 public:
	virtual void fromJsonDocument(QJsonDocument) = 0;
};

class CatsAbstractResponseObject : public QObject {
 public:
	CatsAbstractResponseObject(QObject * = nullptr);
	virtual void fromJsonDocument(QJsonDocument) = 0;
};

class CatsResponseModel : public CatsAbstractResponseModel {
 private:
	QJsonDocument jsonDocument;
 public:
	CatsResponseModel() {
	};
	void fromJsonDocument(QJsonDocument) override;

	QJsonObject getJson() const;
	QJsonArray getJsonArray() const;
};

//=======================================================================================================================================
//
//=======================================================================================================================================

class ErrorModel : public CatsAbstractResponseObject {
 Q_OBJECT
	Q_PROPERTY(QString message READ message CONSTANT)

 private:
	QJsonObject error;

	int code;
	QString _exception;
	QString _message;

 public:
	void fromJsonDocument(QJsonDocument) override;

	QJsonObject getError() const;
	int getCode() const;
	QString getException() const;
	QString getMessage() const;

	const QString& message() const;
};

//=======================================================================================================================================
//
//=======================================================================================================================================

class CatsData {
 protected:
	QByteArray data;

 protected:
	CatsData(const QByteArray&);

 private:
	QJsonDocument toJsonDocument(const QJsonValue&) const;

 public:
	QByteArray getData() const;

	QJsonObject jsonObject() const;

	template<class T = CatsAbstractResponseModel>
	T getData() const {
		T model;
		model.fromJsonDocument(QJsonDocument::fromJson(data));

		return model;
	}

	template<class T>
	T *object() {
		T *object = new T;
		object->fromJsonDocument(QJsonDocument::fromJson(data));

		return object;
	}

	template<class T, class L = QList<CatsAbstractResponseObject *>>
	L objectList(const QString& key = nullptr) {
		L list;
		QJsonArray array;

		if (key == nullptr) {
			array = QJsonDocument::fromJson(data).array();
		} else {
			array = QJsonDocument::fromJson(data).object().value(key).toArray();
		}

			foreach(QJsonValue jsonValue, array) {
				T *object = new T;
				object->fromJsonDocument(toJsonDocument(jsonValue));

				list.append(object);
			}

		return list;
	}
};

class CatsFileInfo : public CatsAbstractRequestModel {
 protected:
	QString key;
	QString name;
	qint64 size;
	QString type;
	QFile *file;

 public:
	CatsFileInfo(const QJsonObject&);
	CatsFileInfo(const QString&, QFile *);
	CatsFileInfo(const QString&, const QString&, QFile *);

	qint64 getSize() const; //remove
	QFile *getFile() const;
	void setFile(QFile *value); //remove, add client only json objecct to abstract request

	QJsonDocument toJsonDocument() const override;
};

class CatsFiles {
 protected:
	QList<CatsFileInfo *> files;

 protected:
	CatsFiles() {
	};
	CatsFiles(const QList<CatsFileInfo *>&);

 public:
	QList<CatsFileInfo *> getFiles() const;
};

//=======================================================================================================================================
//
//=======================================================================================================================================

class CatsAbstractMessage {
 protected:
	CatsAbstractHeader *header;

 public:
	CatsAbstractMessage(CatsAbstractHeader *);

	virtual CatsAbstractHeader *getHeader() const;
};

class CatsAbstractRequest : public CatsAbstractMessage {
 public:
	CatsAbstractRequest(CatsAbstractHeader *);

	virtual QByteArray toByteBuf() const = 0;
};

class CatsAbstractResponse : public CatsAbstractMessage {
 public:
	CatsAbstractResponse(CatsAbstractHeader *);
};

//=======================================================================================================================================
//
//=======================================================================================================================================

class CatsMessage {
 protected:
	QJsonObject *messageHeader = new QJsonObject();

 public:
	CatsMessage(QJsonObject&);

	QJsonObject getMessageHeader() const;
};

class CatsRequest : public CatsAbstractRequest, public CatsMessage {
 protected:
	QJsonObject metaData;
	quint64 timeOut;

	MonoSink<CatsAbstractResponse> *_monoSink = nullptr;

	std::function<void(const quint32&, const quint32&)> writeProgressHandler;
	std::function<void(const quint32&, const quint32&)> readProgressHandler;

 protected:
	CatsRequest(CatsHeader *, QJsonObject&, const QJsonObject&, const quint64&,
		const std::function<void(const quint32&, const quint32&)>,
		const std::function<void(const quint32&, const quint32&)>);

 public:
	CatsHeader *getHeader() const override;

	quint64 getTimeOut() const;

	MonoSink<CatsAbstractResponse> *getMonoSink() const;
	void setMonoSink(MonoSink<CatsAbstractResponse> *monoSink);

	void emitWriteProgressHandler(const quint32&, const quint32&);
	void emitReadProgressHandler(const quint32&, const quint32&);

 protected:
	template<class M = CatsRequest>
	class Builder {
	 protected:
		QJsonObject pMessageHeader;
		QJsonObject pMetaData;
		quint64 pTimeOut = 0;
		std::function<void(const quint32&, const quint32&)> pWriteProgressHandler = nullptr;
		std::function<void(const quint32&, const quint32&)> pReadProgressHandler = nullptr;

	 public:
		M *messageHeader(const QJsonObject& value) {
			pMessageHeader = value;
			return getMember();
		}

		M *messageHeader(const QString& key, const QJsonValue& value) {
			pMessageHeader.insert(key, value);
			return getMember();
		}

		M *metaData(const QJsonObject& value) {
			pMetaData = value;
			return getMember();
		}

		M *metaData(const QString& key, const QJsonValue& value) {
			pMetaData.insert(key, value);
			return getMember();
		}

		M *timeOut(const quint64& value) {
			this->pTimeOut = value;
			return getMember();
		}

		M *writeProgressHandler(const std::function<void(const quint32&, const quint32&)> value) {
			this->pWriteProgressHandler = value;
			return getMember();
		}

		M *readProgressHandler(const std::function<void(const quint32&, const quint32&)> value) {
			this->pReadProgressHandler = value;
			return getMember();
		};

	 private:
		virtual M *getMember() = 0;
	};
};

class CatsResponse : public CatsAbstractResponse, public CatsMessage {
 public:
	CatsResponse(CatsHeader *, QJsonObject&);

	CatsHeader *getHeader() const override;
};

class CatsBasicRequest : public CatsData, public CatsRequest {
 private:
	explicit CatsBasicRequest(CatsHeader *,
		QJsonObject&,
		const QByteArray&,
		const QJsonObject&,
		const quint64&,
		const std::function<void(const quint32&, const quint32&)>,
		const std::function<void(const quint32&, const quint32&)>);

 public:
	CatsInputHeader *getHeader() const override;

	QByteArray toByteBuf() const override;

 private:
	class Builder : public CatsRequest::Builder<CatsBasicRequest::Builder> {
	 public:
		CatsBasicRequest *build(const quint16&, const QByteArray& data = QByteArray());
		CatsBasicRequest *build(const quint16&, const CatsAbstractRequestModel&);
		CatsBasicRequest *buildInput(CatsRequest *, const QByteArray& data = QByteArray());
		CatsBasicRequest *buildInput(CatsRequest *, const CatsAbstractRequestModel&);

	 private:
		CatsBasicRequest::Builder *getMember() override;

		CatsBasicRequest *build(CatsHeader *header, const QByteArray& data);
	};

 public:
	static Builder *builder();
};

class CatsBasicResponse : public CatsData, public CatsResponse {
 public:
	CatsBasicResponse(CatsHeader *, QJsonObject&, const QByteArray&);
};

class CatsFilesRequest : public CatsFiles, public CatsRequest {
 private:
	quint32 writeFileId = 0;

 private:
	explicit CatsFilesRequest(CatsHeader *,
		QJsonObject&,
		const QList<CatsFileInfo *>&,
		const QJsonObject&,
		const quint64&,
		const std::function<void(const quint32&, const quint32&)>,
		const std::function<void(const quint32&, const quint32&)>);

 public:
	quint32 getWriteFileId() const;
	void setWriteFileId(const quint32&);

	QByteArray toByteBuf() const;

 private:
	class Builder : public CatsRequest::Builder<CatsFilesRequest::Builder> {
	 public:
		CatsFilesRequest *build(const quint16&, const QList<CatsFileInfo *>&);
		CatsFilesRequest *buildInput(CatsRequest *, const QList<CatsFileInfo *>&);

	 private:
		CatsFilesRequest::Builder *getMember() override;

		CatsFilesRequest *build(CatsHeader *header, const QList<CatsFileInfo *>&);
	};

 public:
	static CatsFilesRequest::Builder *builder();
};

class CatsFilesResponse : public CatsFiles, public CatsResponse {
 private:
	quint32 readFileId = 0;

 public:
	CatsFilesResponse(CatsHeader *, QJsonObject&);

	quint32 getReadFileId() const;
	void setReadFileId(const quint32& value);
};

class CatsSystemRequest : public CatsAbstractRequest {
 public:
	CatsSystemRequest(CatsAbstractHeader *);

	QByteArray toByteBuf() const override;
};

#endif // CATS_MODELS_H
