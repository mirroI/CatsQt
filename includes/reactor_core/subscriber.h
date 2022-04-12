//
// Created by Алексей Сиротин on 27.01.2022.
//

#ifndef REACTORCORE_SRC_SUBSCRIBER_H
#define REACTORCORE_SRC_SUBSCRIBER_H

#include <exception>

class Subscription {
 public:
//	virtual void request(long n) = 0;
//	virtual void cancel() = 0;
};

template<typename T>
class Subscriber {
 public:
	virtual void onSubscribe(Subscription *subscription) = 0;
	virtual void onNext(T *value) = 0;
	virtual void onError(const std::exception &exception) = 0;
	virtual void onComplete() = 0;
};

template<typename T>
class CoreSubscriber : public Subscriber<T> {

};

#endif //REACTORCORE_SRC_SUBSCRIBER_H
