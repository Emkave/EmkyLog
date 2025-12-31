/*
 * EmkyLog
 * Copyright (c) from 2025 - to 18446744073709551615, author: Natik Agaev
 * All rights reserved.
 * 
 * If you find bugs - please, contact IMMEDIATELY!
 */

#ifndef EMKYLOG_H
#define EMKYLOG_H
#include <format>
#include <chrono>
#include <thread>
#include <source_location>
#include <system_error>
#include <functional>
#include <type_traits>
#include <tuple>
#include <utility>
#include <thread>
#include <optional>
#include <chrono>
#include <charconv>
#include <string_view>
#include <mutex>
#include <filesystem>
#include <string>
#include <fstream>
#include <bitset>

/* TODO:
    -- Completely rewrite the appending operator as it seems the compiler mixes the '<' operator.
    -- Add custom log formatting parser for users' preferences of logs' outlook.
    -- Add printing.
    -- Add colorful printing. Why not though? Fancy ^.^.
    -- Add standard C++ operator observers.
    -- Add system observers. Why not go deeper?
    -- Add timers to measure the cpu, user, and system function execution duration.
    -- Add network observers to observer and log upon the receiving or sending network packets.
    -- Enhance the README.md to make it attractive because a book is judged by its cover!
    -- Add more "requires" stuff so the users would be informed by the compiler in advance if they are about to do smth completely weird.
*/

class emkylog {
    enum class error_code {
        NO_ERROR,
        INIT_FAILED,
        FILE_CLOSED,
        FILE_OPENED,
        CANNOT_OPEN_LOG_FILE,
        INVALID_FILENAME,
        FAILED_DIRECTORY_CREATION,
        CANNOT_OPEN_ERROR_LOG_FILE,
        FAILED_FILE_CREATION
    };

    static std::string log_path;
    static std::string error_log_path;
    static std::string log_filename;
    static std::string error_log_filename;
    static std::ofstream log_stream;
    static std::ofstream error_log_stream;
    static bool inited;
    static std::recursive_mutex mtx;

public:
    struct settings_s {
        bool auto_newline = true;
        bool auto_threadid = false;
        bool auto_date = false;
        bool auto_time = false;
    };


    enum class mode : std::uint32_t {
        none,
        newline = 1<<0,
        nonewline = 1<<1,
        threadid = 1<<2,
        date = 1<<4,
        time = 1<<5
    };

    friend constexpr mode operator | (const mode m1, const mode m2) noexcept {
        return static_cast<mode>(static_cast<std::uint32_t>(m1) | static_cast<std::uint32_t>(m2));
    }

    friend constexpr mode operator & (const mode m1, const mode m2) noexcept {
        return static_cast<mode>(static_cast<std::uint32_t>(m1) & static_cast<std::uint32_t>(m2));
    }

    emkylog() = default;

