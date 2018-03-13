#include <tasks.hpp>
#include <algo.hpp>

namespace tasks
{
    bool operator==(const uniTask& a, const uniTask& b)
    {
	if (a.basic == b.basic || a.param == b.param) return true;
	else return false;
    }

    /*
     * register task without argument
     * task: pointer to function to execute as task
     * return: true if registration was successfull, otherwise false
     */
    bool registerTask(basicTask task)
    {
	unsigned short index = algo::find<bool>(false, 0, MAX_TASKS, _activeTasks); //find first inactive task
	if (index <= MAX_TASKS) //if inactive task was found
	{
	    _activeTasks[index] = true; //activate task
	    _tasks[index].basic = task; //save task function pointer
	    _params[index] = 0; //save as basicTask
	    return true;
	}
	else return false;
    }

    /*
     * register task with one argument
     * task: pointer to function to execute as task
     * param: argument pointer to be passed to task function
     * return: true if registration was successfull, otherwise false
     */
    bool registerTask(paramTask task, void* param)
    {
	unsigned short index = algo::find<bool>(false, 0, MAX_TASKS, _activeTasks); //find first inactive task
	if (index <= MAX_TASKS) //if inactive task was found
	{
	    _activeTasks[index] = true; //activate task
	    _tasks[index].param = task; //save task function pointer
	    _params[index] = param; //save task function argument pointer
	    return true;
	}
	else return false;
    }

    /*
     * delete registration of task without argument
     * task: pointer to function executed by task
     * return: true if task was successfully found and unregistered, otherwise false
     */
    bool unregisterTask(basicTask task)
    {
	uniTask temp; //create temporary task
	temp.basic = task; //store basicTask in uniTask
	unsigned short index = algo::find<uniTask>(temp, 0, MAX_TASKS, _tasks); //find first matching task
	if (index <= MAX_TASKS) //if matching task found
	{
	    _activeTasks[index] = false; //deactivate task
	    _tasks[index].basic = 0; //delete task function pointer
	    return true;
	}
	else return false;
    }

    /*
     * delete registration of task with one argument
     * task: pointer to function executed by task
     * param: argument pointer to be passed to task function
     * return: true if task was successfully found and unregistered, otherwise false
     */
    bool unregisterTask(paramTask task, void* param)
    {
	uniTask temp; //create temporary task
	temp.param = task; //store paramTask in uniTask
	unsigned short index = algo::find<uniTask>(temp, 0, MAX_TASKS, _tasks); //find first matching task
	if (index <= MAX_TASKS) //if matching task found
	{
	    _activeTasks[index] = false; //deactivate task
	    _tasks[index].param = 0; //delete task function pointer
	    _params[index] = 0; //delete task function argument pointer
	    return true;
	}
	else return false;
    }

    /*
     * execute tasks in registration order
     * if a task blocks program execution, other tasks will be executed only when blocking task is terminated
     * use of blocking task is not recommended
     */
    void doTasks()
    {
	static unsigned short index;
	for (index = 0; index < MAX_TASKS; index++) //for each task slot
	{
	    if (_params[index] && _tasks[index].param && _activeTasks[index]) _tasks[index].param(_params[index]); //if task have argument, task function is valid and task is enabled, execute task
	    else if (!_params[index] && _tasks[index].basic && _activeTasks[index]) _tasks[index].basic(); //if task dont have argument, task function is valid and task is enabled, execute task
	}
    }
}
