#ifndef TASK_THREAD_H
#define TASK_THREAD_H

//#include "wx/thread.h"
#include <QThread>
typedef void (*THREAD_TASK)(void* data);

/**
 *  通用任务线程,不依赖于wxWindow
 *
 */
class TaskThread : public QThread
{
public:
    TaskThread();
    TaskThread(void* userdata, THREAD_TASK process,
               THREAD_TASK preprocess = NULL, THREAD_TASK postprocess = NULL);
    virtual ~TaskThread();
    virtual void* Entry();
    void Execute();
    void run();
    void RegistRunTask(THREAD_TASK fr);
    void RegistSetupTask(THREAD_TASK fs);
    void RegistTeardownTask(THREAD_TASK ft);
private:
    void* data_;
    THREAD_TASK process_;
    THREAD_TASK preprocess_;
    THREAD_TASK postprocess_;
};

#endif  /* TASKTHREAD_H */

