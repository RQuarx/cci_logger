#include <iostream>
#include <chrono>
#include "cci_logger.hh"

#ifdef _WIN32
    #include <io.h>
#elif defined(__unix__) || defined(__APPLE__) || defined(__linux__)
    #include <unistd.h>
#else
    #error "Unsupported platform"
#endif


namespace
{
    inline auto
    is_stdin_available( void ) -> bool
    { return isatty(fileno(stdin)); }


    auto
    format_time( std::string_view p_fmt ) -> std::string
    {
        using namespace std::chrono;

        time_point now { std::chrono::system_clock::now() };
        std::time_t time { std::chrono::system_clock::to_time_t(now) };
        tm tm { *std::localtime(&time) };

        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream out;

        while (!p_fmt.empty()) {
            if (p_fmt[0] == '%' && p_fmt.size() >= 2) {
                if (p_fmt.substr(0, 3) == "%MS") {
                    out << std::setw(3) << std::setfill('0') << ms.count();
                    p_fmt.remove_prefix(3);
                } else if (p_fmt.substr(0, 2) == "%S") {
                    out << std::setw(2) << std::setfill('0') << tm.tm_sec;
                    p_fmt.remove_prefix(2);
                } else if (p_fmt.substr(0, 2) == "%M") {
                    out << std::setw(2) << std::setfill('0') << tm.tm_min;
                    p_fmt.remove_prefix(2);
                } else if (p_fmt.substr(0, 2) == "%H") {
                    out << std::setw(2) << std::setfill('0') << tm.tm_hour;
                    p_fmt.remove_prefix(2);
                } else if (p_fmt.substr(0, 2) == "%D") {
                    out << std::put_time(&tm, "%Y-%m-%d");
                    p_fmt.remove_prefix(2);
                } else {
                    out << p_fmt[0];
                    p_fmt.remove_prefix(1);
                }
            } else {
                out << p_fmt[0];
                p_fmt.remove_prefix(1);
            }
        }

        return out.str();
    }
}


Logger::Logger( const LogLevel &p_loglevel ) :
    m_threshold_level(p_loglevel),
    m_time_format("%M:%S.%MS"),
    m_coloured(true),
    m_ask_continue(true),
    m_abort_on_err(true)
{}


void
Logger::set_time_format( const std::string &p_fmt )
{ m_time_format = p_fmt; }


void
Logger::set_log_format( const std::string &p_fmt )
{ m_log_format = p_fmt; }


void
Logger::set_log_format( void )
{ m_log_format.clear(); }


void
Logger::abort_on_error( const bool &p_abort )
{ m_abort_on_err = p_abort; }


void
Logger::ask_continue_on_error( const bool &p_ask )
{ m_ask_continue = p_ask; }


void
Logger::set_coloured_log( const bool &p_coloured )
{ m_coloured = p_coloured; }


auto
Logger::get_time( void ) -> std::string
{
    return format_time(m_time_format);
}


auto
Logger::ask_continue( void ) -> bool
{
    if (!is_stdin_available()) return false;
    std::string line;

    while (true) {
        std::cerr << "An error has occured, do you want to continue? [y/N] ";
        std::getline(std::cin, line);

        if (line.empty()) return false;

        char answ { static_cast<char>(std::tolower(line.at(0))) };
        if (answ == 'y') return true;
        if (answ == 'n') return false;

        std::cerr << "Please enter y or n.\n";
    }
}


void
Logger::print_log( const std::string &p_msg, const bool &p_err )
{ (p_err ? std::cerr : std::clog) << p_msg; }