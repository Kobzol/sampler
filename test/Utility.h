#pragma once

#include <string>
#include "../sampler/sampler.h"

uint32_t createProcess(const std::string& program, int& out, int& in);

std::unique_ptr<Sampler> createSampler(uint32_t frequency = 10, bool catchErrors = true);
