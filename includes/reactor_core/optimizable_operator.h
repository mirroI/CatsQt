//
// Created by Алексей Сиротин on 19.02.2022.
//

#ifndef REACTORCORE_SRC_OPTIMIZABLE_OPERATOR_H
#define REACTORCORE_SRC_OPTIMIZABLE_OPERATOR_H

#include <QObject>

#include "publisher.h"

template<typename IN, typename OUT>
class  OptimizableOperator : public CorePublisher<IN> {
 public:
	virtual CoreSubscriber<OUT> *subscribeOrReturn(CoreSubscriber<IN> *actual) = 0;
	virtual CorePublisher<OUT> *source() = 0;
	virtual OptimizableOperator<QObject, OUT> *nextOptimizableSource() = 0;
};

template<typename T>
class CorePublisherAdapter : public CorePublisher<T>, public OptimizableOperator<T, T> {
 private:
	const Publisher<T> *_publisher;
	const OptimizableOperator<QObject, T> *_optimizableOperator = nullptr;

 public:
	explicit CorePublisherAdapter(Publisher<T> *publisher);

	void subscribe(Subscriber<T> *subscriber) override;
	void subscribe(CoreSubscriber<T> *subscriber) override;

	CoreSubscriber<T> *subscribeOrReturn(CoreSubscriber<T> *actual) override;
	CorePublisher<T> *source() override;
	OptimizableOperator<QObject, T> *nextOptimizableSource() override;
};

template<typename T>
CorePublisherAdapter<T>::CorePublisherAdapter(Publisher<T> *publisher)
	: _publisher(publisher) {
	if (OptimizableOperator<QObject, T> *optimizableOperator = static_cast<OptimizableOperator<QObject, T> *>(publisher)) {
		_optimizableOperator = optimizableOperator;
	}
}

template<typename T>
void CorePublisherAdapter<T>::subscribe(Subscriber<T> *subscriber) {
	_publisher->subscribe(subscriber);
}

template<typename T>
void CorePublisherAdapter<T>::subscribe(CoreSubscriber<T> *subscriber) {
	_publisher->subscribe(subscriber);
}

template<typename T>
CoreSubscriber<T> *CorePublisherAdapter<T>::subscribeOrReturn(CoreSubscriber<T> *actual) {
	return actual;
}

template<typename T>
CorePublisher<T> *CorePublisherAdapter<T>::source() {
	return this;
}

template<typename T>
OptimizableOperator<QObject, T> *CorePublisherAdapter<T>::nextOptimizableSource() {
	return _optimizableOperator;
}

#endif //REACTORCORE_SRC_OPTIMIZABLE_OPERATOR_H
