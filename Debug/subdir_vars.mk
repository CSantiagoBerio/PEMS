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
./syscfg/ti_drivers_config.c \
../main_tirtos.c \
../temp.c 

GEN_FILES += \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./syscfg/ti_drivers_config.d \
./main_tirtos.d \
./temp.d 

OBJS += \
./syscfg/ti_drivers_config.obj \
./main_tirtos.obj \
./temp.obj 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/syscfg_c.rov.xs 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"syscfg\ti_drivers_config.obj" \
"main_tirtos.obj" \
"temp.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\syscfg_c.rov.xs" 

C_DEPS__QUOTED += \
"syscfg\ti_drivers_config.d" \
"main_tirtos.d" \
"temp.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_drivers_config.c" 

SYSCFG_SRCS__QUOTED += \
"../gpiointerrupt.syscfg" 

C_SRCS__QUOTED += \
"./syscfg/ti_drivers_config.c" \
"../main_tirtos.c" \
"../temp.c" 


