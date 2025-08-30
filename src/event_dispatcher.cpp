#include "event_dispatcher.hpp"

Observer::Observer() {
    EventDispatcher::get().registerObserver(this);
}

Observer::~Observer() {
    EventDispatcher::get().unregisterObserver(this);
}

void EventDispatcher::registerObserver(Observer *observer) {
    observers.push_back(observer);
}

void EventDispatcher::unregisterObserver(Observer *observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void EventDispatcher::dispatchEvent(const Event *event) {
    for (Observer* observer : observers) {
        observer->onEvent(event);
    }
}