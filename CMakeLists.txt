idf_component_register(SRCS "main.c" "config.h"
        "gpio/gpioPins.c" "gpio/gpioPins.h"
        "display/display.c" "display/display.h" "display/bitmapDisplay.c" "display/bitmapDisplay.h" "display/PCF8574.c" "display/PCF8574.h" "display/displayScreen.h" "display/displayScreen.c"
        "uart/uart0.h" "uart/uart0.c" "uart/uart2.h" "uart/uart2.c"
        "SIM7000G/sim7000g.h" "SIM7000G/sim7000g.c"
        INCLUDE_DIRS ".")
