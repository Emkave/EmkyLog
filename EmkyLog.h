#ifndef EMKYLOG_H
#define EMKYLOG_H
#include <format>
#include <chrono>
#include <thread>
#include <system_error>
#include <type_traits>
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
    static bool auto_newline;

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
    static void set_auto_new_line(bool &&) noexcept;
    static void SetAutoNewLine(bool &&) noexcept;
    static std::string_view get_log_path() noexcept;
    static std::string_view GetLogPath() noexcept;
    static std::string_view get_error_log_path() noexcept;
    static std::string_view GetErrorLogPath() noexcept;
    static std::string_view get_log_filename() noexcept;
    static std::string_view GetLogFilename() noexcept;
    static std::string_view get_error_log_filename() noexcept;
    static std::string_view GetErrorLogFilename() noexcept;
    static bool get_auto_new_line() noexcept;
    static bool GetAutoNewLine() noexcept;
    static unsigned short log(std::string_view, bool=emkylog::auto_newline);
    template<typename...Args>  static unsigned short log(Args&&...);
    static unsigned short Log(std::string_view, bool=emkylog::auto_newline);
    template<typename...Args> static unsigned short Log(Args&&...args);
    static unsigned short log_error(std::string_view, bool=emkylog::auto_newline);
    template<typename...Args> static unsigned short log_error(Args&&...args);
    static unsigned short LogError(std::string_view, bool=emkylog::auto_newline);
    template<typename...Args> static unsigned short LogError(Args&&...args);
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
        bool auto_flush;

        static unsigned short flush(const level lvl, const std::string_view str, const bool new_line) {
            return (lvl == level::info) ? emkylog::log(str, new_line) : emkylog::log_error(str, new_line);
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
        explicit line(const level lvl, const bool auto_flush=true) : lvl(lvl), auto_flush(auto_flush) {}
        ~line() noexcept {
            if (this->auto_flush) {
                (void)flush(this->lvl, this->string, true);
            }
        }

        [[nodiscard]] unsigned short flush_now(const bool new_line) const {
            return flush(this->lvl, this->string, new_line);
        }

        line & operator << (const std::string_view s) {this->string.append(s); return *this;}
        line & operator << (const char ch) {this->string.push_back(ch); return *this;}
        line & operator << (const char * s) {return *this << std::string_view(s);}
        line & operator << (const bool b) {this->string += (b ? "true" : "false"); return *this;}
        template <typename T> requires (std::is_integral_v<std::remove_reference_t<T>>)
        line & operator << (T v) {
            this->append_to_chars(v);
            return *this;
        }
        template <typename T> requires (std::is_floating_point_v<std::remove_reference_t<T>>)
        line & operator << (T v) {
            this->append_to_chars(v);
            return *this;
        }
    };

    struct stream {
        level lvl;
        template <typename T> line operator << (T && v) const {
            line l(lvl);
            l << std::forward<T>(v);
            return l;
        }
    };

public:
    static constexpr stream loginfo {level::info};
    static constexpr stream logerror {level::error};
};

inline std::string emkylog::log_path = (std::filesystem::current_path() / "emkylog").string();
inline std::string emkylog::error_log_path = (std::filesystem::current_path() / "emkylog").string();
inline std::string emkylog::log_filename = "emkylog.txt";
inline std::string emkylog::error_log_filename = "emkyerrlog.txt";
inline std::ofstream emkylog::log_stream = {};
inline std::ofstream emkylog::error_log_stream = {};
inline bool emkylog::inited = false;
inline bool emkylog::auto_newline = true;


inline unsigned short emkylog::Init() {return emkylog::init();}
inline unsigned short emkylog::SetLogPath(const std::string_view path) {return emkylog::set_log_path(path);}
inline unsigned short emkylog::SetErrorLogPath(const std::string_view path) {return emkylog::set_error_log_path(path);}
inline unsigned short emkylog::SetLogFilename(const std::string_view filename) noexcept {return emkylog::set_log_filename(filename);}
inline unsigned short emkylog::SetErrorLogFilename(const std::string_view filename) noexcept {return emkylog::set_error_log_filename(filename);}
inline void emkylog::SetAutoNewLine(bool && boolean) noexcept {return emkylog::set_auto_new_line(static_cast<bool&&>(boolean));}
inline std::string_view emkylog::GetLogPath() noexcept {return emkylog::get_log_path();}
inline std::string_view emkylog::GetErrorLogPath() noexcept {return emkylog::get_error_log_path();}
inline std::string_view emkylog::GetLogFilename() noexcept {return emkylog::get_log_filename();}
inline std::string_view emkylog::GetErrorLogFilename() noexcept {return emkylog::get_error_log_filename();}
inline bool emkylog::GetAutoNewLine() noexcept {return emkylog::get_auto_new_line();}
inline unsigned short emkylog::Log(const std::string_view log, const bool new_line) {return emkylog::log(log, new_line);}
inline unsigned short emkylog::LogError(const std::string_view log, const bool new_line) {return emkylog::log_error(log, new_line);}
inline unsigned short emkylog::OpenLogger() {return emkylog::open_logger();}
inline unsigned short emkylog::CloseLogger() {return emkylog::close_logger();}
inline bool emkylog::Initiated() noexcept {return emkylog::initiated();}


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


inline void emkylog::set_auto_new_line(bool && boolean) noexcept {
    emkylog::auto_newline = boolean;
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


inline bool emkylog::get_auto_new_line() noexcept {
    return emkylog::auto_newline;
}


inline unsigned short emkylog::log(const std::string_view slog, const bool new_line) {
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

    emkylog::log_stream << slog << (new_line ? "\n" : "");
    emkylog::log_stream.flush();

    return 0;
}


template <typename... Args> unsigned short emkylog::log(Args &&... args) {
    line l(level::info, false);
    (l << ... << std::forward<Args>(args));
    return l.flush_now(emkylog::auto_newline);
}



template <typename... Args> unsigned short emkylog::Log(Args &&... args) {
    line l(level::info, false);
    (l << ... << std::forward<Args>(args));
    return l.flush_now(emkylog::auto_newline);
}


inline unsigned short emkylog::log_error(const std::string_view slog, const bool new_line) {
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

    emkylog::error_log_stream << slog << (new_line ? "\n" : "");
    emkylog::error_log_stream.flush();

    return 0;
}


template <typename... Args> unsigned short emkylog::log_error(Args &&...args) {
    line l(level::error, false);
    (l << ... << std::forward<Args>(args));
    return l.flush_now(emkylog::auto_newline);
}


template <typename... Args> unsigned short emkylog::LogError(Args &&... args) {
    line l(level::error, false);
    (l << ... << std::forward<Args>(args));
    return l.flush_now(emkylog::auto_newline);
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