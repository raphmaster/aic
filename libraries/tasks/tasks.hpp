/*
 * Tasks Register
 */

#ifndef tasks_hpp
#define tasks_hpp

#define MAX_TASKS 32

namespace tasks
{
    typedef void (*basicTask)(); //pointer to a task with no argument
    typedef void (*paramTask)(void*); //pointer to a task with one argument
    
    //store task pointer
    union uniTask
    {
	basicTask basic;
	paramTask param;
    };
    bool operator==(const uniTask& a, const uniTask& b);

    bool registerTask(basicTask task); //register task with no argument
    bool registerTask(paramTask task, void* param); //register task with one argument
    bool unregisterTask(basicTask task); //delete registration of task with no argument
    bool unregisterTask(paramTask task, void* param); //delete registration of task with one argument
    void doTasks(); //execute registered tasks

    namespace //anonymous namespace
    {
	uniTask _tasks[MAX_TASKS]; //registered tasks array
	void* _params[MAX_TASKS]; //registered tasks argument array
	bool _activeTasks[MAX_TASKS]; //active registered tasks array
    }
}

#endif
