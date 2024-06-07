
#include <common/debug.h>
#include <common/assert.h>

#include <stdio.h> /* stdio.h */
#include <common/static_string.h>

static void static_string_test()
{
	debug_log_info("[static_string_t]");
	static_string_64_t str = static_string_64("Hello World");
	printf("static_string_64_t str: %s", str);
	puts("");
	printf("static_string_64_t len(str): %u", static_string_64_len(&str));
	puts("");
	static_string_64_t c_str;
	static_string_64_cpy(&c_str, &str);
	printf("static_string_64_t cpy(str): %s", c_str);
	puts("");
}

int main(int argc, char** argv)
{
	debug_log_info("Common Repository!");

	static_string_test();

	return 0;
}