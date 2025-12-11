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
    static unsigned short set_log_path(std::string_view);
    static unsigned short set_error_log_path(std::string_view);
    static unsigned short set_log_filename(std::string_view) noexcept;
    static unsigned short set_error_log_filename(std::string_view) noexcept;
    static std::string_view get_log_path() noexcept;
    static std::string_view get_error_log_path() noexcept;
    static std::string_view get_log_filename() noexcept;
    static std::string_view get_error_log_filename() noexcept;
    static unsigned short log(std::string_view);
    static unsigned short log_error(std::string_view);
    static unsigned short open_logger();
    static unsigned short open_error_logger();
    static unsigned short close_logger();
    static unsigned short close_error_logger();
    static bool initiated() noexcept;
};

inline std::string emkylog::log_path = (std::filesystem::current_path() / "emkylog").string();
inline std::string emkylog::error_log_path = (std::filesystem::current_path() / "emkylog").string();
inline std::string emkylog::log_filename = "emkylog.txt";
inline std::string emkylog::error_log_filename = "emkyerrlog.txt";
inline std::ofstream emkylog::log_stream;
inline std::ofstream emkylog::error_log_stream;
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
