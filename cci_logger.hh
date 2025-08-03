#pragma once
#include <source_location>
#include <string_view>
#include <cstdint>
#include <format>


/**
 * @enum LogLevel
 * @brief Defines severity levels for log messages.
 */
enum LogLevel : uint8_t
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    __LOG_LEVEL_AMOUNT,
};


class Logger
{
public:
    struct FormatString
    {
        std::string_view fmt;
        std::source_location source;

        FormatString( const char                 *p_fmt,
                      const std::source_location &p_source =
                            std::source_location::current() ) :
            fmt(p_fmt), source(p_source) {}
    };


    /**
     * @brief Constructs a Logger with optional log level threshold.
     * @param p_loglevel Minimum level to log (default WARN).
     */
    Logger( const LogLevel &p_loglevel = WARN );


    /**
     * @brief Sets the time format string for log timestamps.
     * @param p_fmt Format string (e.g., "%H:%M:%S").
     */
    void set_time_format( const std::string &p_fmt = "%MS.%S:%M" );


    /**
     * @brief Sets the overall log message format.
     * @param p_fmt Format string with placeholders.
     */
    void set_log_format( const std::string &p_fmt );


    /**
     * @brief Resets the log format.
     */
    void set_log_format( void );


    /**
     * @brief Enables or disables aborting on error log entries.
     * @param p_abort True to abort on errors (default true).
     */
    void abort_on_error( const bool &p_abort = true );


    /**
     * @brief Enables or disables prompting user to continue on error.
     * @param p_ask True to prompt user (default true).
     */
    void ask_continue_on_error( const bool &p_ask = true );


    /**
     * @brief Enables or disables coloured log output.
     * @param p_coloured True to enable colours (default true).
     */
    void set_coloured_log( const bool &p_coloured = true );


    /**
     * @brief Logs a message at the specified log level.
     * @tparam T_Level LogLevel template parameter for severity.
     * @tparam T_Args  Variadic arguments for formatting.
     * @param p_fmt  FormatString containing text and source info.
     * @param p_args Arguments to format the message.
     *
     * This function performs several steps:
     * - Checks if the log level meets the threshold; ignores if not.
     * - Retrieves current time and source location info.
     * - Formats the message using the provided format string and args.
     * - Applies configured log message format, adding colours if enabled.
     * - Outputs the formatted message to stdout or stderr.
     * - On error level, may prompt user to continue or abort execution.
     */
    template<LogLevel T_Level, typename... T_Args>
    void log( const FormatString &p_fmt,
              T_Args         &&...p_args )
    {
        if (T_Level < m_threshold_level) return;

        std::string
            time      { get_time() },
            file      { p_fmt.source.file_name() },
            function  { p_fmt.source.function_name() },
            line      { std::to_string(p_fmt.source.line()) },
            log_level { m_coloured ? m_LOG_LABELS[T_Level].first
                                   : m_LOG_LABELS[T_Level].second },
            msg  { std::vformat(p_fmt.fmt, std::make_format_args(p_args...)) };

        function = function.substr(function.find_first_of(' ') + 1);
        function = function.substr(0, function.find('('));

        std::string log_format { m_log_format };
        if (m_log_format.empty())
            log_format = m_coloured ? m_LOG_FORMATS.first
                                    : m_LOG_FORMATS.second;
        std::string full { format(log_format,
                           time, log_level, function, file, line, msg) };

        print_log(full, T_Level >= WARN);

        if (T_Level == ERROR && m_abort_on_err)
            if (!ask_continue()) std::abort();
    }

private:
    using view_pair = std::pair<std::string_view, std::string_view>;

    static constexpr std::array<view_pair, __LOG_LEVEL_AMOUNT> m_LOG_LABELS {{
        { "\033[1;36mdebug\033[0;0;0m", "debug" },
        { "\033[1;32minfo\033[0;0;0m",  "info"  },
        { "\033[1;33mwarn\033[0;0;0m",  "warn"  },
        { "\033[1;31merror\033[0;0;0m", "error" },
    }};
    static constexpr view_pair m_LOG_FORMATS {
        "[{0} {1} at \033[1m{2}\033[0m( \033[1;30m{3}:{4}\033[0;0m )]: "
        "\033[1m{5}\033[0m\n",
        "[{0} {1} at {2}( {3}:{4} )]: {5}\n"
    };


    LogLevel m_threshold_level;

    std::string m_time_format;
    std::string m_log_format;

    bool m_coloured;
    bool m_ask_continue;
    bool m_abort_on_err;


    /** @brief Returns current time formatted as string */
    auto get_time( void ) -> std::string;


    /**
     * @brief Prompts user whether to continue after error.
     * @return True if user chooses to continue.
     */
    auto ask_continue( void ) -> bool;


    /**
     * @brief Prints the log message to stdout or stderr.
     * @param p_msg The full formatted log message.
     * @param p_err True if message is an error (print to stderr).
     */
    void print_log( const std::string &p_msg, const bool &p_err );


    /**
     * @brief Helper: format a string with given arguments.
     * @param p_fmt  Format string.
     * @param p_args Arguments to format.
     * @return Formatted string.
     */
    template<typename... T_Args>
    auto format( std::string_view p_fmt, T_Args &&...p_args ) -> std::string
    {
        return std::vformat(p_fmt, std::make_format_args(
                                   std::forward<T_Args>(p_args)...));
    }
};