################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_ARP.c \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_DHCP.c \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_DNS.c \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_IP.c \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_Sockets.c \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_Stream_Buffer.c \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_TCP_IP.c \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_TCP_WIN.c \
../freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_UDP_IP.c 

OBJS += \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_ARP.o \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_DHCP.o \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_DNS.o \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_IP.o \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_Sockets.o \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_Stream_Buffer.o \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_TCP_IP.o \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_TCP_WIN.o \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_UDP_IP.o 

C_DEPS += \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_ARP.d \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_DHCP.d \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_DNS.d \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_IP.d \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_Sockets.d \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_Stream_Buffer.d \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_TCP_IP.d \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_TCP_WIN.d \
./freeRTOS/FreeRTOS_UDP_Source/FreeRTOS_UDP_IP.d 


# Each subdirectory must supply rules for building sources it contributes
freeRTOS/FreeRTOS_UDP_Source/%.o: ../freeRTOS/FreeRTOS_UDP_Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -I../../freeRTOS_UDP_bsp/ps7_cortexa9_1/include -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_Source\portable\GCC\ARM_CA9" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\Compiler\GCC" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\freeRTOS\FreeRTOS_UDP_Source\portable\NetworkInterface\include" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\dma" -I"F:\projectWork\embedded_platform\MIZ7035\projects\hub_ex_sys\hub_ex_sys.sdk\freeRTOS_UDP\app\queue" -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


