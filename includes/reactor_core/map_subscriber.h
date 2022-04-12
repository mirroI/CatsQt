//
// Created by Алексей Сиротин on 06.03.2022.
//

#ifndef REACTORCORE_SRC_MAP_SUBSCRIBER_H
#define REACTORCORE_SRC_MAP_SUBSCRIBER_H

#include "subscriber.h"

template<typename IN, typename OUT>
class MapSubscriber : public CoreSubscriber<IN>, public Subscription {
 private:
	CoreSubscriber<OUT> *_subscriber;
	std::function<OUT *(IN *)> _mapper;

	bool done;

 public:
	explicit MapSubscriber(CoreSubscriber<OUT> *subscriber, std::function<OUT *(IN *)> mapper);

	void onSubscribe(Subscription *subscription) override;
	void onNext(IN *value) override;
	void onError(const std::exception& exception) override;
	void onComplete() override;
};

template<typename IN, typename OUT>
MapSubscriber<IN, OUT>::MapSubscriber(CoreSubscriber<OUT> *subscriber, std::function<OUT *(IN *)> mapper) : _subscriber(
	subscriber), _mapper(mapper) {
	done = false;
}

template<typename IN, typename OUT>
void MapSubscriber<IN, OUT>::onSubscribe(Subscription *subscription) {
	_subscriber->onSubscribe(this);
}

template<typename IN, typename OUT>
void MapSubscriber<IN, OUT>::onNext(IN *value) {
	if (done) {
		return;
	}

	OUT *newValue = _mapper(value);
	_subscriber->onNext(newValue);
}

template<typename IN, typename OUT>
void MapSubscriber<IN, OUT>::onError(const std::exception& exception) {
	if (done) {
		return;
	}

	done = true;
	_subscriber->onError(exception);
}

template<typename IN, typename OUT>
void MapSubscriber<IN, OUT>::onComplete() {
	if (done) {
		return;
	}

	done = true;
	_subscriber->onComplete();
}

#endif //REACTORCORE_SRC_MAP_SUBSCRIBER_H
