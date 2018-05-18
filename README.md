# BetaOS
The BetaOS in this version is not yet officially ready for use on real hardware. It shouldn't break your machine, although use with caution.<br />
For your and your hardware safety use it only as VM.<br />
To compile the source code you need Apple's Xcode 9.3 or later.<br />
Now installation got even easier with dmg disk for VMs (or you can restore it to USB) and USB directory which contents you only need to copy to your USB to make it bootable, proper blessing is required on Mac.<br />
PC users should use Clover bootloader to boot the MACH-O binary through included boot.efi(Apple).

## Changelog:
### Version 0.41:
* Added APIC support.
* Added an AML Parser (Needs a few fixes).
* Added a PNG Reader Class which needs more work to read files properly.
* Made a few changes to Objective C Runtime.
* Migrated to Swift 4.1.
* And a lot of minor changes and improvements.

### Version 0.40.2:
* Improved data recieving from disk.
* Added an early work in progress NVMe module.

### Version 0.40.1:
* Small runtime fix for proper class manipulation on release builds.

### Version 0.40:
* Added better runtime support with simple class management and retaining.
* Added SATA in AHCI mode (needs a little bit more refining but it's working).
* Added FAT32 filesystem support with reading and writing (writing needs more refining and needs to support LFN).
* Added SVGA II driver.
* And lots of overall improvements.

### Version 0.36:
* Fixed broken time command.
* Translated more code into Swift.
* Added automatic memory addresses management.
* Added improved high-speed memcpy for use in framebuffer writes.
* Expanded ACPI table support (but only on EFI systems).
* Implemented simple power saving methods (waiting for IRQs, made a screen refresh at steady 60Hz)
* Started using built-in POSIX header for improved compatibility.
* Added statically-linked Swift frameworks to offload some code form main module. (Needs more troubleshooting, though)
* Added nice, colorful Logging for code written in Swift.

### Version 0.35:
* Added Swift language support for Kernel development using Xcode 9.0 or later. (More code will be translated into it in the future.)
* Added full Kaby Lake support.
* Added support for Mac's ACPI tables.

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