    static error_code init();
    static error_code Init();
    static error_code set_settings(settings_s) noexcept;
    static error_code SetSettings(const settings_s &) noexcept;
    static error_code set_log_path(std::string_view);
    static error_code SetLogPath(std::string_view);
    static error_code set_error_log_path(std::string_view);
    static error_code SetErrorLogPath(std::string_view);
    static error_code set_log_filename(std::string_view) noexcept;
    static error_code SetLogFilename(std::string_view) noexcept;
    static error_code set_error_log_filename(std::string_view) noexcept;
    static error_code SetErrorLogFilename(std::string_view) noexcept;
    static void set_auto_new_line_setting(bool &&) noexcept;
    static void SetAutoNewLineSetting(bool &&) noexcept;
    static void set_auto_thread_id_setting(bool &&) noexcept;
    static void SetAutoThreadIDSetting(bool &&) noexcept;
    static void set_auto_date_setting(bool &&) noexcept;
    static void SetAutoDateSetting(bool &&) noexcept;
    static void set_auto_time_setting(bool &&) noexcept;
    static void SetAutoTimeSetting(bool &&) noexcept;
    static settings_s & get_settings() noexcept;
    static settings_s & GetSettings() noexcept;
    static std::string_view get_log_path() noexcept;
    static std::string_view GetLogPath() noexcept;
    static std::string_view get_error_log_path() noexcept;
    static std::string_view GetErrorLogPath() noexcept;
    static std::string_view get_log_filename() noexcept;
    static std::string_view GetLogFilename() noexcept;
    static std::string_view get_error_log_filename() noexcept;
    static std::string_view GetErrorLogFilename() noexcept;
    static bool get_auto_new_line_setting() noexcept;
    static bool GetAutoNewLineSetting() noexcept;
    static bool get_auto_thread_id_setting() noexcept;
    static bool GetAutoThreadIDSetting() noexcept;
    static bool get_auto_date_setting() noexcept;
    static bool GetAutoDateSetting() noexcept;
    static bool get_auto_time_setting() noexcept;
    static bool GetAutoTimeSetting() noexcept;
    static error_code log(std::string_view, mode=mode::none);
    template<typename...Args> static error_code log(Args&&...);
    static error_code Log(std::string_view, mode=mode::none);
    template<typename...Args> static error_code Log(Args&&...);
    static error_code log_error(std::string_view, mode=mode::none);
    template<typename...Args> static error_code log_error(Args&&...);
    static error_code LogError(std::string_view, mode=mode::none);
    template<typename...Args> static error_code LogError(Args&&...);
    static error_code open();
    static error_code Open();
    static error_code open_logger();
    static error_code OpenLogger();
    static error_code open_error_logger();
    static error_code OpenErrorLogger();
    static error_code close();
    static error_code Close();
    static error_code close_logger();
    static error_code CloseLogger();
    static error_code close_error_logger();
    static error_code CloseErrorLogger();
    static bool initiated() noexcept;
    static bool Initiated() noexcept;

private:
    enum class level {
        info, error
    };

    class line {
        std::string string;
        level lvl;
        bool auto_flush;
        bool suppress_final_newline = false;
        mode mode_;

        static emkylog::error_code flush(const level lvl, const std::string_view str, const emkylog::mode & mode) {
            return (lvl == level::info) ? emkylog::log(str, mode) : emkylog::log_error(str, mode);
        }

        template <typename T> void append_to_chars(T v) {
            char tmp[128];
            auto [ptr, ec] = std::to_chars(tmp, tmp + sizeof(tmp), v);
            if (ec == std::errc{}) {
                this->string.append(tmp, ptr);
            } else {
                this->string += "<to_chars_error>";
            }
        }

    public:
        explicit line(const level lvl, const emkylog::mode mode=emkylog::mode::none, const bool auto_flush=true) : lvl(lvl), auto_flush(auto_flush), mode_(mode) {}
        line(const line &) = delete;
        line & operator = (const line &) = delete;
        line(line && other) noexcept : string(std::move(other.string)), lvl(other.lvl), auto_flush(other.auto_flush), suppress_final_newline(other.suppress_final_newline), mode_(other.mode_) {
            other.auto_flush = false;
        }

        line & operator = (line && other) noexcept {
            if (this != &other) {
                if (this->auto_flush) {
                    (void)flush(this->lvl, this->string, this->mode_);
                }
                string = std::move(other.string);
                this->lvl = other.lvl;
                this->auto_flush = other.auto_flush;
                this->suppress_final_newline = other.suppress_final_newline;
                this->mode_ = other.mode_;

                other.auto_flush = false;
            }
            return *this;
        }


        ~line() noexcept {
            if (!this->auto_flush) {
                return;
            }

            (void)flush(this->lvl, this->string, this->mode_);
        }

        [[nodiscard]] emkylog::error_code flush_now() {
            std::lock_guard lock (emkylog::mtx);
            this->auto_flush = false;
            return flush(this->lvl, this->string, this->mode_);
        }

