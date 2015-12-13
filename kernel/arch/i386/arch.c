#include <kernel/arch/arch.h>

void reboot() {
  unsigned char good = 0x02;
  while ((good & 0x02) != 0)
    good = inb(0x64);
  outb(0x64, 0xFE);
}

void shutdown() {
  printf("Will be added in near future\n");
}
