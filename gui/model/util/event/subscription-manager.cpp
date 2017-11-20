#include "subscription-manager.h"

SubscriptionManager::~SubscriptionManager()
{
    this->dispose();
}

void SubscriptionManager::add(std::function<void()> disposable)
{
    this->disposables.push_back(std::move(disposable));
}

void SubscriptionManager::operator+=(std::function<void()> disposable)
{
    this->add(std::move(disposable));
}

void SubscriptionManager::dispose()
{
    for (auto& disposable: this->disposables)
    {
        disposable();
    }
    this->disposables.clear();
}