        line & operator << (const std::string_view s) {this->string.append(s); return *this;}
        line & operator << (const char ch) {this->string.push_back(ch); return *this;}
        line & operator << (const char * s) {return *this << std::string_view(s);}
        line & operator << (const bool b) {this->string += (b ? "true" : "false"); return *this;}
        line & operator << (const emkylog::mode mode) {this->mode_ = mode; return *this;}
        line & operator << (const std::thread::id tid) {return *this << std::hash<std::thread::id>{}(tid);}

        template <typename T> requires (std::is_integral_v<std::remove_reference_t<T>> || std::is_floating_point_v<std::remove_reference_t<T>>)
        line & operator << (T v) {this->append_to_chars(v); return *this;}

    };

    struct stream {
        level lvl;
        template <typename T> line operator << (T && v) const {
            std::lock_guard lock (emkylog::mtx);
            line l(lvl);
            l << std::forward<T>(v);
            return l;
        }
    };

    template<typename... Ts> struct control_type;
    template<typename T> struct control_type<T> {using type = T;};
    template<typename T, typename... Ts> struct control_type<T, Ts...> : control_type<Ts...> {};
    template<typename... Ts> using control_type_t = typename control_type<Ts...>::type;
    template<typename Tuple, size_t... Is> static void stream_prefix(line&, Tuple&&, std::index_sequence<Is...>);

public:
    static constexpr stream loginfo {level::info};
    static constexpr stream logerror {level::error};

private:
    enum phase {enter, exit, exception};

    struct event {
        phase ph;
        std::string_view name;
        std::string_view message;
        std::chrono::milliseconds duration {};
        std::string_view what {};
    };

    template <typename F> class observer {
        std::string_view name_;
        F f_;
        std::string_view message;

        template <typename Self, typename...Args> static decltype(auto) call_impl(Self&&self, Args&&...args);

    public:
        constexpr observer(const std::string_view name, F f, const std::string_view message) : name_(name), f_(std::move(f)), message(message) {}

        template <typename...Args> decltype(auto) operator()(Args&&...args) & {
            return call_impl(*this, std::forward<Args>(args)...);
        }

        template <typename...Args> decltype(auto) operator()(Args&&...args) const & {
            return call_impl(*this, std::forward<Args>(args)...);
        }

        template <typename...Args> decltype(auto) operator()(Args&&...args) && {
            return call_impl(std::move(*this), std::forward<Args>(args)...);
        }
    };

    static void log_event(const event & e);
    static settings_s settings;

public:
    template <typename F> static constexpr auto observe(std::string_view, F&&, std::string_view="none");

};

inline std::string emkylog::log_path = (std::filesystem::current_path() / "emkylog").string();
inline std::string emkylog::error_log_path = (std::filesystem::current_path() / "emkylog").string();
inline std::string emkylog::log_filename = "emkylog.txt";
inline std::string emkylog::error_log_filename = "emkyerrlog.txt";
inline std::ofstream emkylog::log_stream = {};
inline std::ofstream emkylog::error_log_stream = {};
inline bool emkylog::inited = false;
inline std::recursive_mutex emkylog::mtx;
inline emkylog::settings_s emkylog::settings;

