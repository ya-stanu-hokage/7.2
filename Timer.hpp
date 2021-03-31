#pragma once

#include <iostream>
#include <chrono>
class Timer
{
public:
	using clock_t = std::chrono::steady_clock;
	using time_point_t = clock_t::time_point;
	using time_micro_t = std::chrono::microseconds;

	Timer(const std::string &a) : m_name(a), m_begin(clock_t::now()), m_c(time_micro_t::duration::zero()), m_pause_value(false) {}
	~Timer() noexcept
	{
		try
		{
			if (m_pause_value)
				std::cout << m_name << ": " << m_c.count() << " microseconds " << std::endl;
			else
			{
				auto end = clock_t::now();
				std::cout << m_name << ": " << (std::chrono::duration_cast <
					std::chrono::microseconds> (end - m_begin) + m_c).count() << " microseconds " << std::endl;
			}
		}
		catch (...)
		{

		}
	}

	void Pause()
	{
		if (!m_pause_value)
		{
			auto end_pause = clock_t::now();
			m_c += std::chrono::duration_cast <std::chrono::microseconds> (end_pause - m_begin);
			m_pause_value = true;
		}
	}
	void Continue()
	{
		if (m_pause_value)
		{
			m_begin = clock_t::now();
			m_pause_value = false;
		}
	}

	const int getMc() { return m_c.count(); }
	const std::string& getName() { return m_name; }
private:
	std::string m_name;
	time_point_t m_begin;
	time_micro_t m_c;
	bool m_pause_value;

};