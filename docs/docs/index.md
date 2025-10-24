# Welcome to CommonLib C++ Utility library

## Introduction
CommonLib is a C++ Utility library used across all of my projects. I've made it public to invite contributions.

## Building and Installing
### Building
```shell
build_master meson setup build --buildtype=debug
build_master meson compile -C build
```
### Installing
```shell
build_master meson install -C build
```
### Using as meson subprojects
Create a commonlib.wrap file in your meson project's subprojects directory. And copy paste the following contents into that:
```
[wrap-git]
url = https://github.com/ravi688/common.git
revision = HEAD
depth = 1

[provide]
common = common_static_dep
```
Now you can add `common` as dependency into your `meson.build` file.

## Docs
### [StringUtiliy.hpp](/StringUtility.md)
