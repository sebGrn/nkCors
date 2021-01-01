#pragma once
/*
#include <Eigen/Sparse>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <deque>

typedef Eigen::SparseMatrix<int> NkMatrix;

class ThreadPool
{
private:
	// synchro stuff
	// https://www.youtube.com/watch?v=2Xad9bCYbJE&list=PL1835A90FC78FF8BE&index=6
	static std::deque<std::future<NkMatrix>> task_queue;
	static std::mutex task_guard;
	static std::condition_variable task_signal;

	// to avoid interleaved outputs
	static std::mutex output_guard;

	// size of idle tasks
	static int pending_task_count;

private:
	NkMatrix runTask(NkMatrix&& m0, NkMatrix&& m1);

public:
	ThreadPool();
	~ThreadPool();

	NkMatrix run(NkMatrix&& matrix);
};
*/
