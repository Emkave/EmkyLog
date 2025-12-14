#ifndef EMKYLOG_H
#define EMKYLOG_H
#include <format>
#include <chrono>
#include <thread>
#include <print>
#include <expected>
#include <filesystem>
#include <string>
#include <fstream>


class emkylog {
    static std::string log_path;
    static std::string error_log_path;
    static std::string log_filename;
    static std::string error_log_filename;
    static std::ofstream log_stream;
    static std::ofstream error_log_stream;
    static bool inited;

public:
    emkylog() = default;

    static unsigned short init();
    static unsigned short Init();
    static unsigned short set_log_path(std::string_view);
    static unsigned short SetLogPath(std::string_view);
    static unsigned short set_error_log_path(std::string_view);
    static unsigned short SetErrorLogPath(std::string_view);
    static unsigned short set_log_filename(std::string_view) noexcept;
    static unsigned short SetLogFilename(std::string_view) noexcept;
    static unsigned short set_error_log_filename(std::string_view) noexcept;
    static unsigned short SetErrorLogFilename(std::string_view) noexcept;
    static std::string_view get_log_path() noexcept;
    static std::string_view GetLogPath() noexcept;
    static std::string_view get_error_log_path() noexcept;
    static std::string_view GetErrorLogPath() noexcept;
    static std::string_view get_log_filename() noexcept;
    static std::string_view GetLogFilename() noexcept;
    static std::string_view get_error_log_filename() noexcept;
    static std::string_view GetErrorLogFilename() noexcept;
    static unsigned short log(std::string_view);
    static unsigned short Log(std::string_view);
    static unsigned short log_error(std::string_view);
    static unsigned short LogError(std::string_view);
    static unsigned short open_logger();
    static unsigned short OpenLogger();
    static unsigned short open_error_logger();
    static unsigned short OpenErrorLogger();
    static unsigned short close_logger();
    static unsigned short CloseLogger();
    static unsigned short close_error_logger();
    static unsigned short CloseErrorLogger();
    static bool initiated() noexcept;
    static bool Initiated() noexcept;

private:
    enum class level {
        info, error
    };

    class line {
        std::string string;
        level lvl;

        static unsigned short flush(const level lvl, const std::string_view str) {
            return (lvl == level::info) ? emkylog::log(str) : emkylog::log_error(str);
        }

    public:
        explicit line(const level lvl) : lvl(lvl) {}
        ~line() noexcept {
            (void)flush(this->lvl, this->string);
        }

        line & operator << (const std::string_view s) {
            this->string.append(s);
            return *this;
        }

        line & operator << (const char * s) {
            return *this << std::string_view{s};
        }
    };

    struct stream {
        level lvl;
        line operator << (const std::string_view s) const {
            line l{lvl};
            l << s;
            return l;
        }

        line operator << (const char * s) const {
            return *this << std::string_view(s);
        }

        template <typename T> line operator << (T && v) const {
            line l(lvl);
            l << std::forward<T>(v);
            return l;
        }
    };

public:
    static constexpr stream loginfo{level::info};
    static constexpr stream logerror{level::error};
};

inline std::string emkylog::log_path = ".";
inline std::string emkylog::error_log_path = ".";
inline std::string emkylog::log_filename = "emkylog.txt";
inline std::string emkylog::error_log_filename = "emkyerrlog.txt";
inline std::ofstream emkylog::log_stream = {};
inline std::ofstream emkylog::error_log_stream = {};
inline bool emkylog::inited = false;


inline unsigned short emkylog::init() {
    unsigned short res = 00000;
    res |= emkylog::set_log_path(emkylog::log_path);
    res |= emkylog::set_error_log_path(emkylog::error_log_path);
    res |= emkylog::set_log_filename(emkylog::log_filename);
    res |= emkylog::set_error_log_filename(emkylog::error_log_filename);

    if (res) {
        return res;
    }
    emkylog::inited = true;

    return 0;
}


inline unsigned short emkylog::set_log_path(const std::string_view path) {
    if (emkylog::log_stream.is_open()) {
        return 1111;
    }

    std::error_code ec;
    std::filesystem::create_directories(path, ec);

    if (ec) {
        return 11111;
    }

    emkylog::log_path = path;

    return 0;
}


