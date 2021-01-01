/*
#include "ThreadPool.h"
#include <iostream>

// to avoid interleaved outputs
std::mutex ThreadPool::output_guard;
// synchro stuff
std::deque<std::future<NkMatrix>> ThreadPool::task_queue;
std::mutex ThreadPool::task_guard;
std::condition_variable ThreadPool::task_signal;
int ThreadPool::pending_task_count(0);

ThreadPool::ThreadPool()
{
}

ThreadPool::~ThreadPool()
{
}

NkMatrix ThreadPool::runTask(NkMatrix&& m0, NkMatrix&& m1, bool mult)
{
	NkMatrix res;
	if(mult)
		res = m0 * m1;
	else
		res = m0 + m1;

	// be sure at least one unit is awaken
	task_signal.notify_one();

	// terminate task
	const std::lock_guard<std::mutex> lock(task_guard);
	if (!--pending_task_count)
	{
		// ## EMIT SHUTDOWN SIGNAL ##
		// awake all idle units for auto-shutdown
		task_signal.notify_all();
	}

	return res;
}

NkMatrix ThreadPool::run(NkMatrix&& matrix)
{
		// divide matrix multiplication parts into tasks and create them
	int divRow = matrix.rows() / 2;
	NkMatrix m0(divRow, divRow);
	NkMatrix m1(matrix.rows() - divRow, divRow);
	NkMatrix m2(divRow, matrix.cols() - divRow);
	NkMatrix m3(matrix.rows() - divRow, matrix.cols() - divRow);

	for (int k = 0; k < matrix.outerSize(); ++k)
	{
		for (Eigen::SparseMatrix<int>::InnerIterator it(matrix, k); it; ++it)
		{
			if (it.row() < divRow)
			{
				if (it.col() < divRow)
				{
					m0.insert(it.row(), it.col()) = it.value();
				}
				else
				{
					m1.insert(it.row(), it.col() - divRow) = it.value();
				}
			}
			else
			{
				if (it.col() < divRow)
				{
					m2.insert(it.row() - divRow, it.col()) = it.value();
				}
				else
				{
					m3.insert(it.row() - divRow, it.col() - divRow) = it.value();
				}
			}
		}
	}

	std::cout << "m =" << std::endl << matrix << std::endl;
	
	NkMatrix m00 = m0 * m0;
	NkMatrix m01 = m1 * m2;
	NkMatrix m02 = m01 + m00;
	std::cout << "m02 =" << std::endl << m02 << std::endl;

	NkMatrix m = matrix * matrix;
	std::cout << "m =" << std::endl << m << std::endl;


	// spawn tasks
	{
		auto task = std::async(std::launch::deferred, &ThreadPool::runMultTask, this, std::move(m0), std::move(m0), true);
		{
			const std::lock_guard<std::mutex> lock(task_guard);
			task_queue.emplace_back(std::move(task));
			++pending_task_count;
		}
	}
	{
		auto task = std::async(std::launch::deferred, &ThreadPool::runMultTask, this, std::move(m1), std::move(m2), true);
		{
			const std::lock_guard<std::mutex> lock(task_guard);
			task_queue.emplace_back(std::move(task));
			++pending_task_count;
		}
	}

	NkMatrix result(matrix.rows(), matrix.cols());
	// launch processing units
	std::list<std::future<void>> units;
	const unsigned int thead_multiplicator = 1;
	for (auto n = std::thread::hardware_concurrency() * thead_multiplicator; --n;)
	{
		units.emplace_back(std::async(std::launch::async, [n, &result]() {
			// ## LAUNCH task ##
			std::unique_lock<std::mutex> lock(task_guard);
			while (true)
			{
				if (!task_queue.empty())
				{
					// pick a task
					auto task = std::move(task_queue.front());
					task_queue.pop_front();

					lock.unlock(); // unlock while processing task
					{
						// process task
						task.wait();
						{
							const std::lock_guard<std::mutex> combineMatrixGuard(output_guard);
							// combine matrix part TODO
							NkMatrix m = task.get();
							//result += m;
						}
					}
					lock.lock(); // reacquire lock
				}
				else if (!pending_task_count)
					break;
				else
				{
					// ## PAUSE / IDLE ##
					task_signal.wait(lock);
					// ## AWAKE ##
				}
			}

			// ## TERMINATE ##
		}));
	}

	// wait for shutdown
	for (auto& unit : units)
	{
		unit.wait();
	}

	return result;

}*/