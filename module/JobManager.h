#pragma once
#include <thread>
#include <array>
#include <functional>
#include <queue>
#include <list>
#include <mutex>

template<typename TaskResult, std::size_t NumWorkers>
class JobManager
{
public:
	using Task = std::function<TaskResult()>;
	using TaskCompleteCallback = std::function<void(const TaskResult& result)>;

	JobManager()
	{
	}

	void Start()
	{
		for (int i = 0; i < NumWorkers; ++i)
		{
			_workers[i] = std::thread(&JobManager::RunWorker, this);
		}
	}

	void PushTask(Task task, TaskCompleteCallback completeCallback)
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		_tasks.push({ task, completeCallback });
	}

	void RunWorker()
	{
		for (;;)
		{
			// Wait if there's no task for us
			_mutex.lock();
			if (_tasks.empty())
			{
				_mutex.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			// Get next task
			std::pair<Task, TaskCompleteCallback>& task = _tasks.front();
			_mutex.unlock();

			// Run task
			TaskResult result = task.first();

			// Put result into completed tasks list
			std::lock_guard<std::mutex> lock{ _mutex };
			_completedTasks.push_back({ result, task.second });

			// Remove task
			_tasks.pop();
		}
	}

	void SpreadResults()
	{
		std::lock_guard<std::mutex> lock{ _mutex };

		for (auto& task : _completedTasks)
		{
			task.second(task.first);
		}
	}

private:
	std::array<std::thread, NumWorkers> _workers;
	std::queue<std::pair<Task, TaskCompleteCallback>> _tasks;
	std::list<std::pair<TaskResult, TaskCompleteCallback>> _completedTasks;

	std::mutex _mutex;
};
