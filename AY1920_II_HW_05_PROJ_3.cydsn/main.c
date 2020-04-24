/**
* \brief Main source file for the I2C-Master project.
*
* In this project we set up a I2C master device with
* to understand the I2C protocol and communicate with a
* a I2C Slave device (LIS3DH Accelerometer).
*
* \author Gabriele Belotti
* \date , 2020
*/

// Include required header files
#include "I2C_Interface.h"
#include "project.h"
#include "stdio.h"
#include "Regs.h"

// Set normal mode to the accelerator (3-axes, 100Hz)
#define LIS3DH_NORMAL_MODE_CTRL_REG1 0x57
// Set +-4g FSR and high resolution
#define LIS3DH_CTRL_REG4_HR 0x18

// Bridge Control Panel
#define HEAD 0xA0
#define TAIL 0xC0

// Acc defines
#define ZYXDA 3             // new data available
#define AXES_ACTIVE 3       // all axes active
#define BYTES_PER_AXIS 2    // L & H regs

int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    I2C_Peripheral_Start();
    UART_Debug_Start();
    
    CyDelay(5); //"The boot procedure is complete about 5 milliseconds after device power-up."
    
    // Buffer string to print out messages on the UART
    char message[50];

    // Check which devices are present on the I2C bus
    for (int i = 0 ; i < 128; i++)
    {
        if (I2C_Peripheral_IsDeviceConnected(i))
        {
            // print out the address is hex format
            sprintf(message, "Device 0x%02X is connected\r\n", i);
            UART_Debug_PutString(message); 
        }
        
    }
    
    // Support variable for error flags
    uint8_t error;
    
    /******************************************/
    /*            CTRL_REG1 config            */
    /******************************************/
    
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG1,
                                         LIS3DH_NORMAL_MODE_CTRL_REG1);

    if (error == NO_ERROR)
    {
        UART_Debug_PutString("CONTROL REGISTER 1 successfully written\r\n"); 
    }
    else
    {
        UART_Debug_PutString("ERROR occurred during I2C comm to set control register 1\r\n");   
    }
    
    /******************************************/
    /*            CTRL_REG4 config            */
    /******************************************/
    
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         LIS3DH_CTRL_REG4_HR);

    if (error == NO_ERROR)
    {
        UART_Debug_PutString("CONTROL REGISTER 4 successfully written\r\n"); 
    }
    else
    {
        UART_Debug_PutString("ERROR occurred during I2C comm to set control register 4\r\n");   
    }
    
    /* Private variables ---------------------------------------------------------*/
    // ACCELERATION
    uint8_t status_reg;                         // Store status register current value
    uint8_t AccelerationData[AXES_ACTIVE*2];    // Raw acceleration data in 8-bit
    int16_t OutAcc[AXES_ACTIVE];                // Right-aligned 16-bit acceleration data in mg
    
    // UART
    uint8_t OutArray[AXES_ACTIVE*BYTES_PER_AXIS+2];
    OutArray[0] = HEAD;
    OutArray[AXES_ACTIVE*BYTES_PER_AXIS+1] = TAIL;
    
    for(;;)
    {
        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                            LIS3DH_STATUS_REG,
                                            &status_reg);
        if(status_reg & (1<<ZYXDA))
        {
            // Read aceleration
            error = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                     OUT_X_L,
                                                     AXES_ACTIVE*BYTES_PER_AXIS,
                                                     AccelerationData);
            if (error == NO_ERROR)
            {
                // Acceleration
                OutAcc[0] = ((int16_t) (AccelerationData[0] | (AccelerationData[1]<<8))>>4);
                OutAcc[1] = ((int16_t) (AccelerationData[2] | (AccelerationData[3]<<8))>>4);
                OutAcc[2] = ((int16_t) (AccelerationData[4] | (AccelerationData[5]<<8))>>4);
                
                // X-AXIS
                OutArray[1] = OutAcc[0] & 0xFF;     // MSB
                OutArray[2] = OutAcc[0] >>8;        // LSB
                // Y-AXIS
                OutArray[3] = OutAcc[1] & 0xFF;     // MSB
                OutArray[4] = OutAcc[1] >>8;        // LSB
                // Z-AXIS
                OutArray[5] = OutAcc[2] & 0xFF;     // MSB
                OutArray[6] = OutAcc[2] >>8;        // LSB
                
                UART_Debug_PutArray(OutArray, AXES_ACTIVE*BYTES_PER_AXIS+2);
            }
            else
            {
                UART_Debug_PutString("Error occurred while acquiring acceleration data\r\n");   
            }
        }
    }
}

/* [] END OF FILE */
