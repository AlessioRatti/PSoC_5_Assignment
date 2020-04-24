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

// Set normal mode to the accelerator (50Hz)
#define LIS3DH_NORMAL_MODE_CTRL_REG1 0x47
// Enable temperature sensor and ADC
#define LIS3DH_TEMP_CFG_REG_ACTIVE 0xC0
// Enable BDU option
#define LIS3DH_CTRL_REG4_BDU_ACTIVE 0x80

// Number of bytes needed to store temp
#define LENGTH_T 2

// Bridge Control Panel
#define HEAD 0xA0
#define TAIL 0xC0

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2C_Peripheral_Start();
    UART_Debug_Start();
    
    CyDelay(5); //"The boot procedure is complete about 5 milliseconds after device power-up."
    
    // String to print out messages on the UART
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
     /*          TEMP_CFG_REG config           */
     /******************************************/
    
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_TEMP_CFG_REG,
                                         LIS3DH_TEMP_CFG_REG_ACTIVE);
    
    if (error == NO_ERROR)
    {
        UART_Debug_PutString("TEMPERATURE CONFIG REGISTER successfully written!\r\n"); 
    }
    else
    {
        UART_Debug_PutString("ERROR occurred during I2C comm to read temperature config register\r\n");   
    }
    
    /******************************************/
    /*            CTRL_REG4 config            */
    /******************************************/
    
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         LIS3DH_CTRL_REG4_BDU_ACTIVE);
    
    if (error == NO_ERROR)
    {
        UART_Debug_PutString("CONTROL REGISTER 4 successfully written\r\n"); 
    }
    else
    {
        UART_Debug_PutString("ERROR occurred during I2C comm to read control register4\r\n");   
    }
    
    int16_t OutTemp;
    uint8_t OutArray[4]; 
    uint8_t TemperatureData[2];
    
    OutArray[0] = HEAD;
    OutArray[3] = TAIL;
    
    for(;;)
    {
        CyDelay(200);
        
        error = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                 LIS3DH_OUT_ADC_3L,
                                                 LENGTH_T,
                                                 TemperatureData);
        
        if(error == NO_ERROR)
        {
            OutTemp = (int16)(TemperatureData[0] | (TemperatureData[1]<<8))>>6;
            // Followed ST config (LSB first)
            OutArray[1] = (uint8_t)(OutTemp & 0xFF);    // LSB
            OutArray[2] = (uint8_t)(OutTemp >> 8);      // MSB
            UART_Debug_PutArray(OutArray, 4);
        }
    }
}

/* [] END OF FILE */
