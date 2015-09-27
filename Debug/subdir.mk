################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DataStructures.c \
../FileWriter.c \
../Flow.c \
../Minimax.c \
../gui.c 

OBJS += \
./DataStructures.o \
./FileWriter.o \
./Flow.o \
./Minimax.o \
./gui.o 

C_DEPS += \
./DataStructures.d \
./FileWriter.d \
./Flow.d \
./Minimax.d \
./gui.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


