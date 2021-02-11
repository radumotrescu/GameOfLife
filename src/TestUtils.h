#ifndef TestUtils_h_include
#define TestUtils_h_include

#include <chrono>
#include <utility>

namespace TestUtils
{
	typedef std::chrono::high_resolution_clock::time_point TimePoint;

	template <typename F, typename... Args>
	double TimeIt(F func, Args&&... args)
	{
		auto t0 = TimePoint();
		func(args...);
		auto t1 = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	}

	class Timer
	{
	public:
		void Reset()
		{
			m_t0 = std::chrono::high_resolution_clock::now();
		}

		long long Elapsed() const
		{
			auto t1 = std::chrono::high_resolution_clock::now();
			return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - m_t0).count();
		}

	private:
		TimePoint m_t0 = std::chrono::high_resolution_clock::now();
	};
}

#endif