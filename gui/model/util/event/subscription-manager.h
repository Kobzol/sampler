#pragma once

#include <utility>
#include <vector>
#include <functional>

class SubscriptionManager
{
public:
    SubscriptionManager() = default;
    ~SubscriptionManager();

    SubscriptionManager(const SubscriptionManager& other) = delete;
    SubscriptionManager operator=(const SubscriptionManager& other) = delete;
    SubscriptionManager(const SubscriptionManager&& other) = delete;

    void add(std::function<void ()> disposable);
    void operator+=(std::function<void ()> disposable);

    void dispose();

private:
    std::vector<std::function<void ()>> disposables;
};
