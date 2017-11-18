#include "stacktrace.h"

StackTrace::StackTrace(std::vector<FunctionRecord> functions): functions(std::move(functions))
{

}

const std::vector<FunctionRecord>& StackTrace::getFunctions() const
{
    return this->functions;
}
