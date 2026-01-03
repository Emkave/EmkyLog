# EmkyLog
# Single-Header C++20+ Syncrhonized Modern Cross-Platform Multicompiler-Supported Logger

`emkylog` is a small, header-only logging utility that writes to two files:
- **info log** (`emkylog.txt`)
- **error log** (`emkyerrlog.txt`)

---

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

---

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

---

## Instruction Set

```cpp
static error_code emkylog::init();
static error_code emkylog::Init(); 
```
Initialize logger paths/filenames and mark logger as initiated.
* This calls the setters with current static defaults:
* - log path
* - error log path
* - log filename
* - error log filename

Returns error_code::NO_ERROR on success, otherwise an error code.
Note: Logging calls auto-initialize if not initiated.

---

```cpp
static error_code set_settings(settings_s) noexcept;
static error_code SetSettings(const settings_s &) noexcept;
```
Set all logger settings atomically.
Takes settings_s as input argument.
Returns error_code::NO_ERROR on success.
---

```cpp
static error_code set_log_path(std::string_view);
static error_code SetLogPath(std::string_view);
```
Set directory path for info log output.
Takes string resembling complete path as an argument.
Returns error_code::NO_ERROR on success, otherwise an error code.

Note: Fails if the info log file stream is already open.
Attempts to create the directory if needed.

---

```cpp
static error_code set_error_log_path(std::string_view);
static error_code SetErrorLogPath(std::string_view);
```
Set directory path for error log output.
Takes string resembling complete path as an argument.
Returns error_code::NO_ERROR on success, otherwise an error code.

Note: fails if the error log file stream is already open.
Attempts to create the directory if needed.

---

```cpp
static error_code set_log_filename(std::string_view) noexcept;
static error_code SetLogFilename(std::string_view) noexcept;
```
Set filename for info log file (no path).
Takes string resembling filename (no directories).
Returns error_code::NO_ERROR on success.
Fails if the info log file stream is already open.

---

```cpp
static error_code set_error_log_filename(std::string_view) noexcept;
static error_code SetErrorLogFilename(std::string_view) noexcept;
```
Set filename for error log file (no path).
Takes string resembling filename (no directories).
Returns error_code::NO_ERROR on success.
Fails if the error log file stream is already open.

---

```cpp
static void set_auto_new_line_setting(bool &&) noexcept;
static void SetAutoNewLineSetting(bool &&) noexcept;
```
Enable/disable automatic newline behavior.
Takes boolean rvalue forwarded into the setting.

---

```cpp
static void set_auto_thread_id_setting(bool &&) noexcept;
static void SetAutoThreadIDSetting(bool &&) noexcept;
```
Enable/disable automatic thread-id prefixing.
Takes boolean rvalue forwarded into the setting.

---

```cpp
static void set_auto_date_setting(bool &&) noexcept;
static void SetAutoDateSetting(bool &&) noexcept;
```
Enable/disable automatic date prefixing.
Takes boolean rvalue forwarded into the setting.

---

```cpp
static void set_auto_time_setting(bool &&) noexcept;
static void SetAutoTimeSetting(bool &&) noexcept;
```
Enable/disable automatic time prefixing.
Takes boolean rvalue forwarded into the setting.

---

```cpp
static settings_s & get_settings() noexcept;
static settings_s & GetSettings() noexcept;
```
Get current settings.
Returns a reference to the internal settings object.

---

```cpp
static std::string_view get_log_path() noexcept;
static std::string_view GetLogPath() noexcept;
```
Get current info log directory path.
Returns string view of internal path.

---

```cpp
static std::string_view get_error_log_path() noexcept;
static std::string_view GetErrorLogPath() noexcept;
```
Get current error log directory path.
Returns string view of internal storage.

---

```cpp
static std::string_view get_log_filename() noexcept;
static std::string_view GetLogFilename() noexcept;
```
Get current info log filename.
Returns string view of filename.

---

```cpp
static std::string_view get_error_log_filename() noexcept;
static std::string_view GetErrorLogFilename() noexcept;
```
Get current error log filename.
Returns string view of internal filename.

---

