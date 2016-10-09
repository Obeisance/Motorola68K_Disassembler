Instructions:

1) Download "MotorolaDisassembler.exe", libgcc_s_dw2-1.dll and libstdc++-6.dll
	make sure these stay in the same folder.
2) Place the s record you wish to disassemble and the instruction address list 
	files in the same directory as the executable.
3) Run the executable and follow the on-screen prompts.

___________________________________________________________________________________

This daft disassembler was written with the intent of understanding 
the machine code in a Motorola 68k processor

Specifically, the processor of interest is a 68376 (based on a CPU32) 
with its extra serial and CAN functions. The disassembler should interpret
all commands for the 68k with the ability to interpret those up to the CPU32,
but not the 68020, 68030 or 68040.

The main file, "MotorolaDisassembler.exe" takes in a motorola s record text file 
(I include an example file called "6-16-06 ECU Read"), discards some data 
(I removed text and certain S-types like the header and termination type), 
and linearly steps through it, comparing bits to allowable machine instructions. 
Missing data is filled in with a '0xFF' byte (this does not get interpreted).
Every time certain types of branching commands are found, the code keeps 
track of the landing address to be sure that it does not interpret
instructions which would be bisected by the landing address of a branch command.

If the user suspects that there are instructions at particular addresses, write
those hex addresses on indivual lines into a file (I included an example file
called "HexAddressesOfInstructions - save for 6-16-06 ECU read.txt").
The program will attempt to disassemble instructions starting at those addresses and
will not allow other instructions to contain those addresses. The example file
shows the appropriate formatting.

Additionally, the code looks for the exception table if it is defined in a command
in the binary data. It tries to protect these addresses as well as the addresses
enumerated in the exception table from being bisected by interpreted commands.

This code has many faults:
1) it cannot distinguish data from commands
2) any incorrectly interpreted command affects all subsequent commands, 
	potentially making them erroneous

There is also a linker program which will label memory addresses within the 
assembly commands. I have included an example for how to 
create the text file with addresses and labels (ECU code from 6-06-2006 address list.txt)

-Obeisance