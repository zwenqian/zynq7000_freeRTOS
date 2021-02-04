################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/NetworkInterface.c \
../freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/uncached_memory.c \
../freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_dma.c \
../freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_hw.c \
../freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_physpeed.c 

OBJS += \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/NetworkInterface.o \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/uncached_memory.o \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_dma.o \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_hw.o \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_physpeed.o 

C_DEPS += \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/NetworkInterface.d \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/uncached_memory.d \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_dma.d \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_hw.d \
./freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/x_emacpsif_physpeed.d 


# Each subdirectory must supply rules for building sources it contributes
freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/%.o: ../freeRTOS/FreeRTOS_UDP_Source/portable/NetworkInterface/Zynq/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -I../../freeRTOS_UDP_bsp/ps7_cortexa9_1/include -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\portable\GCC\ARM_CA9" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\Compiler\GCC" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\dma" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\queue" -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


