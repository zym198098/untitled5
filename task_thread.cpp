/*
 * File:   task_thread.cpp
 * Author: Administrator
 *
 * Created on 2010年5月10日, 上午10:57
 */

#include "task_thread.h"

void TaskThread::Execute()
{
    start();
}

TaskThread::TaskThread(void* userdata, THREAD_TASK process,
                       THREAD_TASK preprocess, THREAD_TASK postprocess)
{
    data_ = userdata;
    process_ = process;
    preprocess_ = preprocess;
    postprocess_ = postprocess;
    //preprocess
    if (preprocess_)
    {
        preprocess_(data_);
    }
}

TaskThread::TaskThread() : process_(NULL), preprocess_(NULL), postprocess_(NULL)
{
}

TaskThread::~TaskThread()
{
    //postprocess
    if (postprocess_)
    {
        postprocess_(data_);
    }
}

void* TaskThread::Entry()
{
    //process
    if (process_)
    {
        process_(data_);
    }

    return 0;
}

void TaskThread::RegistRunTask(THREAD_TASK fr)
{
    process_ = fr;
}

void TaskThread::RegistSetupTask(THREAD_TASK fs)
{
    preprocess_ = fs;
}

void TaskThread::RegistTeardownTask(THREAD_TASK ft)
{
    postprocess_ = ft;
}

void TaskThread::run()
{
    Entry();
}
