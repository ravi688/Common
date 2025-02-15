#include <catch2/catch_test_macros.hpp>
#include <common/multi_buffer.h>

static void run_test(u32 capacity, u32 sub_capacity)
{
	buffer_t buf = buf_create(sizeof(int), 3, 0);
	REQUIRE(buf_get_capacity(&buf) == 3);
	buf_ensure_capacity(&buf, 10);
	REQUIRE(buf_get_capacity(&buf) == 12);
	buf_resize(&buf, 23);
	REQUIRE(buf_get_capacity(&buf) == 23);
	buf_push_pseudo(&buf, 23);
	REQUIRE(buf_get_capacity(&buf) == 23);
	REQUIRE(buf_get_element_count(&buf) == 23);
	buf_insert_pseudo(&buf, 13, 10);
	REQUIRE(buf_get_element_count(&buf) == 33);
	REQUIRE(buf_get_capacity(&buf) == 46);
	buf_remove_pseudo(&buf, 13, 10);
	REQUIRE(buf_get_capacity(&buf) == 46);
	REQUIRE(buf_get_element_count(&buf) == 23);
	buf_insert_pseudo(&buf, 13, 10);
	REQUIRE(buf_get_element_count(&buf) == 33);
	REQUIRE(buf_get_capacity(&buf) == 46);
	buf_free(&buf);

	multi_buffer_t mbuf;
	multi_buffer_create(sizeof(int), capacity, &mbuf);

	REQUIRE(multi_buffer_get_count(&mbuf) == 0);
	REQUIRE(multi_buffer_get_capacity(&mbuf) == capacity);
	REQUIRE(multi_buffer_get_combined_sub_buffers_count(&mbuf) == 0);
	REQUIRE(multi_buffer_get_sub_buffer_count(&mbuf) == 0);

	sub_buffer_handle_t handles[4];
	for(u32 i = 1; i <= 4; ++i)
	{
		for(u32 j = 0; j < i; ++j)
		{
			u32 _sub_capacity = (j%2) ? (sub_capacity * j) : (sub_capacity + j);
			AUTO handle = multi_buffer_sub_buffer_create(&mbuf, _sub_capacity);
			handles[j] = handle;

			REQUIRE(multi_buffer_sub_buffer_get_count(&mbuf, handle) == 0);
			REQUIRE(multi_buffer_sub_buffer_get_capacity(&mbuf, handle) >= _sub_capacity);

			multi_buffer_sub_buffer_clear(&mbuf, handle);
			REQUIRE(multi_buffer_sub_buffer_get_count(&mbuf, handle) == 0);

			int value = 0;
			for(u32 k = 0; k < j; ++k, ++value)
			{
				multi_buffer_sub_buffer_push(&mbuf, handle, &value);
				REQUIRE(multi_buffer_sub_buffer_get_count(&mbuf, handle) == static_cast<buf_ucount_t>(value + 1));
			}
			value = 0;
			for(u32 k = 0; k < j; ++k, ++value)
			{
				int out_value;
				multi_buffer_sub_buffer_get_at(&mbuf, handle, k, &out_value);
				REQUIRE(out_value == value);
			}
			value = 0;
			for(u32 k = 0; k < j; ++k, value += 2)
				multi_buffer_sub_buffer_set_at(&mbuf, handle, k, &value);
			value = 0;
			for(u32 k = 0; k < j; ++k, value += 2)
			{
				int out_value;
				multi_buffer_sub_buffer_get_at(&mbuf, handle, k, &out_value);
				REQUIRE(out_value == value);
			}
		}
		for(u32 j = 0; j < u32_min(i, 10); ++j)
			multi_buffer_sub_buffer_destroy(&mbuf, handles[j]);
		for(u32 j = 10; j < i; ++j)
		{
			AUTO handle = handles[j];
			u32 count = multi_buffer_sub_buffer_get_count(&mbuf, handle);
			int value = j;
			multi_buffer_sub_buffer_push(&mbuf, handle, &value);
			REQUIRE(multi_buffer_sub_buffer_get_count(&mbuf, handle) == (count + 1));
			multi_buffer_sub_buffer_destroy(&mbuf, handle);
		}
		REQUIRE(multi_buffer_get_sub_buffer_count(&mbuf) == 0);
	}

	multi_buffer_clear(&mbuf);
	REQUIRE(multi_buffer_get_sub_buffer_count(&mbuf) == 0);
	REQUIRE(multi_buffer_get_count(&mbuf) == 0);

	multi_buffer_free(&mbuf);
}

TEST_CASE( "Multi Buffer", "[multi_buffer_test]" )
{
	run_test(0, 0);
	run_test(1, 0);
	run_test(1, 4);
	run_test(4, 5);
}
