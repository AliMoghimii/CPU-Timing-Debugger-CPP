
#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>

// Useful link https://www.strchr.com/performance_measurements_with_rdtsc

extern "C"
{
#ifdef _WIN64
	INT64 c_ext_getCPUClock();
#else
	INT32 c_ext_getCPUClock();
#endif
}


int main(int, char**) {
	// Setup experiment
	const auto numOfIterations = 250000;
	std::cout << "Number of iterations: " << numOfIterations << std::endl;

	const auto processorInfinity = 2;
	auto* const thread = GetCurrentThread();
	auto* const process = GetCurrentProcess();

	// Set thread and process priorities to very high
	if (!SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL)) {
		SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST);
	}
	if (!SetPriorityClass(process, REALTIME_PRIORITY_CLASS)) {
		SetPriorityClass(process, HIGH_PRIORITY_CLASS);
	}
	SetProcessAffinityMask(process, processorInfinity);
	SetThreadAffinityMask(thread, processorInfinity);

	// Measure the overhead in calculating the time
	std::vector<DWORD> overheadTimes;
	for (auto i = 0; i < numOfIterations; i++) {
		const auto startTime = c_ext_getCPUClock();

		// Deliberately no payload here

		const auto stopTime = c_ext_getCPUClock();
		overheadTimes.push_back(static_cast<int>(stopTime - startTime));
	}

	// Check for over optimization
	if (numOfIterations != overheadTimes.size()) {
		std::cout << "\nERROR: Optimizer removed the code" << std::endl;
		return -1;
	}

	// Calculate the overhead
	sort(overheadTimes.begin(), overheadTimes.end());
	const auto overhead = overheadTimes[overheadTimes.size() / 2];
	std::cout << "\nOverhead duration: " << overhead << std::endl;

	// Dummy values to avoid the compiler optimizing out the test code
	auto dummyX = 1.0;

	// Run the actual experiment
	std::vector<DWORD> experimentTimes;
	for (auto i = 0; i < numOfIterations; i++) {
		const auto startTime = c_ext_getCPUClock();

		// BEGIN payload


		// END payload

		const auto stopTime = c_ext_getCPUClock();
		const auto duration = static_cast<int>(stopTime - startTime - overhead);
		experimentTimes.push_back(duration > 0 ? duration : 1);
	}

	// Reset the thread priorities
	SetThreadPriority(thread, THREAD_PRIORITY_IDLE);
	SetPriorityClass(process, IDLE_PRIORITY_CLASS);

	// Check for over optimization
	if (numOfIterations != experimentTimes.size()) {
		std::cout << "\nERROR: Optimizer removed the code" << std::endl;
		return -1;
	}

	// Calculate the median
	sort(experimentTimes.begin(), experimentTimes.end());
	std::cout << "\nMedian duration: " << experimentTimes[experimentTimes.size() / 2] << std::endl;

	// Calculate the mean
	auto sum = 0.0;
	const auto lo = static_cast<unsigned int>(experimentTimes.size() * 0.1f);
	const auto hi = static_cast<unsigned int>(experimentTimes.size() * 0.9f);
	for (auto i = lo; i < hi; i++)
		sum += experimentTimes[i];
	std::cout << "\nMean (80%) duration: " << (sum / (hi - lo)) << std::endl;

	// Output low and high samples
	const auto sampleSize = 100u;
	std::cout << "\nSample data [" << sampleSize << "]" << std::endl;
	std::cout << "........................" << std::endl;
	for (auto i = 0u; (i < sampleSize) && (i < experimentTimes.size()); i++)
		std::cout << experimentTimes[i] << " ";
	std::cout << "\n........................" << std::endl;
	for (auto i = experimentTimes.size() - sampleSize; i < experimentTimes.size(); i++)
		std::cout << experimentTimes[i] << " ";
	std::cout << "\n........................" << std::endl;

	// Print dummy x to avoid optimizer removing the loop
	std::cout << "\nx= " << dummyX << " (dummy value - ignore) \n" << std::endl;
}