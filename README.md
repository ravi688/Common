
OS | GCC | Clang
-- | ------------------ | --------------------
Linux | [![GCC-Linux-Debug](https://github.com/ravi688/Common/actions/workflows/debug_unit_test.yml/badge.svg)](https://github.com/ravi688/Common/actions/workflows/debug_unit_test.yml) | [![Clang-Linux-Debug](https://github.com/ravi688/Common/actions/workflows/clang_debug_unit_test.yml/badge.svg)](https://github.com/ravi688/Common/actions/workflows/clang_debug_unit_test.yml)  
Windows | [![GCC-Win-Debug](https://github.com/ravi688/Common/actions/workflows/windows_debug_unit_test.yml/badge.svg)](https://github.com/ravi688/Common/actions/workflows/windows_debug_unit_test.yml) | [![GCC-Win-Debug](https://github.com/ravi688/Common/actions/workflows/clang_windows_debug_unit_test.yml/badge.svg)](https://github.com/ravi688/Common/actions/workflows/clang_windows_debug_unit_test.yml)  

OS | GCC | Clang
-- | ------------------ | --------------------
Linux | [![GCC-Linux-Release](https://github.com/ravi688/Common/actions/workflows/release_unit_test.yml/badge.svg)](https://github.com/ravi688/Common/actions/workflows/release_unit_test.yml) | [![Clang-Linux-Release](https://github.com/ravi688/Common/actions/workflows/clang_release_unit_test.yml/badge.svg)](https://github.com/ravi688/Common/actions/workflows/clang_release_unit_test.yml)  
Windows | [![GCC-Win-Release](https://github.com/ravi688/Common/actions/workflows/windows_release_unit_test.yml/badge.svg)](https://github.com/ravi688/Common/actions/workflows/windows_release_unit_test.yml) | [![GCC-Win-Release](https://github.com/ravi688/Common/actions/workflows/clang_windows_release_unit_test.yml/badge.svg)](https://github.com/ravi688/Common/actions/workflows/clang_windows_release_unit_test.yml)


# Common utility library
This repository contains the reusabe functions, macros and constantes being used across all my projects.

## Building and Installing
```
$ meson setup build
$ meson compile -C build
$ meson install -C build
```
## Building and Running unit tess
```
$ meson wrap install catch2
$ meson setup build
$ meson compile -C build main_test main
$ ./build/main_test
$ ./build/main 
```

## Platform Macros
### Header file: common/platform.h
* `PLATFORM_WINDOWS` is defined when compiling in Windows Environment (OS)
* `PLATFORM_LINUX` is defined when compiling in Linux Environment (OS)
* `PLATFORM_APPLE` is defined when compiling in MacOS Environment (OS)

## Bit Operations
### Header file: common/defines.h
* `BIT64(index)` macro returns a 64-bit unsigned integer with only single bit set at the index passed as `index`
* `BIT32(index)` macro returns a 32-bit unsigned integer with only single bit set at the index passed as `index`
* `BIT16(index)` macro returns a 16-bit unsigned integer with only single bit set at the index passed as `index`
* `BIT8(index)` macro returns a 8-bit unsigned integer with only single bit set at the index passed as `index`
* `BIT_MASK32(count)` macro returns a 32-bit unsigned integer with its right most `count` bits set
* `BIT16_PACK8(v1, v2)` macro returns a 16-bit unsigned integer with its left most 8 bits encode `v1` and the right most 8 bits encode `v2`
* `BIT16_UNPACK8(bits, index)` macro returns a 8-bit unsigned integer corresponding to the 8 bits starting at `index x 8` bit index from the left
* `BIT32_PACK16(v1, v2)` macro returns a 32-bit unsigned integer with its left most 16 bits encode `v1` and the right most 16 bits encode `v2`
* `BIT32_UNPACK16(bits, index)` macro returns a 16-bit unsigned integer corresponding to the 16 bits starting at `index x 16` bit index from the left
* `BIT32_PACK8(v1, v2, v3, v4)` macro returns 32-bit unsigned integer having packed 8-bit values `v1`, `v2`, `v3`, and `v4` sequentially from left to right
* `BIT32_UNPACK8(bits, index)` macro returns 8-bit unsigned integer corresponding to the 8 bits starting at `index x 8` from the left
* `BIT64_PACK32(v1, v2)` macro returns 64-bit unsigned integer having packed 32-bit values `v1`, and `v2` from left to right
* `BIT64_UNPACK32(bits, index)` macro returns 32-bit unsigned integer corresponding to the 32-bits starting at `index x 32` from the left

## Assertions
### Header file: common/assert.h
* `com_assert(condition, description)` prints description and breaks the execution when `condition` is false, and it only works in `GLOBAL_DEBUG` mode
* `com_assert_wrn(condition, description)` issues only a warning when `condition` is false, and it only works in `GLOBAL_DEBUG` mode
* `com_assert_called_once()` when called inside any function, it make sures the function is called only once in the lifetime  of a program otherwise it prints an error and breaks the execution, and it only works in `GLOBAL_DEBUG` mode
* `com_assert_not_implemented()` when called inside any function, it issues an error when the function is called indicating the function is not implemented yet and not suitable for any use, and it only works in `GLOBAL_DEBUG` mode
* `_com_assert(condition)` works same as `com_assert` function, however it doesn't require a description, it prints the condition expression as it is, and it only works in `GLOBAL_DEBUG` mode
* `_com_assert_wrn(condition)` works same as `com_assert_wrn` function, however it doesn't require a description as second argument, it prints the condition expression as it is, and it only works in `GLOBAL_DEBUG` mode

## Binary Reader
### Header file: common/binary_reader.h
Given a string of bytes, either loaded from file residing in disk or already existing in the memory as `BUFFER` (a.k.a `buffer_t`) object, the set of functions in `common/binary_reader.h` header can be used to read formatted data out of those bytes. <br>
Example:
```c
// include <disk_manager/file_reader.h> to use load_binary_from_file() function
BUFFER* data = load_binary_from_file("myFile.bin");
// create binary_reader_t object from the base pointer and the number of bytes it points to
binary_reader_t* reader = binary_reader_create(buf_get_ptr(data), buf_get_element_count(data))
// destroy the binary_reader_t object
binary_reader_destroy(reader)
// destroy the BUFFER object
buf_free(data)
```
