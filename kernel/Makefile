DEFAULT_HOST!=../default-host.sh
HOST?=DEFAULT_HOST
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)

CFLAGS?=
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -std=gnu11 -ffreestanding -fno-builtin -fno-stack-protector
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
ASMFLAGS:=$(ASMFLAGS)
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc

ARCHDIR=arch/$(HOSTARCH)

BOOTLOADERDIR=$(ARCHDIR)/boot
CPUDIR=$(ARCHDIR)/cpu
DRIVERSDIR=$(ARCHDIR)/drivers
MEMORYDIR=$(ARCHDIR)/memory

KERNELDIR=kernel
SYSCALLSDIR=syscalls
DEBUGDIR=debug

include $(BOOTLOADERDIR)/make.config
include $(KERNELDIR)/make.config
include $(DRIVERSDIR)/make.config
include $(CPUDIR)/make.config
include $(DEBUGDIR)/make.config
include $(SYSCALLSDIR)/make.config
include $(MEMORYDIR)/make.config

OBJS=\
$(BOOTLOADERDIR)/crti.o \
$(BOOTLOADERDIR)/crtbegin.o \
$(BOOTLOADERDIR)/crtend.o \
$(BOOTLOADERDIR)/crtn.o \
$(BOOT_OBJS) \
$(KERNEL_OBJS) \
$(DRIVERS_OBJS) \
$(CPU_OBJS) \
$(DEBUG_OBJS) \
$(SYSCALLS_OBJS) \
$(MEMORY_OBJS) \

LINK_LIST=\
$(LDFLAGS) \
$(BOOT_OBJS) \
$(BOOTLOADERDIR)/crti.o \
$(BOOTLOADERDIR)/crtbegin.o \
$(DEBUG_OBJS) \
$(MEMORY_OBJS) \
$(SYSCALLS_OBJS) \
$(KERNEL_OBJS) \
$(DRIVERS_OBJS) \
$(CPU_OBJS) \
$(BOOTLOADERDIR)/crtend.o \
$(BOOTLOADERDIR)/crtn.o \
$(LIBS) \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S .asm

all: ipos.kernel

ipos.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot ipos.kernel

$(BOOTLOADERDIR)/crtbegin.o $(BOOTLOADERDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.asm.o:
	$(ASM) $< $(ASMFLAGS) -o $@

clean:
	rm -f ipos.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: ipos.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp ipos.kernel $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
