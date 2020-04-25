# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Users\aless\Documents\PSoC_Creator\AY1920_II_HW_05_RATTI_ALESSIO\AY1920_II_HW_05_PROJ_3.cydsn\AY1920_II_HW_05_PROJ_3.cyprj
# Date: Sat, 25 Apr 2020 10:20:15 GMT
#set_units -time ns
create_clock -name {CyILO} -period 1000000 -waveform {0 500000} [list [get_pins {ClockBlock/ilo}] [get_pins {ClockBlock/clk_100k}] [get_pins {ClockBlock/clk_1k}] [get_pins {ClockBlock/clk_32k}]]
create_clock -name {CyIMO} -period 333.33333333333331 -waveform {0 166.666666666667} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyPLL_OUT} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/pllout}]]
create_clock -name {CyMASTER_CLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/clk_sync}]]
create_generated_clock -name {CyBUS_CLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 2 3} [list [get_pins {ClockBlock/clk_bus_glb}]]
create_generated_clock -name {UART_Debug_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 313 627} [list [get_pins {ClockBlock/dclk_glb_0}]]


# Component constraints for C:\Users\aless\Documents\PSoC_Creator\AY1920_II_HW_05_RATTI_ALESSIO\AY1920_II_HW_05_PROJ_3.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\aless\Documents\PSoC_Creator\AY1920_II_HW_05_RATTI_ALESSIO\AY1920_II_HW_05_PROJ_3.cydsn\AY1920_II_HW_05_PROJ_3.cyprj
# Date: Sat, 25 Apr 2020 10:20:10 GMT
