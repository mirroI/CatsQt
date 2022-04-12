//
// Created by Алексей Сиротин on 10.01.2022.
//

#include "client/client.h"
#include "mono.h"
#include <QThreadPool>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
	QCoreApplication *a = new QCoreApplication(argc, argv);

	CatsClient *client = new CatsClient{
		0,
		"t0ps3cr3t",
		"192.168.31.13",
		9095
	};

	qDebug() << "MAIN THREAD" << QThread::currentThreadId();

	CatsRequest *request1 = CatsBasicRequest::builder()
		->build(0x0001, "hello world 1");
	CatsBasicResponse *response = client->sendMessage(request1)
		->map<CatsBasicResponse>([=](CatsAbstractResponse *response) {
		  return static_cast<CatsBasicResponse *>(response);
		})
		->block();

	qDebug() << "PIDOR 1" << response->getData();

	CatsRequest *request = CatsBasicRequest::builder()
		->build(0x0001, "hello world");
	client->sendMessage(request)
		->map<CatsBasicResponse>([=](CatsAbstractResponse *response) {
		  return static_cast<CatsBasicResponse *>(response);
		})
		->subscribe([=](CatsBasicResponse *response) {
		  qDebug() << "PIDOR" << response->getData();
		  qDebug() << QThread::currentThreadId();
		});

	return a->exec();
}
