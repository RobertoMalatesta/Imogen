// https://github.com/CedricGuillemet/Imogen
//
// The MIT License(MIT)
//
// Copyright(c) 2019 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#ifdef EMSCRIPTEN

#include <emscripten.h>
#include <SDL.h>
#include <GLES3/gl3.h>

typedef int TaskSetPartition;
struct PinnedTask
{
    PinnedTask(int) {}
    virtual void Execute() = 0;
};

struct TaskSet
{
    virtual void ExecuteRange(TaskSetPartition range, uint32_t threadnum) = 0;
};

struct TaskScheduler
{
    void Initialize() {}
    void WaitforAllAndShutdown() {}
    void RunPinnedTasks() {}

    void AddPinnedTask(PinnedTask *task)
    {
        task->Execute();
    }
    void WaitforTask(PinnedTask *task) { }
    void AddTaskSetToPipe(TaskSet* taskSet)
    {
        taskSet->ExecuteRange(0, 0);
    }
};


#elif WIN32

#include <SDL.h>
#include <GL/gl3w.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <shellapi.h>
#include "ffmpegCodec.h"
#include "libtcc/libtcc.h"
#include "ffmpegCodec.h"
#include "TaskScheduler.h"
#include "nfd.h"

#define USE_FFMPEG 1
#define USE_PYTHON 1
#define USE_GLDEBUG 1
#define USE_LIBTCC 1

typedef enki::IPinnedTask PinnedTask;
typedef enki::ITaskSet TaskSet;
typedef enki::TaskSetPartition TaskSetPartition;
typedef enki::TaskScheduler TaskScheduler;

#else
    
#error unknown platform

#endif