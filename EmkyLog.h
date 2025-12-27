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


class emkylog {
    enum error_code {
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
    struct control_s {
        std::optional<std::string> regex;
        std::optional<bool> newline;
    };

    emkylog() = default;

    static error_code init();
    static error_code Init();
    static error_code set_control(control_s) noexcept;
    static error_code SetControl(const control_s &) noexcept;
    static error_code set_log_path(std::string_view);
    static error_code SetLogPath(std::string_view);
    static error_code set_error_log_path(std::string_view);
    static error_code SetErrorLogPath(std::string_view);
    static error_code set_log_filename(std::string_view) noexcept;
    static error_code SetLogFilename(std::string_view) noexcept;
    static error_code set_error_log_filename(std::string_view) noexcept;
    static error_code SetErrorLogFilename(std::string_view) noexcept;
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
    static emkylog::error_code log(std::string_view, const emkylog::control_s & =emkylog::control);
    template<typename...Args> static error_code log(Args&&...);
    static emkylog::error_code Log(std::string_view, const emkylog::control_s & =emkylog::control);
    template<typename...Args> static error_code Log(Args&&...);
    static emkylog::error_code log_error(std::string_view, const emkylog::control_s & =emkylog::control);
    template<typename...Args> static error_code log_error(Args&&...);
    static emkylog::error_code LogError(std::string_view, const emkylog::control_s & =emkylog::control);
    template<typename...Args> static error_code LogError(Args&&...);
    static error_code open_logger();
    static error_code OpenLogger();
    static error_code open_error_logger();
    static error_code OpenErrorLogger();
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
        control_s ctrl;

        static emkylog::error_code flush(const level lvl, const std::string_view str, const emkylog::control_s & ctrl) {
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

            this->ctrl.newline = !this->suppress_final_newline;

            (void)flush(this->lvl, this->string, this->ctrl);
        }

        [[nodiscard]] emkylog::error_code flush_now() const {
            std::lock_guard lock (emkylog::mtx);
            return flush(this->lvl, this->string, this->ctrl);
        }

        line & operator << (const std::string_view s) {this->string.append(s); return *this;}
        line & operator << (const char ch) {this->string.push_back(ch); return *this;}
        line & operator << (const char * s) {return *this << std::string_view(s);}
        line & operator << (const bool b) {this->string += (b ? "true" : "false"); return *this;}
        line & operator << (const emkylog::control_s & ctrl) {this->ctrl = ctrl; return *this;}
        line & operator << (const std::thread::id tid) {return *this << std::hash<std::thread::id>{}(tid);}

        template <typename T> requires (std::is_integral_v<std::remove_reference_t<T>> || std::is_floating_point_v<std::remove_reference_t<T>>)
        line & operator << (T v) {this->append_to_chars(v); return *this;}

        line & operator < (const std::string_view s) {this->suppress_final_newline = true; return *this << s;}
        line & operator < (const char ch) {this->suppress_final_newline = true; return *this << ch;}
        line & operator < (const char * s) {this->suppress_final_newline = true; return *this << s;}
        line & operator < (const bool b) {this->suppress_final_newline = true; return *this << b;}
        line & operator < (const std::thread::id tid) {this->suppress_final_newline = true; return *this << tid;}

        template <typename T> requires (std::is_integral_v<std::remove_reference_t<T>> || std::is_floating_point_v<std::remove_reference_t<T>>)
        line & operator < (T v) { this->suppress_final_newline = true;return *this << v;}

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

private:
    enum phase {enter, exit, exception};

    struct event {
        std::string_view name;
        phase ph;
        std::source_location loc;
        std::thread::id thread_id;
        std::chrono::milliseconds duration {};
        std::string_view what {};
    };

