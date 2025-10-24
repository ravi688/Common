# Welcome to CommonLib C++ Utility library

## Introduction

## Building and Installing

## Getting Started
### common/StringUtility.hpp
#### com::string_join()
Joins multiple strings. A string can be `std::string`, `std::string_view`, or `const char*`. Basically any type convertible to `std::string_view` can be joined.  
Example:
```cpp
#include <common/StringUtility.hpp>
// ...
std::string str = com::string_join("Hello ", "Another ", "World");
std::cout << str << std::endl;
// ...
// Will output:
// Hello Another World
```
