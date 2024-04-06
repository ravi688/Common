# Common utility library
This repository contains the common being used across all my projects.

## Predefined Macros for Platform determination
### Header file: common/platform.h
`PLATFORM_WINDOWS` is defined when compiling in Windows Environment (OS) <br>
`PLATFORM_LINUX` is defined when compiling in Linux Environment (OS) <br>
`PLATFORM_APPLE` is defined when compiling in MacOS Environment (OS) <br>

## Bit Operations
### Header file: common/defines.h
`BIT64(index)` macro returns a 64-bit unsigned integer with only single bit set at the index passed as `index` <br>
`BIT32(index)` macro returns a 32-bit unsigned integer with only single bit set at the index passed as `index` <br>
`BIT16(index)` macro returns a 16-bit unsigned integer with only single bit set at the index passed as `index` <br>
`BIT8(index)` macro returns a 8-bit unsigned integer with only single bit set at the index passed as `index` <br>
`BIT_MASK32(count)` macro returns a 32-bit unsigned integer with its right most `count` bits set <br>
`BIT16_PACK8(v1, v2)` macro returns a 16-bit unsigned integer with its left most 8 bits encode `v1` and the right most 8 bits encode `v2` <br>
`BIT16_UNPACK8(bits, index)` macro returns a 8-bit unsigned integer corresponding to the 8 bits starting at `index x 8` bit index from the left <br>
`BIT32_PACK16(v1, v2)` macro returns a 32-bit unsigned integer with its left most 16 bits encode `v1` and the right most 16 bits encode `v2` <br>
`BIT32_UNPACK16(bits, index)` macro returns a 16-bit unsigned integer corresponding to the 16 bits starting at `index x 16` bit index from the left <br>
`BIT32_PACK8(v1, v2, v3, v4)` macro returns 32-bit unsigned integer having packed 8-bit values `v1`, `v2`, `v3`, and `v4` sequentially from left to right <br>
`BIT32_UNPACK8(bits, index)` macro returns 8-bit unsigned integer corresponding to the 8 bits starting at `index x 8` from the left <br>
`BIT64_PACK32(v1, v2)` macro returns 64-bit unsigned integer having packed 32-bit values `v1`, and `v2` from left to right <br>
`BIT64_UNPACK32(bits, index)` macro returns 32-bit unsigned integer corresponding to the 32-bits starting at `index x 32` from the left
