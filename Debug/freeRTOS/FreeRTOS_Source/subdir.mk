################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freeRTOS/FreeRTOS_Source/croutine.c \
../freeRTOS/FreeRTOS_Source/event_groups.c \
../freeRTOS/FreeRTOS_Source/list.c \
../freeRTOS/FreeRTOS_Source/queue.c \
../freeRTOS/FreeRTOS_Source/stream_buffer.c \
../freeRTOS/FreeRTOS_Source/tasks.c \
../freeRTOS/FreeRTOS_Source/timers.c 

OBJS += \
./freeRTOS/FreeRTOS_Source/croutine.o \
./freeRTOS/FreeRTOS_Source/event_groups.o \
./freeRTOS/FreeRTOS_Source/list.o \
./freeRTOS/FreeRTOS_Source/queue.o \
./freeRTOS/FreeRTOS_Source/stream_buffer.o \
./freeRTOS/FreeRTOS_Source/tasks.o \
./freeRTOS/FreeRTOS_Source/timers.o 

C_DEPS += \
./freeRTOS/FreeRTOS_Source/croutine.d \
./freeRTOS/FreeRTOS_Source/event_groups.d \
./freeRTOS/FreeRTOS_Source/list.d \
./freeRTOS/FreeRTOS_Source/queue.d \
./freeRTOS/FreeRTOS_Source/stream_buffer.d \
./freeRTOS/FreeRTOS_Source/tasks.d \
./freeRTOS/FreeRTOS_Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
freeRTOS/FreeRTOS_Source/%.o: ../freeRTOS/FreeRTOS_Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -I../../freeRTOS_UDP_bsp/ps7_cortexa9_1/include -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\portable\GCC\ARM_CA9" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\Compiler\GCC" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\dma" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\queue" -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


