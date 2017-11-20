#include "ProcessLister.h"

#include <QDirIterator>
#include <iostream>
#include <QtCore/QtCore>

std::vector<Process> ProcessLister::getRunningProcesses() const
{
#ifdef __linux__
    std::vector<Process> processes;

    QDirIterator proc("/proc");
    while (proc.hasNext())
    {
        auto path = proc.next();
        auto dir = QFileInfo(path).baseName();

        bool parsed;
        uint32_t pid = dir.toUInt(&parsed, 10);

        if (parsed)
        {
            QFile cmdline(QDir::cleanPath(path + QDir::separator() + "exe"));
            processes.emplace_back(pid, cmdline.symLinkTarget().toStdString());
        }
    }

    return processes;
#elif _WIN32
    // TODO
#endif
}
