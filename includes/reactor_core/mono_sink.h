//
// Created by Алексей Сиротин on 20.01.2022.
//

#ifndef REACTORCORE_SRC_MONO_SINK_H
#define REACTORCORE_SRC_MONO_SINK_H

#include <QObject>

#include "subscriber.h"

template<typename T>
class MonoSink {
 public:
	virtual void success() = 0;
	virtual void success(T *value) = 0;
	virtual void error(const std::exception& exception) = 0;
};

template<typename T>
class DefaultMonoSink : public MonoSink<T>, public Subscription {
 private:
	Subscriber<T> *_actual;

 public:
	explicit DefaultMonoSink(Subscriber<T> *actual);

	void success() override;
	void success(T *value) override;
	void error(const std::exception& exception) override;
};

template<typename T>
DefaultMonoSink<T>::DefaultMonoSink(Subscriber<T> *actual) : _actual(actual) {
}

template<typename T>
void DefaultMonoSink<T>::success() {
	_actual->onComplete();
}

template<typename T>
void DefaultMonoSink<T>::success(T *value) {
	if (value == nullptr) {
		return success();
	}

	try {
		_actual->onNext(value);
		_actual->onComplete();
	}
	catch (std::exception exception) {
		_actual->onError(exception);
	}
}

template<typename T>
void DefaultMonoSink<T>::error(const std::exception& exception) {
	_actual->onError(exception);
}

#endif //REACTORCORE_SRC_MONO_SINK_H
