#pragma once
#include <string>
#include <vector>
#include <algorithm>

class Event {
    public:
    virtual ~Event() = default;
};

class Observer {
    public:
    virtual void onEvent(const Event *event) = 0;
    virtual ~Observer() = default;
};

class EventDispatcher {
    public:
    static EventDispatcher& get() {
        static EventDispatcher instance;
        return instance;
    }
    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;

    void registerObserver(Observer *observer) {
        observers.push_back(observer);
    }

    void unregisterObserver(Observer *observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void dispatchEvent(const Event *event) {
        for (Observer* observer : observers) {
            observer->onEvent(event);
        }
    }
    private:
    std::vector<Observer*> observers;

    EventDispatcher() = default;
    ~EventDispatcher() = default;  
};