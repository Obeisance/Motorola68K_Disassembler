This daft disassembler was written with the intent of understanding 
the machine code in a Motorola 68k processor

Specifically, the processor of interest is a 68376 (based on a CPU32) 
with its extra serial and CAN functions. The disassembler should interpret
all commands for the 68k with the ability to interpret those up to the CPU32,
but not the 68020, 68030 or 68040.

The main file, "MotorolaDisassembler.cpp" takes in a motorola s record text file 
(the compiled version I include looks for a file called "6-16-06 ECU Read"), 
discards some data (I think I removed text and certain S-types like the header
and termination type), and reads in the s record file and linearly steps 
through it, comparing bits to allowable machine instructions. 
Every time certain types of branching commands are found,
the code keeps track of the landing address to be sure that it does not interpret
instructions which would be bisected by the landing address of a branch command.

If the user suspects that there are instructions at particular addresses, write
those hex addresses on indivual lines into a file called "HexAddressesOfInstructions.txt".
The program will attempt to disassemble instructions starting at those addresses and
will not allow other instructions to contain those addresses.

Additionally, the code looks for the exception table if it is defined in a command
in the s record.

This code has many faults:
1) it cannot distinguish data from commands
2) any incorrectly interpreted command affects all subsequent commands, 
	potentially making them erroneous

An additional file "convertMC68376.cpp" takes in the assemblyVersion.txt file that
is output by the disassembler and tags the register addresses. I have assumed that
the control registers for the additional functions of this chip are in high
addresses ($Fxxxxx).

-Obeisance