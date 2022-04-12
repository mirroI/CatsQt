//
// Created by Алексей Сиротин on 20.02.2022.
//

#include <QThreadPool>

#include "mono.h"
#include "mono_operator.h"

#ifndef REACTORCORE_SRC_MONO_SUBSCRIBE_ON_H
#define REACTORCORE_SRC_MONO_SUBSCRIBE_ON_H

template<typename T>
class MonoSubscribeOn : public InternalMonoOperator<T, T> {
 public:
	MonoSubscribeOn(Mono<T> *source);

	CoreSubscriber<T> *subscribeOrReturn(CoreSubscriber<T> *actual) override;
};

template<typename T>
class SubscribeOnSubscriber : public CoreSubscriber<T>, public Subscription, public QRunnable {
 private:
	Publisher<T> *_publisher;
	CoreSubscriber<T> *_subscriber;

 public:
	explicit SubscribeOnSubscriber(Publisher<T> *publisher, CoreSubscriber<T> *subscriber);

	void run() override;

	void onSubscribe(Subscription *subscription) override;
	void onNext(T *value) override;
	void onError(const std::exception& exception) override;
	void onComplete() override;
};

template<typename T>
MonoSubscribeOn<T>::MonoSubscribeOn(Mono<T> *source):InternalMonoOperator<T, T>(source) {

}

template<typename T>
CoreSubscriber<T> *MonoSubscribeOn<T>::subscribeOrReturn(CoreSubscriber<T> *actual) {
	SubscribeOnSubscriber<T> *parent = new SubscribeOnSubscriber<T>{
		MonoOperator<T, T>::_source,
		actual
	};

	actual->onSubscribe(parent);

	QThreadPool::globalInstance()->start(parent);

	return nullptr;
}

template<typename T>
SubscribeOnSubscriber<T>::SubscribeOnSubscriber(Publisher<T> *publisher,
	CoreSubscriber<T> *subscriber) : _subscriber(subscriber), _publisher(publisher) {

}

template<typename T>
void SubscribeOnSubscriber<T>::run() {
	_publisher->subscribe(this);
}

template<typename T>
void SubscribeOnSubscriber<T>::onSubscribe(Subscription *subscription) {

}

template<typename T>
void SubscribeOnSubscriber<T>::onNext(T *value) {
	_subscriber->onNext(value);
}

template<typename T>
void SubscribeOnSubscriber<T>::onError(const std::exception& exception) {
	_subscriber->onError(exception);
}

template<typename T>
void SubscribeOnSubscriber<T>::onComplete() {
	_subscriber->onComplete();
}

#endif //REACTORCORE_SRC_MONO_SUBSCRIBE_ON_H
