#pragma once

#include <QObject>
#include <QCoreApplication>

template <typename Fun>
void runOnUi(QObject* obj, Fun&& fn)
{
    struct Event : public QEvent
    {
        using F = typename std::decay<Fun>::type;

        explicit Event(F&& function) : QEvent(QEvent::None), function(std::move(function)) {}
        explicit Event(const F& function) : QEvent(QEvent::None), function(function) {}
        ~Event() override
        {
            this->function();
        }

    private:
        F function;
    };
    QCoreApplication::postEvent(obj->thread() ? obj : static_cast<QObject*>(QCoreApplication::instance()),
                                new Event(std::forward<Fun>(fn)));
}
