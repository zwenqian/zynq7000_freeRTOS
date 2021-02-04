################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/port.c 

S_UPPER_SRCS += \
../freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/portASM.S 

OBJS += \
./freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/port.o \
./freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/portASM.o 

S_UPPER_DEPS += \
./freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/portASM.d 

C_DEPS += \
./freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/port.d 


# Each subdirectory must supply rules for building sources it contributes
freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/%.o: ../freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -I../../freeRTOS_UDP_bsp/ps7_cortexa9_1/include -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\portable\GCC\ARM_CA9" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\Compiler\GCC" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\dma" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\queue" -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/%.o: ../freeRTOS/FreeRTOS_Source/portable/GCC/ARM_CA9/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -I../../freeRTOS_UDP_bsp/ps7_cortexa9_1/include -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\portable\GCC\ARM_CA9" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\Compiler\GCC" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\dma" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\queue" -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