    template <typename F> class observer {
        std::string_view name_;
        std::source_location loc_;
        F f_;

        template <typename Self, typename...Args> static decltype(auto) call_impl(Self&&self, Args&&...args);

    public:
        constexpr observer(const std::string_view name, const std::source_location loc, F f) : name_(name), loc_(loc), f_(std::move(f)) {}
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

public:
    template <typename F> static constexpr auto observe(std::string_view, F&&, std::source_location=std::source_location::current());
    template <typename F> static constexpr auto observe(F&&, std::source_location=std::source_location::current());
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

inline emkylog::error_code emkylog::Init() {return emkylog::init();}
inline emkylog::error_code emkylog::SetControl(const emkylog::control_s & control) noexcept {return emkylog::set_control(control);}
inline emkylog::error_code emkylog::SetLogPath(const std::string_view path) {return emkylog::set_log_path(path);}
inline emkylog::error_code emkylog::SetErrorLogPath(const std::string_view path) {return emkylog::set_error_log_path(path);}
inline emkylog::error_code emkylog::SetLogFilename(const std::string_view filename) noexcept {return emkylog::set_log_filename(filename);}
inline emkylog::error_code emkylog::SetErrorLogFilename(const std::string_view filename) noexcept {return emkylog::set_error_log_filename(filename);}
inline void emkylog::SetAutoNewLine(bool && boolean) noexcept {return emkylog::set_auto_new_line(static_cast<bool&&>(boolean));}
inline emkylog::control_s emkylog::GetControl() noexcept {return emkylog::get_control();}
inline std::string_view emkylog::GetLogPath() noexcept {return emkylog::get_log_path();}
inline std::string_view emkylog::GetErrorLogPath() noexcept {return emkylog::get_error_log_path();}
inline std::string_view emkylog::GetLogFilename() noexcept {return emkylog::get_log_filename();}
inline std::string_view emkylog::GetErrorLogFilename() noexcept {return emkylog::get_error_log_filename();}
inline bool emkylog::GetAutoNewLine() noexcept {return emkylog::get_auto_new_line();}
inline emkylog::error_code emkylog::Log(const std::string_view log, const emkylog::control_s & ctrl) {return emkylog::log(log, ctrl);}
inline emkylog::error_code emkylog::LogError(const std::string_view log, const emkylog::control_s & ctrl) {return emkylog::log_error(log, ctrl);}
inline emkylog::error_code emkylog::OpenLogger() {return emkylog::open_logger();}
inline emkylog::error_code emkylog::CloseLogger() {return emkylog::close_logger();}
inline bool emkylog::Initiated() noexcept {return emkylog::initiated();}
template <typename... Args> emkylog::error_code emkylog::LogError(Args &&... args) {return emkylog::log_error(std::forward<Args>(args)...);}
template <typename... Args> emkylog::error_code emkylog::Log(Args &&... args) {return emkylog::log(std::forward<Args>(args)...);}


inline emkylog::error_code emkylog::init() {
    std::lock_guard lock (emkylog::mtx);

    if (emkylog::set_log_path(emkylog::log_path) || emkylog::set_error_log_path(emkylog::error_log_path) || emkylog::set_log_filename(emkylog::log_filename) || emkylog::set_error_log_filename(emkylog::error_log_filename)) {
        return INIT_FAILED;
    }
    emkylog::inited = true;

    return NO_ERROR;
}


inline emkylog::error_code emkylog::set_control(control_s control) noexcept {
    std::lock_guard lock (emkylog::mtx);
    emkylog::control = std::move(control);
    return NO_ERROR;
}


inline emkylog::error_code emkylog::set_log_path(const std::string_view path) {
    std::lock_guard lock (emkylog::mtx);
    if (emkylog::log_stream.is_open()) {
        return FILE_OPENED;
    }

    std::error_code ec;
    std::filesystem::create_directories(path, ec);

    if (ec) {
        return FAILED_DIRECTORY_CREATION;
    }

    emkylog::log_path = path;

    return NO_ERROR;
}


inline emkylog::error_code emkylog::set_error_log_path(const std::string_view path) {
    std::lock_guard lock (emkylog::mtx);
    if (emkylog::error_log_stream.is_open()) {
        return FILE_OPENED;
    }

    std::error_code ec;
    std::filesystem::create_directories(path, ec);

    if (ec) {
        return FAILED_DIRECTORY_CREATION;
    }

    emkylog::error_log_path = path;

    return NO_ERROR;
}


inline emkylog::error_code emkylog::set_log_filename(const std::string_view filename) noexcept {
    std::lock_guard lock (emkylog::mtx);
    if (emkylog::log_stream.is_open()) {
        return FILE_OPENED;
    }

    if (filename.empty()) {
        return INVALID_FILENAME;
    }

    {if (const std::ofstream check (std::filesystem::path(emkylog::log_path) / filename, std::ios::app); !check) {
        return FAILED_FILE_CREATION;
    }}
    emkylog::log_filename = filename;

    return NO_ERROR;
}


inline emkylog::error_code emkylog::set_error_log_filename(const std::string_view filename) noexcept {
    std::lock_guard lock (emkylog::mtx);
    if (emkylog::error_log_stream.is_open()) {
        return FILE_OPENED;
    }

    if (filename.empty()) {
        return INVALID_FILENAME;
    }

    {if (const std::ofstream check (std::filesystem::path(emkylog::error_log_path) / filename, std::ios::app); !check) {
        return FAILED_FILE_CREATION;
    }}
    emkylog::error_log_filename = filename;

    return NO_ERROR;
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


inline emkylog::error_code emkylog::log(const std::string_view slog, const emkylog::control_s & ctrl) {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::initiated()) {
        if (const error_code res = emkylog::init()) {
            return res;
        }
    }

    if (!emkylog::log_stream.is_open()) {
        emkylog::log_stream.open(std::filesystem::path(emkylog::log_path) / emkylog::log_filename, std::ios::app);

        if (!emkylog::log_stream.is_open()) {
            return FILE_CLOSED;
        }
    }

    emkylog::log_stream << slog; // TODO: add here control filters
    emkylog::log_stream << (ctrl.newline.value_or(false) ? "\n" : "");
    emkylog::log_stream.flush();

    return NO_ERROR;
}


template <typename... Args> emkylog::error_code emkylog::log(Args &&... args) {
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


inline emkylog::error_code emkylog::log_error(const std::string_view slog, const control_s & ctrl) {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::initiated()) {
        if (const emkylog::error_code res = emkylog::init()) {
            return res;
        }
    }

    if (!emkylog::error_log_stream.is_open()) {
        emkylog::error_log_stream.open(std::filesystem::path(emkylog::error_log_path) / emkylog::error_log_filename, std::ios::app);

        if (!emkylog::error_log_stream.is_open()) {
            return FILE_CLOSED;
        }
    }

    emkylog::error_log_stream << slog; // TODO: add here control filters
    emkylog::log_stream << (ctrl.newline.value_or(false) ? "\n" : "");
    emkylog::error_log_stream.flush();

    return NO_ERROR;
}


template <typename... Args> emkylog::error_code emkylog::log_error(Args &&...args) {
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


inline emkylog::error_code emkylog::open_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::initiated()) {
        if (const emkylog::error_code res = emkylog::init()) {
            return res;
        }
    }

    if (emkylog::log_stream.is_open()) {
        return FILE_OPENED;
    }

    emkylog::log_stream.open(std::filesystem::path(emkylog::log_path) / emkylog::log_filename, std::ios::app);

    if (!emkylog::log_stream.is_open()) {
        return FILE_CLOSED;
    }
    return NO_ERROR;
}


inline emkylog::error_code emkylog::open_error_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::initiated()) {
        if (const emkylog::error_code res = emkylog::init()) {
            return res;
        }
    }

