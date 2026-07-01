#include <common/DynamicPoolFast.hpp>
#include <common/ProducerConsumerBuffer.hpp>

#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <cassert>

static std::atomic<std::size_t> gPool1DataCounter = 0;
static std::atomic<std::size_t> gPool2DataCounter = 0;

static std::vector<int> gGeneratedRandomNumberList;
static std::vector<int> gProcessedRandomNumberList;

static int GenerateRandomNumber()
{
	static std::random_device r;
	static std::default_random_engine e1(r());
	static std::uniform_int_distribution<int> uniform_dist(1, 100);

	int value = uniform_dist(e1);
	gGeneratedRandomNumberList.push_back(value);

	return value;
}

struct Context
{
	com::DynamicPoolFast<int> pool12;
	com::DynamicPoolFast<int> pool23;

	std::mutex pool12Mutex;
	std::mutex pool23Mutex;

	com::ProducerConsumerBuffer<com::DynamicPoolFast<int>::ElementType> pipe1;
	com::ProducerConsumerBuffer<com::DynamicPoolFast<int>::ElementType> pipe2;
	com::ProducerConsumerBuffer<com::DynamicPoolFast<int>::ElementType> pipe3;

	Context() : pool12([]()
	{
		++gPool1DataCounter;
		return 0;
	},
	[](int&)
	{
		--gPool1DataCounter;
	}),
	pool23([]()
	{
		++gPool2DataCounter;
		return 0;
	},
	[](int&)
	{
		--gPool2DataCounter;
	})
	{

	}
};

static Context gContext;
static constexpr std::size_t gDataCount = 100;

static void _pipe1Process(com::DynamicPoolFast<int>::ElementType& output)
{
	*output = GenerateRandomNumber();
}

static void pipe1Process()
{
	auto dataCount = gDataCount;
	while(dataCount)
	{
		com::DynamicPoolFast<int>::ElementType value;
		{
			std::lock_guard<std::mutex> lock(gContext.pool12Mutex);
			value = gContext.pool12.get();	
		} 
		_pipe1Process(value);
		gContext.pipe1.push(value);
		--dataCount;
	}
}

static void _pipe2Process(const com::DynamicPoolFast<int>::ElementType& input, com::DynamicPoolFast<int>::ElementType& output)
{
	*output = *input;
}

static void pipe2Process()
{
	auto dataCount = gDataCount;
	while(dataCount)
	{
		// Take out the output of pipe1
		auto value = gContext.pipe1.pop();

		// Process it
		com::DynamicPoolFast<int>::ElementType value2;
		{
			std::lock_guard<std::mutex> lock(gContext.pool23Mutex);
			value2 = gContext.pool23.get();
		}
		_pipe2Process(value, value2);
		
		// Push the output of pipe2
		gContext.pipe2.push(value2);

		// Return the output of pipe1 back to pipe1's pool
		{
			std::lock_guard<std::mutex> lock(gContext.pool12Mutex);
			gContext.pool12.put(value);
		}
		--dataCount;
	}
}

static void pipe3Process()
{
	auto dataCount = gDataCount;
	while(dataCount)
	{
		// Take out the output of pipe1
		auto value = gContext.pipe2.pop();

		gProcessedRandomNumberList.push_back(*value);

		// Return the output of pipe2 back to pipe2's pool
		{
			std::lock_guard<std::mutex> lock(gContext.pool23Mutex);
			gContext.pool23.put(value);
		}
		--dataCount;
	}
}


// 3 stage pipeline
// Pipe 1: generate input and put into Pipe 2
// Pipe 2: process input and put into Pipe 3
// Pipe 3: process input and compare with pipe 1's generated output
int main()
{
	std::thread pipe1Thread(pipe1Process);
	std::thread pipe2Thread(pipe2Process);
	std::thread pipe3Thread(pipe3Process);

	if(pipe1Thread.joinable())
		pipe1Thread.join();
	if(pipe2Thread.joinable())
		pipe2Thread.join();
	if(pipe3Thread.joinable())
		pipe3Thread.join();

	assert(gProcessedRandomNumberList.size() == gGeneratedRandomNumberList.size());
	assert(gProcessedRandomNumberList.size() == gDataCount);

	for(std::size_t i = 0; i < gGeneratedRandomNumberList.size(); ++i)
	{
		assert(gGeneratedRandomNumberList[i] == gProcessedRandomNumberList[i]);
	}

	std::cout << "gDataCount: " << gDataCount << "\n";
	std::cout << "gPool1DataCounter: " << gPool1DataCounter << "\n";
	std::cout << "gPool2DataCounter: " << gPool2DataCounter << "\n";

	assert(gPool1DataCounter <= gDataCount);
	assert(gPool2DataCounter <= gDataCount);

	return 0;
}
