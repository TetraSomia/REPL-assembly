# REPL-assembly
Command-line tool to experiment with x86_64 assembly on GNU/Linux.

## Dependencies
* Make
* GCC or Clang
* NASM and objdump
* libreadline
* libdl

## Build
```
make
```

## Usage
```
./repl-asm
```

## How to use it ?
The goal of this project is to run custom assembly instructions in a REPL (Read/Eval/Print/Loop) environment with a set of control/configuration commands.
### Commands
The ``help`` command will give you more information about those commands and how to use them.
#### Code edition
* The ``inst `` command can be used to create, edit and remove instructions.
* The ``label`` command allows you to create labels into the code to be able to jump at different location of the code easily.
* The ``unit`` command allows you to create "code units". A code unit can be seen as a source file. Labels from one code unit are locals, and are not accessible by another unit, the only external symbol is the name of the unit, that can be used to call a unit from another one.
Most commands affect and refer to the "current" code unit, so this command also permits to switch to another code unit. Hitting a breakpoint in a different code unit will automatically set it as the current one.
#### Execution control
* The ``run`` command will start the execution of the "main" code unit.
* The ``breakpoint`` command allows to set and reset breakpoints on different instructions to stop the execution flow and to get back to the command interpreter.
* The ``cont`` command resumes a stopped execution.
* The ``step`` command executes one user instruction.
#### Context exploration
* The ``disas`` command shows the assembly code of a particular unit with additional information such as the addresses of instructions, the RIP and breakpoint locations, and the corresponding symbol name of operand addresses.
* The ``print`` command prints the content of a register or at a particular address, both can also be dereferenced multiple times. Formats (string, hexadecimal or decimal values), word size (1/2/4/8) and the number of iterations can also be provided.
* The ``stack`` command is used to explore the stack content using different mode, based on RSP and RBP.
#### Edition and Declaration
* The ``set`` command works a lot like ``print`` but allows to set a register or the content of an address to a particular value.
* The ``array`` command allows declaration of arrays. They can be strings or raw bytes. They can be referenced into the code using their name.
* The ``dl`` command allows the importation of functions from external dynamic libraries. Once loaded, they can be called from the code.
### Symbols
Three types of symbols can be used within the code:
* Code units
* Arrays
* Dynamic symbols loaded with ``dl``

All of them have a name associated, and a ``$`` operator must be inserted before their name to be interpreted correctly. Labels do not count as symbols as they are directly interpreted by the external assembler (NASM). 
### Crash handling
Three signals are caught when raised from the assembly code or within critical section of the REPL (dereferencing custom addresses): SIGSEGV, SIGILL and SIGFPE. When caught, the exception will be described and the execution of the assembly code will be aborted.
If the execution is stuck into an assembly infinite loop, SIGINT is also handled, so you can CTRL+C to stop the execution.
The stack allocated to the user assembly code can be freely corrupted without risking to crash the REPL.
### Crash possibility: warning
Because of the user assembly code being run within the same thread than the REPL, modification of REPL-owned memory, or calling some system calls can lead to the REPL crash.
