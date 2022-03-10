//
// Created by Алексей Сиротин on 10.01.2022.
//

#include "client/client.h"
//#include <QThreadPool>
#include <QCoreApplication>

int main(int argc, char *argv[]) {

//

//




//	QCoreApplication *a = new QCoreApplication(argc, argv);


	CatsClient *client = new CatsClient{
		0,
		"t0ps3cr3t",
		"192.168.0.109",
		9095
	};



//	qDebug() << QThread::currentThreadId();
//	CatsAbstractResponse *response = nullptr;
//
//	CatsRequest *request = CatsBasicRequest::builder()
//		->responseHandler([&response](CatsAbstractResponse *abstractResponse) {
//		  	response = abstractResponse;
//		})
//		->build(0x0001, "hello world");
//	client->sendMessage(request);
//
//	while (response == nullptr) {
//		qDebug() << "TEST";
//		QThread::sleep(1);
//	}
//
//	qDebug() << QThread::currentThreadId();\
//
//	qDebug() << static_cast<CatsBasicResponse *>(response)->getData();

	return 0;
}
