################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../CC3220S_LAUNCHXL_TIRTOS.cmd 

SYSCFG_SRCS += \
../gpiointerrupt.syscfg 

C_SRCS += \
../MHZ16.c \
../dht22.c \
./syscfg/ti_drivers_config.c \
../main_tirtos.c \
../pems.c 

GEN_FILES += \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./MHZ16.d \
./dht22.d \
./syscfg/ti_drivers_config.d \
./main_tirtos.d \
./pems.d 

OBJS += \
./MHZ16.obj \
./dht22.obj \
./syscfg/ti_drivers_config.obj \
./main_tirtos.obj \
./pems.obj 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/syscfg_c.rov.xs 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"MHZ16.obj" \
"dht22.obj" \
"syscfg\ti_drivers_config.obj" \
"main_tirtos.obj" \
"pems.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\syscfg_c.rov.xs" 

C_DEPS__QUOTED += \
"MHZ16.d" \
"dht22.d" \
"syscfg\ti_drivers_config.d" \
"main_tirtos.d" \
"pems.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_drivers_config.c" 

C_SRCS__QUOTED += \
"../MHZ16.c" \
"../dht22.c" \
"./syscfg/ti_drivers_config.c" \
"../main_tirtos.c" \
"../pems.c" 

SYSCFG_SRCS__QUOTED += \
"../gpiointerrupt.syscfg" 