inline unsigned short emkylog::set_error_log_path(const std::string_view path) {
    if (emkylog::error_log_stream.is_open()) {
        return 11111;
    }

    std::error_code ec;
    std::filesystem::create_directories(path, ec);

    if (ec) {
        return 11111;
    }

    emkylog::error_log_path = path;

    return 0;
}


inline unsigned short emkylog::set_log_filename(const std::string_view filename) noexcept {
    if (emkylog::log_stream.is_open()) {
        return 11111;
    }

    if (filename.empty()) {
        return 11111;
    }

    {if (const std::ofstream check (std::filesystem::path(emkylog::log_path) / filename, std::ios::app); !check) {
        return 11111;
    }}
    emkylog::log_filename = filename;

    return 0;
}


inline unsigned short emkylog::set_error_log_filename(const std::string_view filename) noexcept {
    if (emkylog::error_log_stream.is_open()) {
        return 11111;
    }

    if (filename.empty()) {
        return 11111;
    }

    {if (const std::ofstream check (std::filesystem::path(emkylog::error_log_path) / filename, std::ios::app); !check) {
        return 11111;
    }}
    emkylog::error_log_filename = filename;

    return 0;
}


inline std::string_view emkylog::get_log_path() noexcept {
    return emkylog::log_path;
}


inline std::string_view emkylog::get_error_log_path() noexcept {
    return emkylog::error_log_path;
}


inline std::string_view emkylog::get_log_filename() noexcept {
    return emkylog::log_filename;
}


inline std::string_view emkylog::get_error_log_filename() noexcept {
    return emkylog::error_log_filename;
}


inline unsigned short emkylog::log(const std::string_view slog) {
    if (!emkylog::initiated()) {
        if (const unsigned short res = emkylog::init()) {
            return res;
        }
    }

    if (!emkylog::log_stream.is_open()) {
        emkylog::log_stream.open(std::filesystem::path(emkylog::log_path) / emkylog::log_filename, std::ios::app);

        if (!emkylog::log_stream.is_open()) {
            return 11111;
        }
    }

    emkylog::log_stream << slog << '\n';
    emkylog::log_stream.flush();

    return 0;
}


inline unsigned short emkylog::log_error(const std::string_view slog) {
    if (!emkylog::initiated()) {
        if (const unsigned short res = emkylog::init()) {
            return res;
        }
    }

    if (!emkylog::error_log_stream.is_open()) {
        emkylog::error_log_stream.open(std::filesystem::path(emkylog::error_log_path) / emkylog::error_log_filename, std::ios::app);

        if (!emkylog::error_log_stream.is_open()) {
            return 11111;
        }
    }

    emkylog::error_log_stream << slog << '\n';
    emkylog::error_log_stream.flush();

    return 0;
}


inline unsigned short emkylog::open_logger() {
    if (!emkylog::initiated()) {
        if (const unsigned short res = emkylog::init()) {
            return res;
        }
    }

    if (emkylog::log_stream.is_open()) {
        return 0;
    }

    emkylog::log_stream.open(std::filesystem::path(emkylog::log_path) / emkylog::log_filename, std::ios::app);

    if (!emkylog::log_stream.is_open()) {
        return 11111;
    }
    return 0;
}


inline unsigned short emkylog::open_error_logger() {
    if (!emkylog::initiated()) {
        if (const unsigned short res = emkylog::init()) {
            return res;
        }
    }

    if (emkylog::error_log_stream.is_open()) {
        return 0;
    }

    emkylog::error_log_stream.open(std::filesystem::path(emkylog::error_log_path) / emkylog::error_log_filename, std::ios::app);

    if (!emkylog::error_log_stream.is_open()) {
        return 11111;
    }
    return 0;
}


inline unsigned short emkylog::close_logger() {
    if (!emkylog::log_stream.is_open()) {
        return 11111;
    }

    emkylog::log_stream.close();
    return 0;
}


inline unsigned short emkylog::close_error_logger() {
    if (!emkylog::error_log_stream.is_open()) {
        return 11111;
    }

    emkylog::error_log_stream.close();
    return 0;
}


inline bool emkylog::initiated() noexcept {
    return emkylog::inited;
}



#endif //EMKYLOG_H
