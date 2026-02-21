#include <common/ProducerConsumerBuffer.hpp>

#include <random>
#include <thread>
#include <mutex>
#include <chrono>

#include <iostream>

int getRandomNumber()
{
	static std::random_device r;
	static std::default_random_engine e1(r());
	static std::uniform_int_distribution<int> uniform_dist(1, 100);

	int value = uniform_dist(e1);

	return value;
}

int getRandomTime(int min, int max)
{
	static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	static std::mt19937 e1(seed);
	static std::uniform_int_distribution<int> uniform_dist(min, max);

	int value = uniform_dist(e1);

	return value;
}

int getRandomTime2(int min, int max)
{
	static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	static std::mt19937 e1(seed);
	static std::uniform_int_distribution<int> uniform_dist(min, max);

	int value = uniform_dist(e1);

	return value;
}



std::mutex printMutex;

void thread1Handler(com::ProducerConsumerBuffer<int>& buffer)
{
	for(int i = 0; i < 50; ++i)
	{
		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(getRandomTime(10, 100)));
		int value = getRandomNumber();
		buffer.push(value);
		std::lock_guard<std::mutex> lock(printMutex);
		std::cout << "[thread 1]: pushed " << value << "\n";
	}
}

void thread2Handler(com::ProducerConsumerBuffer<int>& buffer)
{
	for(int i = 0; i < 50; ++i)
	{
		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(getRandomTime2(1000, 2000)));
		int value = buffer.pop();
		std::lock_guard<std::mutex> lock(printMutex);
		std::cout << "[thread 2]: poped " << value << "\n";
	}
}

int main()
{
	com::ProducerConsumerBuffer<int> m_buffer(10);
	std::thread thread1(thread1Handler, std::ref(m_buffer));
	std::thread thread2(thread2Handler, std::ref(m_buffer));
	thread1.join();
	thread2.join();
	return 0;
}
