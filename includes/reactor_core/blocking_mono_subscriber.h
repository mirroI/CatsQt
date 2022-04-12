//
// Created by Алексей Сиротин on 12.04.2022.
//

#ifndef REACTORCORE_SRC_BLOCKING_MONO_SUBSCRIBER_H
#define REACTORCORE_SRC_BLOCKING_MONO_SUBSCRIBER_H

#include <QThread>
#include "subscriber.h"

template<typename T>
class BlockingMonoSubscriber : public CoreSubscriber<T> {
 private:
	T *_value;
	std::exception _exception;
	Subscription *_subscription;

 public:
	void onSubscribe(Subscription *subscription) override;
	void onNext(T *value) override;
	void onError(const std::exception& exception) override;
	void onComplete() override;

	T *blockingGet();
};

template<typename T>
void BlockingMonoSubscriber<T>::onSubscribe(Subscription *subscription) {
	_subscription = subscription;
}

template<typename T>
void BlockingMonoSubscriber<T>::onNext(T *value) {
	if (_value == nullptr) {
		_value = value;
	}
}

template<typename T>
void BlockingMonoSubscriber<T>::onError(const std::exception& exception) {
	if (_value == nullptr) {
		_exception = exception;
	}
}

template<typename T>
void BlockingMonoSubscriber<T>::onComplete() {

}

template<typename T>
T *BlockingMonoSubscriber<T>::blockingGet() {
	while (_value == nullptr) {
		QThread::msleep(50);
	}

	return _value;
}

#endif //REACTORCORE_SRC_BLOCKING_MONO_SUBSCRIBER_H
