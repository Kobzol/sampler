#pragma once

#include <vector>
#include <functional>

template <typename Owner, typename... Args>
class EventBroadcaster
{
    friend Owner;
    using CallbackType = std::function<void (Args...)>;

public:
    struct Listener
    {
    public:
        explicit Listener(CallbackType callback): callback(std::move(callback))
        {

        }
        CallbackType callback;
    };

    EventBroadcaster() = default;
    EventBroadcaster(const EventBroadcaster& other) = delete;
    EventBroadcaster operator=(const EventBroadcaster& other) = delete;
    EventBroadcaster(const EventBroadcaster&& other) = delete;

    std::function<void()> subscribe(CallbackType callback)
    {
        auto listener = std::make_unique<Listener>(callback);
        auto ptr = listener.get();
        this->listeners.push_back(std::move(listener));

        return [this, ptr]() {
            this->unsubscribe(ptr);
        };
    }

private:
    void invoke(Args... args)
    {
        for (auto& listener: this->listeners)
        {
            listener->callback(args...);
        }
    }
    void operator()(Args... args)
    {
        this->invoke(args...);
    }

    void unsubscribe(void* listener)
    {
        auto it = std::find_if(this->listeners.begin(), this->listeners.end(),
                               [listener](const std::unique_ptr<Listener>& l) {
                                   return l.get() == listener;
                               });
        if (it != this->listeners.end())
        {
            this->listeners.erase(it);
        }
    }

    std::vector<std::unique_ptr<Listener>> listeners;
};
