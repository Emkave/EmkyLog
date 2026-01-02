# EmkyLog
# Single-Header C++20+ Syncrhonized Modern Cross-Platform Multicompiler-Supported Logger

`emkylog` is a small, header-only logging utility that writes to two files:
- **info log** (`emkylog.txt`)
- **error log** (`emkyerrlog.txt`)

It supports:
- simple `log("text")` / `log_error("text")`
- **variadic** logging: `log("x=", 42, " y=", 3.14)`
- an `operator<<` stream-like interface: `emkylog::loginfo << "Hello " << 123;`
- a basic **control** struct (currently mainly for auto-newline)


---

## Features

- **Header-only** (just include `EMKYLOG_H`)
- **Two separate outputs**: info + error files
- **Thread-safe** via a single mutex `std::recursive_mutex`
- **Auto-init** on first log call (if you don’t call `Init()` manually)
- **Fast numeric formatting** using `std::to_chars` for ints/floats
- **Control object** can be passed as the **last argument** to variadic logging
- **Observers** allow the logger to observe any functions/anonymous functions/methods and log on execution
---

## Requirements

This code requires a conforming C++20 implementation with:
* std::format
* C++20 chrono time zones (std::chrono::zoned_time)
* std::source_location
* std::filesystem
* std::to_chars for integers and floating-point types

## Compilers supported

- x86-64 clang 17.0.1+
- x86-64 clang 17.0.1 (assertions)+
- x86-64 clang (assertions trunk)
- x86-64 gcc 13.1 (assertions)+
- x86-64 gcc 13.1+
- x86-64 icx 2023.0.0+
- armv7-a clang 19.1.0+
- armv8-a clang 17.0.1+
- ARM gcc 13.1.0+
- ARM64 gcc 13.1.0+
- arm64 msvc v19.29 VS16.11+
- x64 msvc v19.29 VS16.11+
- x86 msvc v19.29 VS16.11+
- HPPA gcc 14.2.0+
- LoongArch64 clang 17.0.1+
- loongarch64 gcc 13.0.1+
- MinGW clang 16.0.2
- MinGW gcc 13.1.0+
- mips gcc 13.1.0+
- mips64 gcc 13.1.0+
- mips64 (el) gcc 13.1.0+
- mipsel gcc 13.1.0+
- power gcc 13.1.0+
- power64 gcc 13.1.0+
- power64le gcc 13.1.0+
- RISC-V (32-bits) gcc 13.1.0+
- RISC-V (64-bits) gcc 13.1.0+
- RISC-V rv32gc clang 19.1.0+
- RISC-V rv64gc clang 19.1.0+
- s390 gcc 13.1.0+
- SPARC gcc 13.1.0+
- SPARC LEON gcc 13.1.0+
- SPARC64 gcc 13.1.0+

---

## Files created

By default, the logger uses:

- directory: `<current_working_directory>/emkylog/`
- info log file: `emkylog.txt`
- error log file: `emkyerrlog.txt`

So on first use it will attempt to create the `emkylog` directory.

---

## Quick start

### 1) Include the header

```cpp
#include "emkylog.h"
```

## Instruction Set

```cpp
static error_code emkylog::init();
static error_code emkylog::Init(); 
```