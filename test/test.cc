#include <cci_logger.hh>


auto
main( void ) -> int32_t
{
    Logger logger { DEBUG };

    logger.log<DEBUG>("Test debug");
    logger.log<INFO>("Test info");
    logger.log<WARN>("Test warn");

    logger.set_time_format("%H:%M:%S");
    logger.log<INFO>("Test time");

    logger.set_time_format();
    logger.set_log_format("[{0} {1} {2} {3}:{4}] >> {5}\n");
    logger.log<WARN>("Test log format {}", 1);

    logger.set_log_format();
    logger.abort_on_error(false);
    logger.log<ERROR>("Test ERROR");

    Logger other { INFO };

    other.log<DEBUG>("wont print");

    other = logger;

    logger.log<DEBUG>("TEST LOGGER");
    other.log<DEBUG>("TEST OTHER");

    return 0;
}