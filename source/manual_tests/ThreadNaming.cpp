#include <common/ThreadNaming.hpp>
#include <common/third_party/debug_break.h>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

// Running this test:
// gdb --args ./build/ThreadNaming
// (gdb) info threads
//
// Following should be the output:
// (gdb) info threads
//   Id   Target Id                     Frame
// * 1    Thread 7876.0x3098            trap_instruction () at ../include/common/third_party/debug_break.h:50
//   2    Thread 7876.0xa48             0x00007fff93005704 in ntdll!ZwWaitForWorkViaWorkerFactory () from C:\WINDOWS\SYSTEM32\ntdll.dll
//   3    Thread 7876.0x824             0x00007fff93005704 in ntdll!ZwWaitForWorkViaWorkerFactory () from C:\WINDOWS\SYSTEM32\ntdll.dll
//   4    Thread 7876.0x154c            0x00007fff93005704 in ntdll!ZwWaitForWorkViaWorkerFactory () from C:\WINDOWS\SYSTEM32\ntdll.dll
//   5    Thread 7876.0x2fe4 "Thread-1" 0x00007fff93001ad4 in ntdll!ZwWaitForSingleObject () from C:\WINDOWS\SYSTEM32\ntdll.dll
//   6    Thread 7876.0xdcc "Thread-2"  0x00007fff93001ad4 in ntdll!ZwWaitForSingleObject () from C:\WINDOWS\SYSTEM32\ntdll.dll

int main()
{
	std::atomic<int> counter = 0;
	std::mutex mutex;
	std::condition_variable cv;

	std::thread thread1([&]()
	{
		com::SetThreadName("Thread-1");
		++counter;
		{
			std::unique_lock<std::mutex> lock(mutex);
			cv.notify_one();
		}
		std::this_thread::sleep_for(std::chrono::duration<float, std::ratio<1, 1>>(2));
	});

	std::thread thread2([&]()
	{
		com::SetThreadName("Thread-2");
		++counter;
		{
			std::unique_lock<std::mutex> lock(mutex);
			cv.notify_one();
		}
		std::this_thread::sleep_for(std::chrono::duration<float, std::ratio<1, 1>>(2));
	});


	// Wait for both the threads to set their names
	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [&]() { return counter == 2; });
	}

	debug_break();

	if(thread1.joinable())
		thread1.join();
	if(thread2.joinable())
		thread2.join();

	return 0;
}
