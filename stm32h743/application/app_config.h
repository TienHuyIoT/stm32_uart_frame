#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>\n

// <h> WDT Timeout Value

//==========================================================
// <o> WDOG_ENABLE

#ifndef WDOG_ENABLE
#define WDOG_ENABLE    1
#endif

//==========================================================
// <o> WDOG_TIMEOUT_VALUE

#ifndef WDOG_TIMEOUT_VALUE
#define WDOG_TIMEOUT_VALUE    3000 /* ms */
#endif

//==========================================================
// <o> WDOG_WINDOW_VALUE

#ifndef WDOG_WINDOW_VALUE
#define WDOG_WINDOW_VALUE    1000 /* ms */
#endif

// </h>

// <h> Serial Number

//==========================================================
// <o> SERIAL_NUMBER_ADDR

#ifndef SERIAL_NUMBER_ADDR
#define SERIAL_NUMBER_ADDR          0x1234 /* Address location store serial number */
#define SERIAL_NUMBER_LENGTH_MAX    32
#endif

// </h>

// <h> Hardware Version

//==========================================================
// <o> HW_VERSION_STRING

#ifndef HW_VERSION_STRING
#define HW_VERSION_STRING "NUCLEO-h743ZI"
#endif

// </h>

// <h> Firmware Version

//==========================================================
// <o> FW_VERSION_MAJOR

#ifndef FW_VERSION_MAJOR
#define FW_VERSION_MAJOR 1
#endif

//==========================================================
// <o> FW_VERSION_MINOR

#ifndef FW_VERSION_MINOR
#define FW_VERSION_MINOR 0
#endif

//==========================================================
// <o> FW_VERSION_BUILD

#ifndef FW_VERSION_BUILD
#define FW_VERSION_BUILD 1
#endif

// </h>

// <<< end of configuration section >>>

#endif /* APP_CONFIG_H */