```cpp
static bool get_auto_new_line_setting() noexcept;
static bool GetAutoNewLineSetting() noexcept;
```
Get whether auto new line is enabled. 
Returns boolean.

---

```cpp
static bool get_auto_thread_id_setting() noexcept;
static bool GetAutoThreadIDSetting() noexcept;
```
Get whether auto thread-id is enabled.
Returns boolean.

---

```cpp
static bool get_auto_date_setting() noexcept;
static bool GetAutoDateSetting() noexcept;
```
Get whether auto date is enabled.
Returns boolean.

---

```cpp
static bool get_auto_time_setting() noexcept;
static bool GetAutoTimeSetting() noexcept;
```
Get whether auto time is enabled.
Returns boolean.

---

```cpp
static error_code log(std::string_view, mode=mode::none);
static error_code Log(std::string_view, mode=mode::none);
```
Log an info message.
Takes string view and per-call mode flags (optional).
Returns error_code::NO_ERROR on success.

Note: auto-initializes and auto-opens the info log file if needed.

---

```cpp
template<typename...Args> static error_code log(Args&&...);
template<typename...Args> static error_code Log(Args&&...);
```
Log an info message using stream-style variadic arguments.
Takes Args argument types.
Returns error_code::NO_ERROR on success.

Note: if the last argument is a mode, it is interpreted as a per-call formatting flag.
Otherwise, all args are appended to a line and written as info.

---

```cpp
static error_code log_error(std::string_view, mode=mode::none);
static error_code LogError(std::string_view, mode=mode::none);
```
Log an error message.
Takes string view and per-call mode flags (optional).
Returns error_code::NO_ERROR on success.

Note: auto-initializes and auto-opens the error log file if needed

---

```cpp
template<typename...Args> static error_code log_error(Args&&...);
template<typename...Args> static error_code LogError(Args&&...);
```
Log an error message using stream-style variadic arguments.
Takes args values to append.
Returns error_code::NO_ERROR on success.

Note: If the last argument is a emkylog::mode, it is interpreted per-call formatting flag.
Otherwise, all args are appended to a line and written as error.

---

```cpp
static error_code open();
static error_code Open();
```
Opens both info and error log streams.
Returns error_code::NO_ERROR on success.


---

```cpp
static error_code open_logger();
static error_code OpenLogger();
```
Opens only the info log stream.
Returns error_code::NO_ERROR on success.

---

```cpp
static error_code open_error_logger();
static error_code OpenErrorLogger();
```
Opens only the error log stream.
Returns error_code::NO_ERROR on success.

---

```cpp
static error_code close();
static error_code Close();
```
Closes both info and error log streams.
Returns error_code::NO_ERROR on success.

---

```cpp
static error_code close_logger();
static error_code CloseLogger();
```
Close only the info log stream.
Returns error_code::NO_ERROR on success.

---

```cpp
static error_code close_error_logger();
static error_code CloseErrorLogger();
```
Close only the error log stream.
Returns error_code::NO_ERROR on success.

---

```cpp
static bool initiated() noexcept;
static bool Initiated() noexcept;
```
Check whether EmkyLog has been initiated.
Returns true if initiated, otherwise false.

---

```cpp
static constexpr stream loginfo {level::info};
```
Stream-style entry for info logging.
Example: ```emkylog::loginfo << "Hello " << 123 << emkylog::mode::newline;```

---

```cpp
static constexpr stream logerror {level::error};
```
Stream-style entry for error logging.
Example: ```emkylog::logerror << "Oops: " << "failed" << emkylog::mode::newline;```

---

```cpp
template <typename F> static constexpr auto observe(std::string_view, F&&, std::string_view="none");
```
Create an observing wrapper around a callable.
The returned object behaves like the original callable, but it logs:
- enter event
- exit event + duration (ms)
- exception event + duration (ms) + exception message, then rethrows

It takes F - callable to wrap, observer's name for logs, optional message to be shown in logs, otherwise "none" is used.
Returns an observer wrapper callable.

---

```cpp
```



---

```cpp
```



---

```cpp
```



---

```cpp
```



---

```cpp
```



---

```cpp
```


