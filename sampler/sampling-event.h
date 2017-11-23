#pragma once

enum class SamplingEvent
{
    Start = 0,
    Pause,
    Resume,
    Exit,
    TaskCreate,
    TaskCollect,
    TaskExit
};
