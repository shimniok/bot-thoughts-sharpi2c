A2Di2c
------

Want to edit the code, contribute, develop? Read on...

The code is written for AVR-GCC toolchains so if you're using IAR, sorry.


=== AVR Studio 4

1. Check out all the software into a directory (like A2Dfiles). You'll find a software directory. Under that, A2D, USI_I2C, and A2Di2c.
2. Open AVR Studio 4, use version 3.4.2 of the Atmel toolchain.
3. Create a new project, use the A2Di2c directory
4. Right click Source Files and Add Existing Source File(s)... and add the files
5. Right click on the project, select Edit Configuration Options...
6. Check Use External Makefile under General, type in Makefile as the name of the makefile

Now you should be able to click the Build Active tool and build the project using the external makefile which also builds USI_I2C and A2D libraries.


=== Eclipse

well, that's a long story which I'll get to later.


=== NetBeans

if I get around to it


=== Xcode

why don't you write up something for me, k?


=== GUI Editor + Command line

1. Check out all the software into a directory (like A2Dfiles). You'll find a software directory. Under that, A2D, USI_I2C, and A2Di2c.
2. Use your favorite editor (Notepad++, Geany, Xcode, GNU Emacs)
3. At the command prompt in the A2Di2c directory, type make