    if (emkylog::error_log_stream.is_open()) {
        return FILE_OPENED;
    }

    emkylog::error_log_stream.open(std::filesystem::path(emkylog::error_log_path) / emkylog::error_log_filename, std::ios::app);

    if (!emkylog::error_log_stream.is_open()) {
        return FILE_CLOSED;
    }
    return NO_ERROR;
}


inline emkylog::error_code emkylog::close_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::log_stream.is_open()) {
        return FILE_CLOSED;
    }

    emkylog::log_stream.close();
    return NO_ERROR;
}


inline emkylog::error_code emkylog::close_error_logger() {
    std::lock_guard lock (emkylog::mtx);
    if (!emkylog::error_log_stream.is_open()) {
        return FILE_CLOSED;
    }

    emkylog::error_log_stream.close();
    return NO_ERROR;
}


inline bool emkylog::initiated() noexcept {
    std::lock_guard lock (emkylog::mtx);
    return emkylog::inited;
}


template<typename Tuple, size_t... Is> inline void emkylog::stream_prefix(line & l, Tuple && t, std::index_sequence<Is...>) {
    (l << ... << std::get<Is>(std::forward<Tuple>(t)));
}


template<typename F> constexpr auto emkylog::observe(std::string_view name, F && f, std::source_location loc) {
    return emkylog::observer<std::decay_t<F>>{name, loc, std::forward<F>(f)};
}


template<typename F> constexpr auto emkylog::observe(F && f, std::source_location loc) {
    return emkylog::observe(loc.function_name(), std::forward<F>(f), loc);
}


inline void emkylog::log_event(const event & e) {
    emkylog::loginfo << "[Observer Log] Thread ID: " << e.thread_id << " : at " << e.loc.function_name() << " Status:  ";

    switch (e.ph) {
        case emkylog::enter:
            emkylog::loginfo << " ENTERED.";
            break;

        case emkylog::exit:
            emkylog::loginfo << " EXITED.";
            break;

        case emkylog::exception:
            emkylog::loginfo << " THROWED.";
            break;
    }

    emkylog::loginfo << " " << e.duration.count() << "ms.\n";
}


template<typename F> template<typename Self, typename... Args> decltype(auto) emkylog::observer<F>::call_impl(Self && self, Args &&... args) {
    using clock = std::chrono::steady_clock;
    const auto start = clock::now();
    emkylog::log_event(event{
        .name = self.name_,
        .ph = phase::enter,
        .loc = self.loc_,
        .thread_id = std::this_thread::get_id(),
        .duration = {}
    });

    try {
        using R = std::invoke_result_t<decltype((self.f_)), Args...>;

        if constexpr (std::is_void_v<R>) {
            std::invoke(self.f_, std::forward<Args>(args)...);

            const auto end = clock::now();
            emkylog::log_event(event{
                .name = self.name_,
                .ph = phase::exit,
                .loc = self.loc_,
                .thread_id = std::this_thread::get_id(),
                .duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            });
        } else {
            R result = std::invoke(self.f_, std::forward<Args>(args)...);

            const auto end = clock::now();
            emkylog::log_event(event{
                .name = self.name_,
                .ph = phase::exit,
                .loc = self.loc_,
                .thread_id = std::this_thread::get_id(),
                .duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            });

            return result;
        }
    } catch (std::exception & e) {
        const auto end = clock::now();
        emkylog::log_event(event{
            .name = self.name_,
            .ph = phase::exception,
            .loc = self.loc_,
            .thread_id = std::this_thread::get_id(),
            .duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start),
            .what = e.what()
        });
        throw;
    }
}



#endif //EMKYLOG_H