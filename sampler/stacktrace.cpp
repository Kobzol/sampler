#include "stacktrace.h"

StackTrace::StackTrace(std::vector<FunctionRecord> functions, size_t timestamp)
        : functions(std::move(functions)), timestamp(timestamp)
{

}

const std::vector<FunctionRecord>& StackTrace::getFunctions() const
{
    return this->functions;
}

size_t StackTrace::getTimestamp() const
{
    return this->timestamp;
}
