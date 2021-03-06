/**
* \brief Main source file for the AY1920_II_HW_05_PROJ_3
*
* In this project we test the accelerometer output
* capabilities by sampling with high resolution 
* all 3 axes at 100Hz.
* Data is sent as mm/s^2 (int).
*
* \author Alessio Ratti
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
#define ZYXDA 3                 // new data available
#define AXES_ACTIVE 3           // all axes active
#define READ_BYTES_PER_AXIS 2   // L & H regs
#define UART_BYTES_PER_AXIS 4   // bytes needed for transmission

// Conversion (still avoiding mg per level)
#define milli 1000          // conversion for mm
#define levels_per_g 511    // levels -> g
#define g_CONST 9.807       // gravitational constant

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
    uint8_t status_reg;                     // Store status register current value
    // Raw acceleration data in 8-bit
    uint8_t AccelerationData[AXES_ACTIVE*READ_BYTES_PER_AXIS];    
    int16_t OutAcc[AXES_ACTIVE];            // Right-aligned 16-bit acceleration data
    float acc_ms2[3];                       // Acceleration in m/s^2
    int32_t acc_mms2[3];                    // Acceleration in mm/s^2 (int)
    
    
    
    // UART
    uint8_t OutArray[AXES_ACTIVE*UART_BYTES_PER_AXIS+2];
    OutArray[0] = HEAD;
    OutArray[AXES_ACTIVE*UART_BYTES_PER_AXIS+1] = TAIL;
    
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
                                                     AXES_ACTIVE*READ_BYTES_PER_AXIS,
                                                     AccelerationData);
            if (error == NO_ERROR)
            {
                // Acceleration
                OutAcc[0] = ((int16_t) (AccelerationData[0] | (AccelerationData[1]<<8))>>4);
                OutAcc[1] = ((int16_t) (AccelerationData[2] | (AccelerationData[3]<<8))>>4);
                OutAcc[2] = ((int16_t) (AccelerationData[4] | (AccelerationData[5]<<8))>>4);
                
                // Convert to m/s^2
                acc_ms2[0] = (float) (OutAcc[0]*g_CONST/levels_per_g);
                acc_ms2[1] = (float) (OutAcc[1]*g_CONST/levels_per_g);
                acc_ms2[2] = (float) (OutAcc[2]*g_CONST/levels_per_g);
                
                // decimal places truncated for the printf
                //sprintf(message, "X: %+02.3f Y: %+02.3f Z: %+02.3f\r\n", acc_ms2[0], acc_ms2[1], acc_ms2[2]);
                //UART_Debug_PutString(message);
                
                // Cast float to int with 3 significant figures (mm/s^2)
                acc_mms2[0] = (float) (acc_ms2[0]*milli);
                acc_mms2[1] = (float) (acc_ms2[1]*milli);
                acc_mms2[2] = (float) (acc_ms2[2]*milli);
                
                // X-AXIS
                OutArray[1]  = (acc_mms2[0]     ) & 0xFF;   // LSB
                OutArray[2]  = (acc_mms2[0] >> 8) & 0xFF;
                OutArray[3]  = (acc_mms2[0] >>16) & 0xFF;
                OutArray[4]  = (acc_mms2[0] >>24) & 0xFF;   // MSB
                // Y-AXIS
                OutArray[5]  = (acc_mms2[1]     ) & 0xFF;   // LSB
                OutArray[6]  = (acc_mms2[1] >> 8) & 0xFF;
                OutArray[7]  = (acc_mms2[1] >>16) & 0xFF;
                OutArray[8]  = (acc_mms2[1] >>24) & 0xFF;   // MSB
                // Z-AXIS
                OutArray[9]  = (acc_mms2[2]     ) & 0xFF;   // LSB
                OutArray[10] = (acc_mms2[2] >> 8) & 0xFF;
                OutArray[11] = (acc_mms2[2] >>16) & 0xFF;
                OutArray[12] = (acc_mms2[2] >>24) & 0xFF;   // MSB
                
                UART_Debug_PutArray(OutArray, AXES_ACTIVE*UART_BYTES_PER_AXIS+2);
            }
            else
            {
                UART_Debug_PutString("Error occurred while acquiring acceleration data\r\n");   
            }
        }
    }
}

/* [] END OF FILE */


