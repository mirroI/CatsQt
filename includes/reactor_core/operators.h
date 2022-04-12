//
// Created by Алексей Сиротин on 05.02.2022.
//

#ifndef REACTORCORE_SRC_OPERATORS_H
#define REACTORCORE_SRC_OPERATORS_H

#include "publisher.h"

class Operators {
 public:
	template<typename Base, typename T>
	static inline bool instanceof(const T*);

	template<typename T>
	static CoreSubscriber<T> *toCoreSubscriber(Subscriber<T> *subscriber);
};

template<typename Base, typename T>
bool Operators::instanceof(const T *) {
	return std::is_base_of_v<Base, T>;
}

template<typename T>
CoreSubscriber<T> *Operators::toCoreSubscriber(Subscriber<T> *subscriber) {
	if (CoreSubscriber<T> *coreSubscriber = static_cast<CoreSubscriber<T> *>(subscriber)) {
		return coreSubscriber;
	}

	return nullptr;
}

#endif //REACTORCORE_SRC_OPERATORS_H
