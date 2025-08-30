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
    Observer();
    virtual void onEvent(const Event *event) = 0;
    virtual ~Observer();
};

class EventDispatcher {
    public:
    static EventDispatcher& get() {
        static EventDispatcher instance;
        return instance;
    }
    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;

    void registerObserver(Observer *observer);

    void unregisterObserver(Observer *observer);

    void dispatchEvent(const Event *event);
    
    private:
    std::vector<Observer*> observers;

    EventDispatcher() = default;
    ~EventDispatcher() = default;  
};