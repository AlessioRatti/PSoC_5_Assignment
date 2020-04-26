/**
* \brief Main source file for the AY1920_II_HW_05_PROJ_3 alt
*
* Further development of project 2.3
* Data is actually sent as float over UART [m/s^2]
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
#define ZYXDA 3             // new data available
#define AXES_ACTIVE 3       // all axes active
#define BYTES_PER_AXIS 2    // L & H regs

// Conversion (still avoiding mg per level)
#define levels_per_g 511    // levels -> g
#define g_CONST 9.807       // gravitational constant

// Custom PutArray function prototype
void UART_Debug_BCP(const uint8 string[], uint8 byteCount, uint8 head, uint8 tail);

int main(void)
{
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
    
    // union of float exactly matched with 4 uint8_t in the memory
    union {
        float axis;
        int32_t middle;
        uint8_t init[2];
        uint8_t bytes[4];
    } axes[3];
    
    // UART (no variables needed thanks to the structure)
    
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
                // From raw to signed integer
                OutAcc[0] = (int16_t) (AccelerationData[0] | (AccelerationData[1]<<8))>>4;
                OutAcc[1] = (int16_t) (AccelerationData[2] | (AccelerationData[3]<<8))>>4;
                OutAcc[2] = (int16_t) (AccelerationData[4] | (AccelerationData[5]<<8))>>4;
                
                // Convert to m/s^2 as float
                // from signed integer to float (automatic conversion into 4 blocks of uint8_t)
                axes[0].axis = (float) OutAcc[0]*g_CONST/levels_per_g;
                axes[1].axis = (float) OutAcc[1]*g_CONST/levels_per_g;
                axes[2].axis = (float) OutAcc[2]*g_CONST/levels_per_g;
                
                // Debug: decimal places truncated for the printf
                // sprintf(message, "X: %+03.3f Y: %+03.3f Z: %+03.3f\r\n", axes[0].axis, axes[1].axis, axes[2].axis);
                // UART_Debug_PutString(message);
                
                /* Thanks to the union structure there's no need to convert to uint8,
                 * moreover, the values are allocated consecutively in the memory,
                 * no need for further re-arrangement with the help of the custom
                 * function as well.
                */
                UART_Debug_BCP(&axes[0].bytes[0], AXES_ACTIVE*4, HEAD, TAIL);
            }
            else
            {
                UART_Debug_PutString("Error occurred while acquiring acceleration data\r\n");   
            }
        }
    }
}

// Custom version of the PutArray function
// it avoids having to deal with head and tail (I can avoid placing the data in a separated array)
void UART_Debug_BCP(const uint8 string[], uint8 byteCount, uint8 head, uint8 tail)
{
    uint8 bufIndex = 0u;

    /* If not Initialized then skip this function */
    if(UART_Debug_initVar != 0u)
    {
        UART_Debug_PutChar(head);   // tranmit head
        while(bufIndex < byteCount) // transmit body
        {
            UART_Debug_PutChar(string[bufIndex]);
            bufIndex++;
        }
        UART_Debug_PutChar(tail);   // transmit tail
    }
}

/* [] END OF FILE */
