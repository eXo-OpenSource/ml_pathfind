#pragma once
#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <list>
#include <mutex>

template<typename TaskResult>
class JobManager
{
public:
	using Task = std::function<TaskResult()>;
	using TaskCompleteCallback = std::function<void(const TaskResult& result)>;

	JobManager(std::size_t numWorkers) : _numWorkers(numWorkers)
	{
	}

	void Start()
	{
		for (std::size_t i = 0; i < _numWorkers; ++i)
		{
			_workers.emplace_back(&JobManager::RunWorker, this);
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
	std::vector<std::thread> _workers;
	std::size_t _numWorkers;

	std::queue<std::pair<Task, TaskCompleteCallback>> _tasks;
	std::list<std::pair<TaskResult, TaskCompleteCallback>> _completedTasks;

	std::mutex _mutex;
};