inline emkylog::error_code emkylog::Init() {return emkylog::init();}
inline emkylog::error_code emkylog::SetSettings(const emkylog::settings_s & control) noexcept {return emkylog::set_settings(control);}
inline emkylog::error_code emkylog::SetLogPath(const std::string_view path) {return emkylog::set_log_path(path);}
inline emkylog::error_code emkylog::SetErrorLogPath(const std::string_view path) {return emkylog::set_error_log_path(path);}
inline emkylog::error_code emkylog::SetLogFilename(const std::string_view filename) noexcept {return emkylog::set_log_filename(filename);}
inline emkylog::error_code emkylog::SetErrorLogFilename(const std::string_view filename) noexcept {return emkylog::set_error_log_filename(filename);}
inline void emkylog::SetAutoNewLineSetting(bool && boolean) noexcept {return emkylog::set_auto_new_line_setting(static_cast<bool&&>(boolean));}
inline void emkylog::SetAutoDateSetting(bool && boolean) noexcept {return emkylog::set_auto_date_setting(static_cast<bool&&>(boolean));}
inline void emkylog::SetAutoThreadIDSetting(bool && boolean) noexcept {return emkylog::set_auto_thread_id_setting(static_cast<bool&&>(boolean));}
inline void emkylog::SetAutoTimeSetting(bool && boolean) noexcept {return emkylog::set_auto_time_setting(static_cast<bool&&>(boolean));}
inline emkylog::settings_s & emkylog::GetSettings() noexcept {return emkylog::get_settings();}
inline std::string_view emkylog::GetLogPath() noexcept {return emkylog::get_log_path();}
inline std::string_view emkylog::GetErrorLogPath() noexcept {return emkylog::get_error_log_path();}
inline std::string_view emkylog::GetLogFilename() noexcept {return emkylog::get_log_filename();}
inline std::string_view emkylog::GetErrorLogFilename() noexcept {return emkylog::get_error_log_filename();}
inline bool emkylog::GetAutoNewLineSetting() noexcept {return emkylog::get_auto_new_line_setting();}
inline bool emkylog::GetAutoDateSetting() noexcept {return emkylog::get_auto_date_setting();}
inline bool emkylog::GetAutoThreadIDSetting() noexcept {return emkylog::get_auto_thread_id_setting();}
inline bool emkylog::GetAutoTimeSetting() noexcept {return emkylog::get_auto_time_setting();}
inline emkylog::error_code emkylog::Log(const std::string_view log, const emkylog::mode mode) {return emkylog::log(log, mode);}
inline emkylog::error_code emkylog::LogError(const std::string_view log, const emkylog::mode mode) {return emkylog::log_error(log, mode);}
inline emkylog::error_code emkylog::OpenLogger() {return emkylog::open_logger();}
inline emkylog::error_code emkylog::Close() {return emkylog::close();}
inline emkylog::error_code emkylog::CloseLogger() {return emkylog::close_logger();}
inline bool emkylog::Initiated() noexcept {return emkylog::initiated();}
template <typename... Args> emkylog::error_code emkylog::LogError(Args &&... args) {return emkylog::log_error(std::forward<Args>(args)...);}
template <typename... Args> emkylog::error_code emkylog::Log(Args &&... args) {return emkylog::log(std::forward<Args>(args)...);}


inline emkylog::error_code emkylog::init() {
    std::lock_guard lock (emkylog::mtx);

    if (emkylog::set_log_path(emkylog::log_path) != error_code::NO_ERROR || emkylog::set_error_log_path(emkylog::error_log_path) != error_code::NO_ERROR || emkylog::set_log_filename(emkylog::log_filename) != error_code::NO_ERROR || emkylog::set_error_log_filename(emkylog::error_log_filename) != error_code::NO_ERROR) {
        return emkylog::error_code::INIT_FAILED;
    }
    emkylog::inited = true;

    return emkylog::error_code::NO_ERROR;
}


inline emkylog::error_code emkylog::set_settings(const settings_s settings) noexcept {
    std::lock_guard lock (emkylog::mtx);
    emkylog::settings = settings;
    return error_code::NO_ERROR;
}


inline emkylog::error_code emkylog::set_log_path(const std::string_view path) {
    std::lock_guard lock (emkylog::mtx);
    if (emkylog::log_stream.is_open()) {
        return error_code::FILE_OPENED;
    }

    std::error_code ec;
    std::filesystem::create_directories(path, ec);

    if (ec) {
        return error_code::FAILED_DIRECTORY_CREATION;
    }

    emkylog::log_path = path;

    return error_code::NO_ERROR;
}


inline emkylog::error_code emkylog::set_error_log_path(const std::string_view path) {
    std::lock_guard lock (emkylog::mtx);
    if (emkylog::error_log_stream.is_open()) {
        return error_code::FILE_OPENED;
    }

    std::error_code ec;
    std::filesystem::create_directories(path, ec);

    if (ec) {
        return error_code::FAILED_DIRECTORY_CREATION;
    }

    emkylog::error_log_path = path;

    return error_code::NO_ERROR;
}


