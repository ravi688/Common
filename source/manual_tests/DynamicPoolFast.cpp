// Running under valgrind
// valgrind --tool=memcheck ./build/DynamicPoolFast
// Should only show 3 allocations and 3 frees
// 1 allocation made by the runtime library/system
// 1 allocation made by com::DynamicPoolFast<int>::reserve()
// 1 allocation made by std::vector<int>::reserve()


#include <common/DynamicPoolFast.hpp>

int main()
{
	com::DynamicPoolFast<int> myPool([]()
			{
				static int counter = 0;
				return counter++;
			});
	myPool.reserve(50);
	std::vector<com::DynamicPoolFast<int>::ElementType> v;
	v.reserve(50);
       	for(int i = 0; i < 50; ++i)
		v.push_back(myPool.get());

	for(int i = 0; i < 50; ++i)
		myPool.putFast(v[i]);

	for(int i = 0; i < 100; ++i)
	{
		v.clear();
		for(int j = 0; j < 50; ++j)
		{
			v.push_back(myPool.get());
		}
		for(int j = 0; j < 50; ++j)
		{
			myPool.putFast(v[j]);
		}
	}
	return 0;
}
