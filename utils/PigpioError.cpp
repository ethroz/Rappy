#include <stdexcept>
#include <pigpio.h>

#include "PigpioError.hpp"

namespace pigpio {

std::string error(int code) {
    switch (code) {
    case PI_INIT_FAILED:      return "GpioInitialise failed";
    case PI_BAD_USER_GPIO:    return "GPIO not 0-31";
    case PI_BAD_GPIO:         return "GPIO not 0-53";
    case PI_BAD_MODE:         return "Mode not 0-7";
    case PI_BAD_LEVEL:        return "Level not 0-1";
    case PI_BAD_PUD:          return "Pud not 0-2";
    case PI_BAD_PULSEWIDTH:   return "Pulsewidth not 0 or 500-2500";
    case PI_BAD_DUTYCYCLE:    return "Dutycycle outside set range";
    case PI_BAD_TIMER:        return "Timer not 0-9";
    case PI_BAD_MS:           return "Ms not 10-60000";
    case PI_BAD_TIMETYPE:     return "Timetype not 0-1";
    case PI_BAD_SECONDS:      return "Seconds < 0";
    case PI_BAD_MICROS:       return "Micros not 0-999999";
    case PI_TIMER_FAILED:     return "GpioSetTimerFunc failed";
    case PI_BAD_WDOG_TIMEOUT: return "Timeout not 0-60000";
    case PI_NO_ALERT_FUNC:    return "DEPRECATED";
    case PI_BAD_CLK_PERIPH:   return "Clock peripheral not 0-1";
    case PI_BAD_CLK_SOURCE:   return "DEPRECATED";
    case PI_BAD_CLK_MICROS:   return "Clock micros not 1, 2, 4, 5, 8, or 10";
    case PI_BAD_BUF_MILLIS:   return "Buf millis not 100-10000";
    case PI_BAD_DUTYRANGE:    return "Dutycycle range not 25-40000";
    case PI_BAD_SIGNUM:       return "Signum not 0-63";
    case PI_BAD_PATHNAME:     return "Can't open pathname";
    case PI_NO_HANDLE:        return "No handle available";
    case PI_BAD_HANDLE:       return "Unknown handle";
    case PI_BAD_IF_FLAGS:     return "IfFlags > 3";
    case PI_BAD_PRIM_CHANNEL: return "DMA primary channel not 0-14";
    case PI_BAD_SOCKET_PORT:  return "Socket port not 1024-32000";
    case PI_BAD_FIFO_COMMAND: return "Unrecognized fifo command";
    case PI_BAD_SECO_CHANNEL: return "DMA secondary channel not 0-6";
    case PI_NOT_INITIALISED:  return "Function called before gpioInitialise";
    case PI_INITIALISED:      return "Function called after gpioInitialise";
    case PI_BAD_WAVE_MODE:    return "Waveform mode not 0-3";
    case PI_BAD_CFG_INTERNAL: return "Bad parameter in gpioCfgInternals call";
    case PI_BAD_WAVE_BAUD:    return "Baud rate not 50-250K(RX)/50-1M(TX)";
    case PI_TOO_MANY_PULSES:  return "Waveform has too many pulses";
    case PI_TOO_MANY_CHARS:   return "Waveform has too many chars";
    case PI_NOT_SERIAL_GPIO:  return "No bit bang serial read on GPIO";
    case PI_BAD_SERIAL_STRUC: return "Bad (null) serial structure parameter";
    case PI_BAD_SERIAL_BUF:   return "Bad (null) serial buf parameter";
    case PI_NOT_PERMITTED:    return "GPIO operation not permitted";
    case PI_SOME_PERMITTED:   return "One or more GPIO not permitted";
    case PI_BAD_WVSC_COMMND:  return "Bad WVSC subcommand";
    case PI_BAD_WVSM_COMMND:  return "Bad WVSM subcommand";
    case PI_BAD_WVSP_COMMND:  return "Bad WVSP subcommand";
    case PI_BAD_PULSELEN:     return "Trigger pulse length not 1-100";
    case PI_BAD_SCRIPT:       return "Invalid script";
    case PI_BAD_SCRIPT_ID:    return "Unknown script id";
    case PI_BAD_SER_OFFSET:   return "Add serial data offset > 30 minutes";
    case PI_GPIO_IN_USE:      return "GPIO already in use";
    case PI_BAD_SERIAL_COUNT: return "Must read at least a byte at a time";
    case PI_BAD_PARAM_NUM:    return "Script parameter id not 0-9";
    case PI_DUP_TAG:          return "Script has duplicate tag";
    case PI_TOO_MANY_TAGS:    return "Script has too many tags";
    case PI_BAD_SCRIPT_CMD:   return "Illegal script command";
    case PI_BAD_VAR_NUM:      return "Script variable id not 0-149";
    case PI_NO_SCRIPT_ROOM:   return "No more room for scripts";
    case PI_NO_MEMORY:        return "Can't allocate temporary memory";
    case PI_SOCK_READ_FAILED: return "Socket read failed";
    case PI_SOCK_WRIT_FAILED: return "Socket write failed";
    case PI_TOO_MANY_PARAM:   return "Too many script parameters (> 10)";
    case PI_SCRIPT_NOT_READY: return "Script initialising";
    case PI_BAD_TAG:          return "Script has unresolved tag";
    case PI_BAD_MICS_DELAY:   return "Bad MICS delay (too large)";
    case PI_BAD_MILS_DELAY:   return "Bad MILS delay (too large)";
    case PI_BAD_WAVE_ID:      return "Non existent wave id";
    case PI_TOO_MANY_CBS:     return "No more CBs for waveform";
    case PI_TOO_MANY_OOL:     return "No more OOL for waveform";
    case PI_EMPTY_WAVEFORM:   return "Attempt to create an empty waveform";
    case PI_NO_WAVEFORM_ID:   return "No more waveforms";
    case PI_I2C_OPEN_FAILED:  return "Can't open I2C device";
    case PI_SER_OPEN_FAILED:  return "Can't open serial device";
    case PI_SPI_OPEN_FAILED:  return "Can't open SPI device";
    case PI_BAD_I2C_BUS:      return "Bad I2C bus";
    case PI_BAD_I2C_ADDR:     return "Bad I2C address";
    case PI_BAD_SPI_CHANNEL:  return "Bad SPI channel";
    case PI_BAD_FLAGS:        return "Bad i2c/spi/ser open flags";
    case PI_BAD_SPI_SPEED:    return "Bad SPI speed";
    case PI_BAD_SER_DEVICE:   return "Bad serial device name";
    case PI_BAD_SER_SPEED:    return "Bad serial baud rate";
    case PI_BAD_PARAM:        return "Bad i2c/spi/ser parameter";
    case PI_I2C_WRITE_FAILED: return "I2c write failed";
    case PI_I2C_READ_FAILED:  return "I2c read failed";
    case PI_BAD_SPI_COUNT:    return "Bad SPI count";
    case PI_SER_WRITE_FAILED: return "Ser write failed";
    case PI_SER_READ_FAILED:  return "Ser read failed";
    case PI_SER_READ_NO_DATA: return "Ser read no data available";
    case PI_UNKNOWN_COMMAND:  return "Unknown command";
    case PI_SPI_XFER_FAILED:  return "Spi xfer/read/write failed";
    case PI_BAD_POINTER:      return "Bad (NULL) pointer";
    case PI_NO_AUX_SPI:       return "No auxiliary SPI on Pi A or B";
    case PI_NOT_PWM_GPIO:     return "GPIO is not in use for PWM";
    case PI_NOT_SERVO_GPIO:   return "GPIO is not in use for servo pulses";
    case PI_NOT_HCLK_GPIO:    return "GPIO has no hardware clock";
    case PI_NOT_HPWM_GPIO:    return "GPIO has no hardware PWM";
    case PI_BAD_HPWM_FREQ:    return "Hardware PWM frequency not 1-125M";
    case PI_BAD_HPWM_DUTY:    return "Hardware PWM dutycycle not 0-1M";
    case PI_BAD_HCLK_FREQ:    return "Hardware clock frequency not 4689-250M";
    case PI_BAD_HCLK_PASS:    return "Need password to use hardware clock 1";
    case PI_HPWM_ILLEGAL:     return "Illegal, PWM in use for main clock";
    case PI_BAD_DATABITS:     return "Serial data bits not 1-32";
    case PI_BAD_STOPBITS:     return "Serial (half) stop bits not 2-8";
    case PI_MSG_TOOBIG:       return "Socket/pipe message too big";
    case PI_BAD_MALLOC_MODE:  return "Bad memory allocation mode";
    case PI_TOO_MANY_SEGS:    return "Too many I2C transaction segments";
    case PI_BAD_I2C_SEG:      return "An I2C transaction segment failed";
    case PI_BAD_SMBUS_CMD:    return "SMBus command not supported by driver";
    case PI_NOT_I2C_GPIO:     return "No bit bang I2C in progress on GPIO";
    case PI_BAD_I2C_WLEN:     return "Bad I2C write length";
    case PI_BAD_I2C_RLEN:     return "Bad I2C read length";
    case PI_BAD_I2C_CMD:      return "Bad I2C command";
    case PI_BAD_I2C_BAUD:     return "Bad I2C baud rate, not 50-500k";
    case PI_CHAIN_LOOP_CNT:   return "Bad chain loop count";
    case PI_BAD_CHAIN_LOOP:   return "Empty chain loop";
    case PI_CHAIN_COUNTER:    return "Too many chain counters";
    case PI_BAD_CHAIN_CMD:    return "Bad chain command";
    case PI_BAD_CHAIN_DELAY:  return "Bad chain delay micros";
    case PI_CHAIN_NESTING:    return "Chain counters nested too deeply";
    case PI_CHAIN_TOO_BIG:    return "Chain is too long";
    case PI_DEPRECATED:       return "Deprecated function removed";
    case PI_BAD_SER_INVERT:   return "Bit bang serial invert not 0 or 1";
    case PI_BAD_EDGE:         return "Bad ISR edge value, not 0-2";
    case PI_BAD_ISR_INIT:     return "Bad ISR initialisation";
    case PI_BAD_FOREVER:      return "Loop forever must be last command";
    case PI_BAD_FILTER:       return "Bad filter parameter";
    case PI_BAD_PAD:          return "Bad pad number";
    case PI_BAD_STRENGTH:     return "Bad pad drive strength";
    case PI_FIL_OPEN_FAILED:  return "File open failed";
    case PI_BAD_FILE_MODE:    return "Bad file mode";
    case PI_BAD_FILE_FLAG:    return "Bad file flag";
    case PI_BAD_FILE_READ:    return "Bad file read";
    case PI_BAD_FILE_WRITE:   return "Bad file write";
    case PI_FILE_NOT_ROPEN:   return "File not open for read";
    case PI_FILE_NOT_WOPEN:   return "File not open for write";
    case PI_BAD_FILE_SEEK:    return "Bad file seek";
    case PI_NO_FILE_MATCH:    return "No files match pattern";
    case PI_NO_FILE_ACCESS:   return "No permission to access file";
    case PI_FILE_IS_A_DIR:    return "File is a directory";
    case PI_BAD_SHELL_STATUS: return "Bad shell return status";
    case PI_BAD_SCRIPT_NAME:  return "Bad script name";
    case PI_BAD_SPI_BAUD:     return "Bad SPI baud rate, not 50-500k";
    case PI_NOT_SPI_GPIO:     return "No bit bang SPI in progress on GPIO";
    case PI_BAD_EVENT_ID:     return "Bad event id";
    case PI_PIGIF_ERR_0:      return "If error 0";
    case PI_PIGIF_ERR_99:     return "If error 99";
    case PI_CUSTOM_ERR_0:     return "Custom error 0";
    case PI_CUSTOM_ERR_999:   return "Custom error 999";
    default:                  return "Unknown error";
    }
}

int checkError(int code) {
    if (code < 0) {
        throw std::runtime_error(error(code));
    }
    return code;
}

} // namespace pigpio