inline emkylog::error_code emkylog::set_log_filename(const std::string_view filename) noexcept {
    std::lock_guard lock (emkylog::mtx);
    if (emkylog::log_stream.is_open()) {
        return error_code::FILE_OPENED;
    }

    if (filename.empty()) {
        return error_code::INVALID_FILENAME;
    }

    {if (const std::ofstream check (std::filesystem::path(emkylog::log_path) / filename, std::ios::app); !check) {
        return error_code::FAILED_FILE_CREATION;
    }}
    emkylog::log_filename = filename;

    return error_code::NO_ERROR;
}


inline emkylog::error_code emkylog::set_error_log_filename(const std::string_view filename) noexcept {
    std::lock_guard lock (emkylog::mtx);
    if (emkylog::error_log_stream.is_open()) {
        return error_code::FILE_OPENED;
    }

    if (filename.empty()) {
        return error_code::INVALID_FILENAME;
    }

    {if (const std::ofstream check (std::filesystem::path(emkylog::error_log_path) / filename, std::ios::app); !check) {
        return error_code::FAILED_FILE_CREATION;
    }}
    emkylog::error_log_filename = filename;

    return error_code::NO_ERROR;
}


inline void emkylog::set_auto_new_line_setting(bool && boolean) noexcept {
    std::lock_guard lock (emkylog::mtx);
    emkylog::settings.auto_newline = boolean;
}


inline void emkylog::set_auto_thread_id_setting(bool && boolean) noexcept {
    std::lock_guard lock (emkylog::mtx);
    emkylog::settings.auto_threadid = boolean;
}


inline void emkylog::set_auto_date_setting(bool && boolean) noexcept {
    std::lock_guard lock (emkylog::mtx);
    emkylog::settings.auto_date = boolean;
}


inline void emkylog::set_auto_time_setting(bool && boolean) noexcept {
    std::lock_guard lock (emkylog::mtx);
    emkylog::settings.auto_time = boolean;
}


inline emkylog::settings_s & emkylog::get_settings() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::settings;
}


inline std::string_view emkylog::get_log_path() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::log_path;
}


inline std::string_view emkylog::get_error_log_path() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::error_log_path;
}


inline std::string_view emkylog::get_log_filename() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::log_filename;
}


inline std::string_view emkylog::get_error_log_filename() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::error_log_filename;
}


inline bool emkylog::get_auto_new_line_setting() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::settings.auto_newline;
}


inline bool emkylog::get_auto_date_setting() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::settings.auto_date;
}


inline bool emkylog::get_auto_thread_id_setting() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::settings.auto_threadid;
}


inline bool emkylog::get_auto_time_setting() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::settings.auto_time;
}


inline emkylog::error_code emkylog::log(const std::string_view slog, const emkylog::mode mode) {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::initiated()) {
        if (const emkylog::error_code res = emkylog::init(); res != error_code::NO_ERROR) {
            return res;
        }
    }

    if (!emkylog::log_stream.is_open()) {
        emkylog::log_stream.open(std::filesystem::path(emkylog::log_path) / emkylog::log_filename, std::ios::app);

        if (!emkylog::log_stream.is_open()) {
            return emkylog::error_code::FILE_CLOSED;
        }
    }
    const std::underlying_type_t<emkylog::mode> bits = static_cast<std::underlying_type_t<emkylog::mode>>(mode);

    if (bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::date) || emkylog::get_auto_date_setting()) {
        emkylog::log_stream << std::format("{:%Y-%m-%d}", std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())}) << " ";
    }

    if (bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::time) || emkylog::get_auto_time_setting()) {
        emkylog::log_stream << std::format("{:%H:%M:%S}", std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}) << " ";
    }

    if (bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::threadid) || emkylog::get_auto_thread_id_setting()) {
        emkylog::log_stream << "TID: " << std::this_thread::get_id() << " ";
    }

    emkylog::log_stream << slog;

    bool is_newline = emkylog::get_auto_new_line_setting();
    if (bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::nonewline)) {
        is_newline = false;
    }

    if ((bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::newline)) || is_newline) {
        emkylog::log_stream << '\n';
    }
    emkylog::log_stream.flush();
    return error_code::NO_ERROR;
}


