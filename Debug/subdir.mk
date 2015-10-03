################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DataStructures.c \
../FileWriter.c \
../Flow.c \
../Gui.c \
../Minimax.c 

O_SRCS += \
../DataStructures.o \
../FileWriter.o \
../Flow.o \
../Gui.o \
../Minimax.o 

OBJS += \
./DataStructures.o \
./FileWriter.o \
./Flow.o \
./Gui.o \
./Minimax.o 

C_DEPS += \
./DataStructures.d \
./FileWriter.d \
./Flow.d \
./Gui.d \
./Minimax.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


