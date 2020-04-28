<a href="https://www.cypress.com/"><img src="https://raw.githubusercontent.com/AlessioRatti/PSoC_5_Assignment/master/Cypress%20Logo%20Full%20Color_JPEG.jpg" title="Cypress Logo" alt="Cypress Logo"></a>

<!-- [![FVCproductions](https://www.cypress.com/file/349411/download)](https://www.cypress.com/) -->

# PSoC 5 Assignment

## PsoC I2C

**Objective:** 3 projects within a single workspace using PSoC Creator and the CY8CKIT-059 KIT along with the ST LIS3DH 3-Axis accelerometer breakout board by Adafruit.

<img src="https://raw.githubusercontent.com/AlessioRatti/PSoC_5_Assignment/master/my_board1.jpg" title="Prototype" alt="Prototype">
> Fig.1 Prototype

---

## 2. Context:

1. **Project 1**: Implementing multiread and multiwrite functions
    1. Start from the final example (PSoC_03_Complete.zip) regarding the temperature sensor on the breakout board
    1. Add implementation of multiread and multiwrite functions in the I2C_Interface.c source file
    1. Substitute the single register readings in the for(;;) loop with a multiread
    1. Plot Temperature Data using Bridge Control Panel

1. **Project 2**: Testing the accelerometer output capabilities
    1. Correctly set the control registers of the LIS3DH accelerometer to output 3 Axis accelerometer data in _Normal Mode at 100 Hz_ in the _±2.0 g FSR_.
    1. Read Output registers at a correct frequency (verify new Data is available using StatusReg information).
    1. Convert the 3 axial outputs of the Accelerometer to 3 right-justified 16-bit integers with the correct scaling (i.e. in mg).
    1. Correctly setup the UART Serial Communication: is the baud rate you chose sufficient to keep up with the data acquisition? Perform the correct calculations and add some comments regarding your choice in the TopDesign of the project.
    1. Send the values to be plotted to Bridge Control Panel.
    1. Save the Bridge Control Panel .ini and .iic files.

1. **Project 3**: Reading accelerometer output in m/s2
    1. Correctly set the control register to output a 3 Axis Signal in _High Resolution Mode_ at 100 Hz in the _±4.0 g FSR_.
    1. Read Output registers at a correct frequency (verify StatusReg). As for Project 2, carefully think about the possible options to read data at a constant rate.
    1. Convert the 3 axial outputs of the Accelerometer to a floating point in m/s2 units.
    1. Cast the floating point values to an int variable without losing information, keeping at least 3 decimals. What is a standard technique to perform such a task?
    1. Correctly setup the UART Serial Communication: is the baud rate you chose sufficient to keep up with the data acquisition? Perform the correct calculations and add some comments regarding your choice in the TopDesign of the project.
    1. Send the values to be plotted to Bridge Control Panel in m/s2.
    1. Save the Bridge Control Panel .ini and .iic files.

---

## Git and GitHub
While doing the assignment, you are asked to do source-code versioning following these instructions:
- For each new project you create, add a .gitignore file and a subfolder named “Bridge Control Panel”, where you’ll store the .ini and .iic files.
- While developing the projects, you are asked to make commits for any significant change of the source code (e.g., every time you program the PSoC and test a new feature).
- Start with Project 2: when you have finished writing the source code, do a git commit operation; you may do intermediate commits while doing the project (e.g., at every 2.2.x point). If everything works, git push to the remote repository on GitHub. For every commit operation, write a detailed commit message, so that people can understand what changed with respect to the previous version of the source code.
- Do the same for Project 3.
- At the end of the assignment, you should have a GitHub repository with the 3 projects. Generate a git log file by typing in the Git BASH the following command: ```git log > log.txt```

---

<img src="https://raw.githubusercontent.com/AlessioRatti/PSoC_5_Assignment/master/schematic.png" title="schematic" alt="schematic">
> Fig.2 Schematic