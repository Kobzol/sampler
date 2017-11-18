#pragma once

#include "../sampler.h"

#include <Windows.h>

class DebugeventSampler: public Sampler
{
public:
    explicit DebugeventSampler(uint32_t interval);

    void connect(uint32_t pid) override;
    void connect(const std::vector<std::string>& args) override;

    void stop() override;

    void waitForExit() override;

protected:
    std::unique_ptr<StacktraceCollector> createCollector(uint32_t pid) override;

private:
    void loop();
    bool handleEvent(DEBUG_EVENT* event, std::vector<DWORD>& newThreads);
    void initializeThreads(DWORD pid, DWORD tidIgnore);
	void initializeSymbols();
    void stopTasks();

    std::atomic<bool> running;
    std::thread loopThread;

	bool symbolsInitialized = false;
	HANDLE process;
};