template <typename... Args> emkylog::error_code emkylog::log(Args &&... args) {
    std::lock_guard lock (emkylog::mtx);
    using last_t = std::remove_cvref_t<emkylog::control_type_t<Args...>>;

    if constexpr (std::is_same_v<last_t, emkylog::mode>) {
        auto tuple = std::forward_as_tuple(std::forward<Args>(args)...);
        constexpr size_t N = sizeof...(Args);
        static_assert(N >= 1);
        line l(level::info, std::get<N-1>(tuple), false);
        emkylog::stream_prefix(l, tuple, std::make_index_sequence<N-1>{});
        return l.flush_now();
    } else {
        line l(level::info, emkylog::mode::none, false);
        (l << ... << std::forward<Args>(args));
        return l.flush_now();
    }
}


inline emkylog::error_code emkylog::log_error(const std::string_view slog, const emkylog::mode mode) {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::initiated()) {
        if (const emkylog::error_code res = emkylog::init(); res != error_code::NO_ERROR) {
            return res;
        }
    }

    if (!emkylog::error_log_stream.is_open()) {
        emkylog::error_log_stream.open(std::filesystem::path(emkylog::error_log_path) / emkylog::error_log_filename, std::ios::app);

        if (!emkylog::error_log_stream.is_open()) {
            return error_code::FILE_CLOSED;
        }
    }

    const std::underlying_type_t<emkylog::mode> bits = static_cast<std::underlying_type_t<emkylog::mode>>(mode);

    if (bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::date) || emkylog::get_auto_date_setting()) {
        emkylog::error_log_stream << std::format("{:%Y-%m-%d}", std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())}) << " ";
    }

    if (bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::time) || emkylog::get_auto_time_setting()) {
        emkylog::error_log_stream << std::format("{:%H:%M:%S}", std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}) << " ";
    }

    if (bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::threadid) || emkylog::get_auto_thread_id_setting()) {
        emkylog::error_log_stream << "TID: " << std::this_thread::get_id() << " ";
    }

    emkylog::error_log_stream << slog;

    bool is_newline = emkylog::get_auto_new_line_setting();
    if (bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::nonewline)) {
        is_newline = false;
    }

    if ((bits & static_cast<std::underlying_type_t<emkylog::mode>>(mode::newline)) || is_newline) {
        emkylog::error_log_stream << '\n';
    }
    emkylog::error_log_stream.flush();
    return error_code::NO_ERROR;
}


template <typename... Args> emkylog::error_code emkylog::log_error(Args &&...args) {
    std::lock_guard lock (emkylog::mtx);
    using last_t = std::remove_cvref_t<emkylog::control_type_t<Args...>>;

    if constexpr (std::is_same_v<last_t, emkylog::mode>) {
        auto tuple = std::forward_as_tuple(std::forward<Args>(args)...);
        constexpr size_t N = sizeof...(Args);
        static_assert(N >= 1);
        line l(level::error, std::get<N-1>(tuple), false);
        emkylog::stream_prefix(l, tuple, std::make_index_sequence<N-1>{});
        return l.flush_now();
    } else {
        line l(level::error, emkylog::mode::none, false);
        (l << ... << std::forward<Args>(args));
        return l.flush_now();
    }
}


inline emkylog::error_code emkylog::open() {
    if (const emkylog::error_code res = emkylog::open_error_logger(); res != error_code::NO_ERROR) {
        return res;
    }

    return emkylog::open_logger();
}



inline emkylog::error_code emkylog::open_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::initiated()) {
        if (const emkylog::error_code res = emkylog::init(); res != error_code::NO_ERROR) {
            return res;
        }
    }

    if (emkylog::log_stream.is_open()) {
        return error_code::FILE_OPENED;
    }

    emkylog::log_stream.open(std::filesystem::path(emkylog::log_path) / emkylog::log_filename, std::ios::app);

    if (!emkylog::log_stream.is_open()) {
        return error_code::FILE_CLOSED;
    }
    return error_code::NO_ERROR;
}


