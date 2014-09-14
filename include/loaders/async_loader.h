#ifndef ASYNC_LOADER_H
#define ASYNC_LOADER_H

#include <future>
#include <unordered_map>
#include <string>

/*
 * An asynchronous resource loader: takes a task name,
 * the loading function (which should return bool indicating task status)
 * and the args for that function
 * You can wait for the tasks to finish via the wait function
 */
class AsyncLoader {
	std::unordered_map<std::string, std::future<bool>> tasks;

public:
	/*
	 * Add a task to be executed asynchronously and monitored
	 * by the async loader
	 */
	template<typename F, typename... Args>
	void run_task(const std::string &name, F &&f, Args&&... args){
		tasks.emplace(std::make_pair(name,
			std::async(std::launch::async, f, std::forward<Args>(args)...)));
	}
	/*
	 * Wait for all tasks to be completed and report their status
	 */
	inline void wait(){
		for (auto &task : tasks){
			if (task.second.get()){
				std::cout << "Task " << task.first << " completed successfully" << std::endl;
			}
			else {
				std::cout << "Task " << task.first << " failed to complete" << std::endl;
			}
		}
	}
};

#endif

