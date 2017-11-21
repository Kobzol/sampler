#include "ProcessLister.h"

#include <QDirIterator>
#include <iostream>
#include <QtCore/QtCore>

#ifdef __linux__
static std::string loadExe(const QString& taskPath)
{
    QFile file(QDir::cleanPath(taskPath + QDir::separator() + "exe"));
    return file.symLinkTarget().toStdString();
}
static std::string loadCmdline(const QString& taskPath)
{
    QFile file(QDir::cleanPath(taskPath + QDir::separator() + "cmdline"));
    file.open(QFile::ReadOnly | QFile::Text);
    return file.readAll().toStdString();
}
#endif

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
            auto cmdline = loadCmdline(path);
            processes.emplace_back(pid, cmdline.empty() ? loadExe(path) : cmdline);
        }
    }

    return processes;
#elif _WIN32
    // TODO
#endif
}
