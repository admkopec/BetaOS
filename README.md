# BetaOS
The BetaOS in this version is not ready for use on real hardware. It probably won't work. It can also break your machine.<br />
For your and your hardware safety use it only as VM.<br />
To compile the source code you need Apple's/LLVM's clang and binutils (ld and ar) for target x86_64-elf for instructions how to build the cross compiler (ld and ar) visit this [page](http://wiki.osdev.org/GCC_Cross-Compiler).<br />
Now you can run the BetaOS from ISO.

###Changelog:
* Screen printing is working.
* Keyboard input is working.
* Commands are working.
* Added more commands.
* Added bootable ISO.
* Added build script.
* Improved printf function.
* Added time.
* Added kpanic function.
* Added reboot.
* Added whole new build mechanism.
* Improved commands management.
* Improved time command.
* Added new keyboard driver.
* Added GDT.
* Moved from i386 to x86_64 (this also adds paging).
