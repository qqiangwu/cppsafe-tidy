# Intro
A clang-tidy plugin for personal checks. These checks are too optionated such that not suitable to be contributed to clang-tidy.

NOTE:

> The plugin must be used with clang-tidy and cannot be used along. And I assume you already knows how to use clang-tidy.

# Usage
## Build
### MacOS + Local LLVM
```bash
# Configuration
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DLOCAL_CLANG=$(brew --prefix llvm@17)

# Build
cmake --build build -j
```

You will get a `libcppsafe-tidy.so` file after build, put it somewhere.

## Test
```cpp
cd tests
bash run-tests.sh
```

## Use
Make sure clang-tidy is installed.

```bash
clang-tidy --checks='cppsafe*' --load xxxx/libcppsafe-tidy.so your-code.cpp
```

# Checks
## cppsafe-unused-return-value
The opposite of [bugprone-unused-return-value](https://clang.llvm.org/extra/clang-tidy/checks/bugprone/unused-return-value.html). Code is almost copied from it.

Check all return values by default with the following exceptions:

+ functions
    + ::sleep
    + ::usleep
    + memcpy
    + memset
    + memmove
    + printf
    + fprintf
    + sprintf
    + snprintf
    + strcpy
    + strncpy
    + signal
    + fputs
    + ::std::copy
    + ::std::copy_if
    + ::std::copy_n
    + ::std::copy_backward
    + ::std::transform
    + erase
    + erase_if
    + emplace_back
    + fetch_add
    + fetch_sub
    + ::fmt::format_to
+ functions returns T&
+ functions named `insert` and `emplace` but returns an `iterator`
+ member functions of std::atomic

You can mark a function as `[[clang::annotate("cppsafe::may_discard")]]` to ignore its return values.

```cpp
struct FileHandle
{
    int Open();
    [[clang::annotate("cppsafe::may_discard")]] int Close();
};

FileHandle f;
f.Open();  // warning
f.Close();  // no warning
```

There are also options to ignore it.

+ IgnoredFunctions: `;` separated names
+ IgnoreFunctionRegex: a regex, functions matched are ignored
+ AllowCastToVoid: default `true`
