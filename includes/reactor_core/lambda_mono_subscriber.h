//
// Created by Алексей Сиротин on 29.01.2022.
//

#ifndef REACTORCORE_SRC_LAMBDA_MONO_SUBSCRIBER_H
#define REACTORCORE_SRC_LAMBDA_MONO_SUBSCRIBER_H

#include <functional>
#include "subscriber.h"

template<typename T>
class LambdaMonoSubscriber : public CoreSubscriber<T> {
 private:
	std::function<void(T *)> _consumer;
	std::function<void(const std::exception&)> _errorConsumer;

 public:
	explicit LambdaMonoSubscriber(const std::function<void(T *)>& consumer,
		const std::function<void(const std::exception&)>& errorConsumer);

	void onSubscribe(Subscription *subscription) override;
	void onNext(T *value) override;
	void onError(const std::exception& exception) override;
	void onComplete() override;

 private:
	void doError(const std::exception &exception);
};

template<typename T>
LambdaMonoSubscriber<T>::LambdaMonoSubscriber(const std::function<void(T *)>& consumer,
	const std::function<void(const std::exception&)>& errorConsumer)
	:_consumer(consumer), _errorConsumer(errorConsumer) {
}

template<typename T>
void LambdaMonoSubscriber<T>::onSubscribe(Subscription *subscription) {

}

template<typename T>
void LambdaMonoSubscriber<T>::onNext(T *value) {
	if (_consumer != nullptr) {
		try {
			_consumer(value);
		} catch (std::exception &exception) {
			doError(exception);
		}
	}
}

template<typename T>
void LambdaMonoSubscriber<T>::onError(const std::exception& exception) {
	doError(exception);
}

template<typename T>
void LambdaMonoSubscriber<T>::onComplete() {

}

template<typename T>
void LambdaMonoSubscriber<T>::doError(const std::exception& exception) {
	if (_errorConsumer != nullptr) {
		_errorConsumer(exception);
	}
}

#endif //REACTORCORE_SRC_LAMBDA_MONO_SUBSCRIBER_H
