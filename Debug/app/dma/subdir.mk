################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/dma/dmaUser.c \
../app/dma/dma_intr.c \
../app/dma/sys_intr.c 

OBJS += \
./app/dma/dmaUser.o \
./app/dma/dma_intr.o \
./app/dma/sys_intr.o 

C_DEPS += \
./app/dma/dmaUser.d \
./app/dma/dma_intr.d \
./app/dma/sys_intr.d 


# Each subdirectory must supply rules for building sources it contributes
app/dma/%.o: ../app/dma/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -I../../freeRTOS_UDP_bsp/ps7_cortexa9_1/include -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\portable\GCC\ARM_CA9" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\Compiler\GCC" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\dma" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\queue" -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


