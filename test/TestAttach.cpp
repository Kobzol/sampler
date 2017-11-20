#include <catch.hpp>
#include "Utility.h"

#include <unistd.h>
#include <cassert>
#include <cstring>

TEST_CASE("Sampler attach doesn't stop target process")
{
    int out = -1, in = -1;
    uint32_t pid = createProcess("loop-wait-for-input", out, in);
    auto sampler = createSampler(10);
    sampler->attach(pid);

    sleep(1);

    write(in, "a\n", 2);

    char buffer[128];
    ssize_t len = read(out, buffer, sizeof(buffer));
    REQUIRE(len > 0);
    REQUIRE(buffer[0] == '0');

    sampler->stop();
    int a = 5;
};
