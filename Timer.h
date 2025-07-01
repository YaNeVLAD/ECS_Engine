#pragma once

#include <chrono>
#include <iostream>
#include <string>

class Timer
{
public:
	Timer(std::string name)
		: m_name(std::move(name))
		, m_start(std::chrono::high_resolution_clock::now())
	{
	}

	~Timer()
	{
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();
		std::cout << m_name << " took " << duration << " ms" << std::endl;
	}

private:
	std::string m_name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};
