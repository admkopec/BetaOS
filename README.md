# BetaOS
The BetaOS in this version is not yet officially ready for use on real hardware. It shouldn't break your machine, althoug use with caution.<br />
For your and your hardware safety use it only as VM.<br />
To compile the source code you need Apple's/LLVM's clang. (No need for GNU, because we're using MACH-O now)<br />
Now installation got even easier with vmdk disk for VMware VMs and USB directory which contents you only need to copy to your USB to make it bootable.

###Changelog:
###Version 0.2:
* Screen printing is working.
###Verion 0.3:
* Keyboard input is working.
* Commands are working.
###Version 0.5:
* Added more commands.
* Added bootable ISO.
* Added build script.
* Improved printf function.
* Added time.
* Added kpanic function.
###Version 0.7:
* Added reboot.
###Version 0.9:
* Added whole new build mechanism.
* Improved commands management.
* Improved time command.
###Version 0.10:
* Added new keyboard driver.
###Version 0.11:
* Added GDT.
* Moved from i386 to x86_64 (this also adds paging).
###Version 0.25:
* Moved from ELF executable to MACH-O.
* Moved from legacy BIOS to EFI (for now using Apple's EFI bootloader and because of this if you don't have a Mac then you have to use some kind of bootloader (ex. Clover)).
* Changed build mechanism.
* Added Pixel mode (EFI's VESA compatible video device).
* Added advanced Paging. For now still using previous Paging, a lot of issues with the new one.
