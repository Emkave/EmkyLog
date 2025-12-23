#ifndef EMKYLOG_H
#define EMKYLOG_H
#include <format>
#include <chrono>
#include <thread>
#include <system_error>
#include <type_traits>
#include <print>
#include <tuple>
#include <utility>
#include <optional>
#include <charconv>
#include <string_view>
#include <mutex>
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
    static std::recursive_mutex mtx;

public:
    struct control_s {
        std::optional<std::string> string;
        std::optional<bool> newline;
    };

    emkylog() = default;

    static unsigned short init();
    static unsigned short Init();
    static unsigned short set_control(control_s) noexcept;
    static unsigned short SetControl(const control_s &) noexcept;
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
    static control_s get_control() noexcept;
    static control_s GetControl() noexcept;
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
    static unsigned short log(std::string_view, const emkylog::control_s & =emkylog::control);
    template<typename...Args>  static unsigned short log(Args&&...);
    static unsigned short Log(std::string_view, const emkylog::control_s & =emkylog::control);
    template<typename...Args> static unsigned short Log(Args&&...args);
    static unsigned short log_error(std::string_view, const emkylog::control_s & =emkylog::control);
    template<typename...Args> static unsigned short log_error(Args&&...args);
    static unsigned short LogError(std::string_view, const emkylog::control_s & =emkylog::control);
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
        control_s ctrl;

        static unsigned short flush(const level lvl, const std::string_view str, const emkylog::control_s & ctrl) {
            return (lvl == level::info) ? emkylog::log(str, ctrl) : emkylog::log_error(str, ctrl);
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
        explicit line(const level lvl, emkylog::control_s ctrl=emkylog::default_control(), const bool auto_flush=true) : lvl(lvl), auto_flush(auto_flush), ctrl(std::move(ctrl)) {}

        ~line() noexcept {
            if (!this->auto_flush) {
                return;
            }
            std::lock_guard lock (emkylog::mtx);
            (void)flush(this->lvl, this->string, this->ctrl);
        }

        [[nodiscard]] unsigned short flush_now() const {
            std::lock_guard lock (emkylog::mtx);
            return flush(this->lvl, this->string, this->ctrl);
        }

        line & operator << (const std::string_view s) {this->string.append(s); return *this;}
        line & operator << (const char ch) {this->string.push_back(ch); return *this;}
        line & operator << (const char * s) {return *this << std::string_view(s);}
        line & operator << (const bool b) {this->string += (b ? "true" : "false"); return *this;}
        line & operator << (const emkylog::control_s & ctrl) {this->ctrl = ctrl; return *this;}

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

    static control_s control;
    static control_s default_control() {return {{}, std::nullopt};}
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
inline std::recursive_mutex emkylog::mtx;
inline emkylog::control_s emkylog::control{std::nullopt, std::nullopt};

inline unsigned short emkylog::Init() {return emkylog::init();}
inline unsigned short emkylog::SetControl(const emkylog::control_s & control) noexcept {return emkylog::set_control(control);}
inline unsigned short emkylog::SetLogPath(const std::string_view path) {return emkylog::set_log_path(path);}
inline unsigned short emkylog::SetErrorLogPath(const std::string_view path) {return emkylog::set_error_log_path(path);}
inline unsigned short emkylog::SetLogFilename(const std::string_view filename) noexcept {return emkylog::set_log_filename(filename);}
inline unsigned short emkylog::SetErrorLogFilename(const std::string_view filename) noexcept {return emkylog::set_error_log_filename(filename);}
inline void emkylog::SetAutoNewLine(bool && boolean) noexcept {return emkylog::set_auto_new_line(static_cast<bool&&>(boolean));}
inline emkylog::control_s emkylog::GetControl() noexcept {return emkylog::get_control();}
inline std::string_view emkylog::GetLogPath() noexcept {return emkylog::get_log_path();}
inline std::string_view emkylog::GetErrorLogPath() noexcept {return emkylog::get_error_log_path();}
inline std::string_view emkylog::GetLogFilename() noexcept {return emkylog::get_log_filename();}
inline std::string_view emkylog::GetErrorLogFilename() noexcept {return emkylog::get_error_log_filename();}
inline bool emkylog::GetAutoNewLine() noexcept {return emkylog::get_auto_new_line();}
inline unsigned short emkylog::Log(const std::string_view log, const emkylog::control_s & ctrl) {return emkylog::log(log, ctrl);}
inline unsigned short emkylog::LogError(const std::string_view log, const emkylog::control_s & ctrl) {return emkylog::log_error(log, ctrl);}
inline unsigned short emkylog::OpenLogger() {return emkylog::open_logger();}
inline unsigned short emkylog::CloseLogger() {return emkylog::close_logger();}
inline bool emkylog::Initiated() noexcept {return emkylog::initiated();}
template <typename... Args> unsigned short emkylog::LogError(Args &&... args) {return emkylog::log_error(std::forward<Args>(args)...);}
template <typename... Args> unsigned short emkylog::Log(Args &&... args) {return emkylog::log(std::forward<Args>(args)...);}


inline unsigned short emkylog::init() {
    std::lock_guard lock (emkylog::mtx);
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


inline unsigned short emkylog::set_control(control_s control) noexcept {
    std::lock_guard lock (emkylog::mtx);
    emkylog::control = std::move(control);
    return 0;
}



inline unsigned short emkylog::set_log_path(const std::string_view path) {
    std::lock_guard lock (emkylog::mtx);
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
    std::lock_guard lock (emkylog::mtx);
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
    std::lock_guard lock (emkylog::mtx);
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
    std::lock_guard lock (emkylog::mtx);
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
    std::lock_guard lock (emkylog::mtx);
    emkylog::control.newline = boolean;
}


inline emkylog::control_s emkylog::get_control() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::control;
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


inline bool emkylog::get_auto_new_line() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::control.newline.value_or(false);
}


inline unsigned short emkylog::log(const std::string_view slog, const emkylog::control_s & ctrl) {
    std::lock_guard lock (emkylog::mtx);
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

    emkylog::log_stream << slog; // TODO: add here control filters
    emkylog::log_stream << (ctrl.newline.value_or(false) ? "\n" : "");
    emkylog::log_stream.flush();

    return 0;
}


template <typename... Args> unsigned short emkylog::log(Args &&... args) {
    std::lock_guard lock (emkylog::mtx);
    using last_t = std::remove_cvref_t<emkylog::control_type_t<Args...>>;

    if constexpr (std::is_same_v<last_t, emkylog::control_s>) {
        auto tuple = std::forward_as_tuple(std::forward<Args>(args)...);
        constexpr size_t N = sizeof...(Args);
        static_assert(N >= 1);
        emkylog::control_s ctrl = std::get<N-1>(tuple);
        line l(level::info, std::move(ctrl), false);
        emkylog::stream_prefix(l, tuple, std::make_index_sequence<N-1>{});
        return l.flush_now();
    } else {
        line l(level::info, emkylog::default_control(), false);
        (l << ... << std::forward<Args>(args));
        return l.flush_now();
    }
}


inline unsigned short emkylog::log_error(const std::string_view slog, const control_s & ctrl) {
    std::lock_guard lock (emkylog::mtx);
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

    emkylog::error_log_stream << slog; // TODO: add here control filters
    emkylog::log_stream << (ctrl.newline.value_or(false) ? "\n" : "");
    emkylog::error_log_stream.flush();

    return 0;
}


template <typename... Args> unsigned short emkylog::log_error(Args &&...args) {
    std::lock_guard lock (emkylog::mtx);
    using last_t = std::remove_cvref_t<emkylog::control_type_t<Args...>>;

    if constexpr (std::is_same_v<last_t, emkylog::control_s>) {
        auto tuple = std::forward_as_tuple(std::forward<Args>(args)...);
        constexpr size_t N = sizeof...(Args);
        static_assert(N >= 1);
        emkylog::control_s ctrl = std::get<N-1>(tuple);
        line l(level::error, std::move(ctrl), false);
        emkylog::stream_prefix(l, tuple, std::make_index_sequence<N-1>{});
        return l.flush_now();
    } else {
        line l(level::error, emkylog::default_control(), false);
        (l << ... << std::forward<Args>(args));
        return l.flush_now();
    }
}


inline unsigned short emkylog::open_logger() {
    std::lock_guard lock (emkylog::mtx);
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
    std::lock_guard lock (emkylog::mtx);
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
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::log_stream.is_open()) {
        return 11111;
    }

    emkylog::log_stream.close();
    return 0;
}


inline unsigned short emkylog::close_error_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::error_log_stream.is_open()) {
        return 11111;
    }

    emkylog::error_log_stream.close();
    return 0;
}


inline bool emkylog::initiated() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::inited;
}


template<typename Tuple, size_t... Is> inline void emkylog::stream_prefix(line & l, Tuple && t, std::index_sequence<Is...>) {
    (l << ... << std::get<Is>(std::forward<Tuple>(t)));
}



#endif //EMKYLOG_H