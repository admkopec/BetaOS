# BetaOS
The BetaOS in this version is not yet officially ready for use on real hardware. It shouldn't break your machine, although use with caution.<br />
For your and your hardware safety use it only as VM.<br />
To compile the source code you need Apple's Xcode.<br />
Now installation got even easier with dmg disk for VMs (or you can restore it to USB) and USB directory which contents you only need to copy to your USB to make it bootable.<br />
PC users should use Clover bootloader to boot the MACH-O binary through included boot.efi(Apple).

## Changelog:
### Version 0.31.1:
* Added Screen Caching.
* Added USB Ports detection.
* Fixed UNIX Epoch time.
* Added new auto-incrementing build number.
* Added SSE instructions on supported CPUs.
* Added send_packet through Intel E1000 NICs and an abstract layer.
* Made PS2 Controller compatible with Controller API.

### Version 0.31:
* Added Libc++.
* Added simple allocation methods.
* Added Interrupts.
* Change the way Modules are loaded and operate.
* Made a few changes in USB, SATA and Ethernet Modules.

### Version 0.30:
* Using advanced Paging.
* Added IDT.
* Testing USB and new PCI Modules initialization.
* Added Unix absolute time

### Version 0.26:
* Added ACPI.
* Shut down is now working.

### Version 0.25:
* Moved from ELF executable to MACH-O.
* Moved from legacy BIOS to EFI (for now using Apple's EFI bootloader and because of this if you don't have a Mac then you have to use some kind of bootloader (ex. Clover)).
* Changed build mechanism.
* Added Pixel mode (EFI's VESA compatible video device).
* Added advanced Paging. For now still using previous Paging, a lot of issues with the new one.

### Version 0.11:
* Added GDT.
* Moved from i386 to x86_64 (this also adds paging).

### Version 0.10:
* Added new keyboard driver.

### Version 0.9:
* Added whole new build mechanism.
* Improved commands management.
* Improved time command.

### Version 0.7:
* Added reboot.

### Version 0.5:
* Added more commands.
* Added bootable ISO.
* Added build script.
* Improved printf function.
* Added time.
* Added kpanic function.

### Verion 0.3:
* Keyboard input is working.
* Commands are working.

### Version 0.2:
* Screen printing is working.
