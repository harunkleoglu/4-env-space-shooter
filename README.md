# Space Invaders in 4 Environments

This project contains a simple space shooter game developed in 4 different Environments. The game aims to control a spaceship to avoid falling meteors and earn as many points as possible.

## Environments

### Mkeykernel 
- Simple console environment which runs on Linux based systems.
\
https://github.com/arjun024/mkeykernel

### Basekernel
- Simple graphic environment which also runs on Linux based systems.
\
https://github.com/arjun024/mkeykernel

### MinGW
- Minimalist GNU for Windows platform.
\
https://sourceforge.net/projects/mingw/

### WinBGI
- Borland Graphics Interface for Windows platform.
\
https://github.com/nirin/WinBGI

# Getting Started

This section will guide you on how to set up and run the Space Shooter game in each environment.

## Requirements:

- C & C++ compiler (GCC recommended)
- NASM (Netwide Assembler) for assembly language programming
- QEMU for emulating virtual machines
- graphics.h library for graphics programming (specifically for WinBGI on Windows)
- pseudo_random.h library (custom-written) for pseudo-random number generation


## Environment Specific Requirements and Instructions:

1. **Mkeykernel:**

   Mkeykernel is a lightweight kernel environment specifically designed for educational purposes.
   Follow the instructions on the Mkeykernel repository to install and set up the environment on your Linux system: [Mkeykernel Repository](https://github.com/arjun024/mkeykernel)
   The Space Invaders code for Mkeykernel might require minor modifications to adapt to the library functions it provides. Refer to the code itself and potentially the Mkeykernel documentation for guidance.

2. **Basekernel:**

   Similar to Mkeykernel, Basekernel is another lightweight graphics environment for Linux.
   Follow the installation instructions on the Basekernel repository: [Basekernel Repository](https://github.com/dthain/basekernel)
   The Space Invaders code for Basekernel might also require adjustments to work with the Basekernel graphics library.

3. **MinGW:**

   MinGW provides a development toolkit to create native Windows applications using GCC compiler.
   Download and install MinGW from [MinGW Website](https://www.mingw-w64.org/).
   Once installed, you can compile the Space Invaders code using a command like:
   ```bash
   g++ SpaceInvaders.cpp -o SpaceInvaders -lmingw32 -lgdi32

## WinBGI:

WinBGI is a popular graphics library specifically designed for Windows development. Download the WinBGI library from a trusted source and follow the installation instructions. The provided code in this repository (assuming it's written for WinBGI) should compile and run directly after including the necessary WinBGI header files (graphics.h). You might need to link the graphics.lib library during compilation. Refer to your compiler's documentation for specific linking instructions.

## Running the Game:

Once you've successfully compiled the code for your desired environment, follow the specific instructions below:

- **MinGW and WinBGI (Windows):** You can compile the code directly using the GCC compiler provided by MinGW and execute the compiled program. However, for WinBGI, additional configuration might be required in your compiler settings to properly link the WinBGI library and header files. Ensure that you include the necessary WinBGI header files (such as `graphics.h`) and link the `graphics.lib` library during compilation. Refer to your compiler's documentation for specific instructions on how to configure WinBGI for your environment.


- **Linux Environments (Mkeykernel and Basekernel):** The game code is written to run directly within the kernel rather than as a user program. Therefore, to run the game on Linux environments such as Mkeykernel and Basekernel, you'll need to use a virtual machine like QEMU. Ensure you have QEMU set up on your system and follow the instructions provided with Mkeykernel or Basekernel to boot the kernel image containing the Space Shooter game.

## Contributing:

We welcome contributions to this project! If you'd like to add a new environment or improve the existing code, feel free to fork the repository and submit a pull request.