inline emkylog::error_code emkylog::open_error_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::initiated()) {
        if (const emkylog::error_code res = emkylog::init(); res != error_code::NO_ERROR) {
            return res;
        }
    }

    if (emkylog::error_log_stream.is_open()) {
        return error_code::FILE_OPENED;
    }

    emkylog::error_log_stream.open(std::filesystem::path(emkylog::error_log_path) / emkylog::error_log_filename, std::ios::app);

    if (!emkylog::error_log_stream.is_open()) {
        return error_code::FILE_CLOSED;
    }
    return error_code::NO_ERROR;
}


inline emkylog::error_code emkylog::close() {
    if (const emkylog::error_code res = emkylog::close_error_logger(); res != error_code::NO_ERROR) {
        return res;
    }

    return emkylog::close_logger();
}


inline emkylog::error_code emkylog::close_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::log_stream.is_open()) {
        return error_code::FILE_CLOSED;
    }

    emkylog::log_stream.close();
    return error_code::NO_ERROR;
}


inline emkylog::error_code emkylog::close_error_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::error_log_stream.is_open()) {
        return error_code::FILE_CLOSED;
    }

    emkylog::error_log_stream.close();
    return error_code::NO_ERROR;
}


inline bool emkylog::initiated() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::inited;
}


template<typename Tuple, size_t... Is> inline void emkylog::stream_prefix(line & l, Tuple && t, std::index_sequence<Is...>) {
    (l << ... << std::get<Is>(std::forward<Tuple>(t)));
}


template<typename F> constexpr auto emkylog::observe(const std::string_view name, F && f, std::string_view message) {
    return emkylog::observer<std::decay_t<F>>{name, std::forward<F>(f), message};
}


inline void emkylog::log_event(const event & e) {
    switch (e.ph) {
        case emkylog::enter:
            emkylog::loginfo << "[Observer]: " << e.name << " has" << " entered " << emkylog::mode::nonewline;
            goto noerror;

        case emkylog::exit:
            emkylog::loginfo << "[Observer]: " << e.name << " has" << " exited " << emkylog::mode::nonewline;
            goto noerror;

        case emkylog::exception:
            emkylog::logerror << "[Observer]: " << e.name << " has" << " threw " << emkylog::mode::nonewline;
            emkylog::logerror << " with the message: " << e.message << ". " << e.duration.count() << "ms.\n";
            break;

        default:
            noerror:
            emkylog::loginfo << " with the message: " << e.message<< ". " << e.duration.count() << "ms.\n";
            break;
    }
}


template<typename F> template<typename Self, typename... Args> decltype(auto) emkylog::observer<F>::call_impl(Self && self, Args &&... args) {
    using clock = std::chrono::steady_clock;
    const auto start = clock::now();
    emkylog::log_event(event{
        .ph = phase::enter,
        .name = self.name_,
        .message = self.message,
        .duration = {}
    });

    try {
        using R = std::invoke_result_t<decltype((self.f_)), Args...>;

        if constexpr (std::is_void_v<R>) {
            std::invoke(self.f_, std::forward<Args>(args)...);

            const auto end = clock::now();
            emkylog::log_event(event{
                .ph = phase::exit,
                .name = self.name_,
                .message = self.message,
                .duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            });
        } else {
            R result = std::invoke(self.f_, std::forward<Args>(args)...);

            const auto end = clock::now();
            emkylog::log_event(event{
                .ph = phase::exit,
                .name = self.name_,
                .message = self.message,
                .duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            });

            return result;
        }
    } catch (std::exception & e) {
        const auto end = clock::now();
        emkylog::log_event(event{
            .ph = phase::exception,
            .name = self.name_,
            .message = self.message,
            .duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start),
            .what = e.what()
        });
        throw;
    }
}



#endif //EMKYLOG_H