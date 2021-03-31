#include <algorithm>
#include <future>
#include <fstream>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

#include "Timer.hpp"

class Threads_Guard
{
public:

	explicit Threads_Guard(std::vector < std::thread >& threads) :
		m_threads(threads)
	{}

	Threads_Guard(Threads_Guard const&) = delete;

	Threads_Guard& operator=(Threads_Guard const&) = delete;

	~Threads_Guard() noexcept
	{
		try
		{
			for (std::size_t i = 0; i < m_threads.size(); ++i)
			{
				if (m_threads[i].joinable())
				{
					m_threads[i].join();
				}
			}
		}
		catch (...)
		{

		}
	}

private:

	std::vector < std::thread >& m_threads;
};

template < typename Iterator, typename T >
struct accumulate_block
{
	T operator()(Iterator first, Iterator last)
	{
		return std::accumulate(first, last, T());
	}
};

template < typename Iterator, typename T >
T parallel_accumulate(Iterator first, Iterator last, T init, std::size_t Num_threads)
{
	const std::size_t length = std::distance(first, last);

	if (!length)
		return init;

	const std::size_t N_Threads = Num_threads;

	const std::size_t block_size = length / N_Threads;

	std::vector < std::future < T > > futures(N_Threads - 1);
	std::vector < std::thread >		  threads(N_Threads - 1);

	Threads_Guard guard(threads);

	Iterator block_start = first;

	for (std::size_t i = 0; i < (N_Threads - 1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);

		std::packaged_task < T(Iterator, Iterator) > task{
			accumulate_block < Iterator, T >() };

		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task), block_start, block_end);

		block_start = block_end;
	}

	T last_result = accumulate_block < Iterator, T >()(block_start, last);

	T result = init;

	for (std::size_t i = 0; i < (N_Threads - 1); ++i)
	{
		result += futures[i].get();
	}

	result += last_result;

	return result;
}

int main()
{
	std::vector < int > v(100000000);

	std::iota(v.begin(), v.end(), 1);



	std::fstream fout("Task7.2.txt", std::ios::out);


	for (auto i = 1U; i < 1000; i++)
	{
		Timer t("Time");

		parallel_accumulate(v.begin(), v.end(), 0, i);

		t.Pause();

		fout << i << ", " << t.getMc() << std::endl; 
	}
	return 0;
}