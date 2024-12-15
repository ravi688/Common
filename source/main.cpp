
#include <common/debug.h>
#include <common/assert.h>
#include <common/UnorderedEraseSafeVectorProxy.hpp>
#include <iostream>

#include <stdio.h> /* stdio.h */
#include <common/static_string.h>
#include <common/utility.h>

#define TEST_NAME(name) std::cout << "Testing: " << name << std::endl
#define CHECK(to_be_checked) if(!(to_be_checked)) std::cout << "Failed: " << #to_be_checked << std::endl

static void static_string_test()
{
	debug_log_info("[static_string_t]");
	static_string_64_t str = static_string_64("Hello World");
	printf("static_string_64_t str: %s", str.data);
	puts("");
	printf("static_string_64_t len(str): %u", static_string_64_len(&str));
	puts("");
	static_string_64_t c_str;
	static_string_64_cpy(&c_str, &str);
	printf("static_string_64_t cpy(str): %s", c_str.data);
	puts("");
	c_str = static_string_64_n("Hello World", 11);
	printf("static_string_64_t n(str): %s", c_str.data);
	puts("");
}


static void UnorderedEraseSafeVectorProxyTest() noexcept
{
	TEST_NAME("UnorderedEraseSafeVectorProxy<>");
	com::UnorderedEraseSafeVectorProxy<u32> values { { 1, 2, 3, 4, 5, 6, 7, 8, 9 } };
	std::cout << "UnorderedEraseSafeVectorProxy values: "; 
	for(u32 value : values)
		std::cout << value << ", ";
	std::cout << std::endl;

	std::cout << "Erase at 4 while being at 2: ";
	values.traverse([&values](u32& value) noexcept
	{
		std::cout << value << " ";
		static std::size_t counter = 0;
		if(counter == 2)
		{
			values.erase(values.indexToIterator(counter + 2));
		}
		++counter;
	});
	std::cout << std::endl;
	std::cout << "Should Match: 1 2 3 4 9 6 7 8" << std::endl;
	std::cout << "Erase at 2 while being at 2 (print while traverse): ";
	values.traverse([&values](u32& value) noexcept
	{
		std::cout << value << " ";
		static std::size_t counter = 0;
		if(counter == 2)
		{
			values.erase(values.indexToIterator(counter));
		}
		++counter;
	});
	std::cout << std::endl;
	std::cout << "Should Match (print while traverse): 1 2 3 8 4 9 6 7" << std::endl;
	values.traverse([&values](u32& value) noexcept
	{
		std::cout << value << " ";
	});
	std::cout << std::endl;
	std::cout << "Should Match (print after traverse): 1 2 8 4 9 6 7" << std::endl;
	std::cout << "Erase at 0 while being at 5 (print while traverse): ";
	values.traverse([&values](const u32& value) noexcept
	{
		std::cout << value << " ";
		static std::size_t counter = 0;
		if(counter == 5)
		{
			values.erase(values.indexToIterator(0));
		}
		++counter;
	});
	std::cout << std::endl;
	std::cout << "Should Match (print while traverse): 1 2 8 4 9 6 7" << std::endl;
	values.traverse([&values](u32& value) noexcept
	{
		std::cout << value << " ";
	});
	std::cout << std::endl;
	std::cout << "Should Match (print after traverse): 2 8 4 9 6 7" << std::endl;
}

static void floatVerifyTest() noexcept
{
	TEST_NAME("Float verify test");
	CHECK(com_verify_float_str("3.4f"));
	CHECK(com_verify_float_str("3.4e+100f"));
	CHECK(com_verify_float_str("3.4e-100f"));
	CHECK(com_verify_float_str("3.444f"));
	CHECK(com_verify_float_str("3.4e+100f"));
	CHECK(com_verify_float_str("3434.434e-100f"));
	CHECK(com_verify_float_str("+3.4f"));
	CHECK(com_verify_float_str("+3.4e+100f"));
	CHECK(com_verify_float_str("+3.4e-100f"));
	CHECK(com_verify_float_str("+3.444f"));
	CHECK(com_verify_float_str("+3.4e+100f"));
	CHECK(com_verify_float_str("+3434.434e-100f"));
	CHECK(com_verify_float_str("-3.4f"));
	CHECK(com_verify_float_str("-3.4e+100f"));
	CHECK(com_verify_float_str("-3.4e-100f"));
	CHECK(com_verify_float_str("-3.444f"));
	CHECK(com_verify_float_str("-3.4e+100f"));
	CHECK(com_verify_float_str("-3434.434e-100f"));

	CHECK(!com_verify_float_str("e3.4f"));
	CHECK(!com_verify_float_str("f3.4e+100f"));
	CHECK(!com_verify_float_str("3.f4e-100f"));
	CHECK(!com_verify_float_str("3.#444f"));
	CHECK(!com_verify_float_str("3$.4e+100f"));
	CHECK(!com_verify_float_str("3 434.434e-100f"));
	CHECK(!com_verify_float_str("++3.4f"));
	CHECK(!com_verify_float_str("+-3.4e+100f"));
	CHECK(!com_verify_float_str("+3.+4e-100f"));
	CHECK(!com_verify_float_str("+3.er444f"));
	CHECK(!com_verify_float_str("+3.f4e+100f"));
	CHECK(!com_verify_float_str("+3_434.434e-100f"));
	CHECK(!com_verify_float_str("--+3.4f"));
	CHECK(!com_verify_float_str("-+3.+4e+100f"));
	CHECK(!com_verify_float_str("-3.+4e-100f"));
	CHECK(!com_verify_float_str("asdf-3.444f"));
	CHECK(!com_verify_float_str("bvcc+3.4e+100f"));
	CHECK(!com_verify_float_str("-3434.4f34e-100f"));
}

int main(int argc, char** argv)
{
	debug_log_info("Common Repository!");

	static_string_test();
	UnorderedEraseSafeVectorProxyTest();
	floatVerifyTest();

	return 0;
}