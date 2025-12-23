# EmkyLog
# Single-Header C++23 Syncrhonized Modern Cross-Platform Multicompiler-Supported Logger

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
- **Thread-safe** via a single global `std::recursive_mutex`
- **Auto-init** on first log call (if you don’t call `Init()` manually)
- **Fast numeric formatting** using `std::to_chars` for ints/floats
- **Control object** can be passed as the **last argument** to variadic logging

---

## Requirements

- C++23+

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
