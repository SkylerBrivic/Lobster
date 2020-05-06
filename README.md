# Lobster
An Interpreted Multi-Threaded Programming Language

To create the Lobster Compiler on Mac or Linux, go into the directory containing the project and type in the following command:

g++ -std=c++11 -o LobsterC CodeGenerator.cpp lexicalAnalyzer.cpp

To create the Lobster Virtual Machine on Mac or Linux, go into the directory containing the project and type in the following command:

g++ -std=c++11 -o Lobster runTimeObject.cpp

After you have built the compiler and virtual machine, you can compile a file of code named SampleFileName using the following command:

./LobsterC SampleFileName

This will in turn output a compiled version of your program, which will be stored in SampleFileName.Lobster.

To run a program named SampleFileName.Lobster on the virtual machine, you can type in the following command:

./Lobster SampleFileName.Lobster
