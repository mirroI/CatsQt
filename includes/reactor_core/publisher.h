//
// Created by Алексей Сиротин on 19.01.2022.
//

#ifndef REACTORCORE_SRC_PUBLISHER_H
#define REACTORCORE_SRC_PUBLISHER_H

#include <functional>

#include "subscriber.h"

template<typename T>
class Publisher {
 public:
	virtual void subscribe(Subscriber<T> *subscriber) = 0;
};

template<typename T>
class CorePublisher : public Publisher<T> {
 public:
	virtual void subscribe(CoreSubscriber<T> *subscriber) = 0;
};

#endif //REACTORCORE_SRC_PUBLISHER_H
