# Makefile to the ucOS_II for x86 Protected Mode.
#
# Copyright(C) 2009, Li Yuan(liyuan@ss.buaa.edu.cn)
#

NASM = nasm
GAS = /opt/rtems-4.9/bin/i386-rtems4.9-as
#CC = /opt/crosstool/gcc-4.1.0-glibc-2.3.6/i686-unknown-linux-gnu/bin/i686-unknown-linux-gnu-gcc
CC = /opt/rtems-4.9/bin/i386-rtems4.9-gcc
#CC = gcc
AR = /opt/rtems-4.9/bin/i386-rtems4.9-ar
LD = /opt/rtems-4.9/bin/i386-rtems4.9-ld

NASM_FLAGS= -f elf
CFLAGS= -Wall -I./include -ffreestanding -funsigned-char -fno-builtin -Wparentheses -mtune=i386
# 重要：编译内核时少加那些会改变编译结果的编译参数，否则会出现莫名其妙的错误。不要加优化参数！
LDFLAGS= -s -Ttext 0x100000 --entry=start
                   
IMG = kernel
OBJS = boot.o test.o motorTask.o pm511p_IO.o keyboardTask.o pm511p.o closeLoop.o \
        display.o closeLoopISR.o scanTask.o commTask.o freqScanTask.o dds_da.o \
        stepResponseTask.o forceCurveTask.o scannerResponseTask.o
LIB = ucos_ii.a

INCLUDES = ./include/ucos_ii.h ./include/os_cfg.h ./include/os_cpu.h ./display.h
          

all: $(IMG)
	@echo '**kernel created, enjoy it!**'
$(IMG): $(OBJS) $(LIB)
	$(LD) $(LDFLAGS) $(OBJS) $(LIB) -o kernel -Map link.map
# 注意：boot.o 一定要放在最前面。否则 grub 就不能引导	
# 报告错误为：Error 13: Invalid or unsupported executable format


##########################################
## main
##########################################
test.o:./test1.c
	$(CC) $(CFLAGS) -o $@ -c $<
motorTask.o:./motorTask.c
	$(CC) $(CFLAGS) -o $@ -c $<
scanTask.o:./scanTask2.c
	$(CC) $(CFLAGS) -o $@ -c $<
keyboardTask.o: keyboardTask.c
	$(CC) $(CFLAGS) -o $@ -c $<
freqScanTask.o: freqScanTask.c
	$(CC) $(CFLAGS) -o $@ -c $<
stepResponseTask.o:./stepResponseTask.c
	$(CC) $(CFLAGS) -o $@ -c $<
scannerResponseTask.o:./scannerResponseTask.c
	$(CC) $(CFLAGS) -o $@ -c $<
forceCurveTask.o:./forceCurveTask.c
	$(CC) $(CFLAGS) -o $@ -c $<
commTask.o :commTask.c
	$(CC) $(CFLAGS) -o $@ -c $<
pm511p_IO.o: pm511p_IO.c
	$(CC) $(CFLAGS) -o $@ -c $<
pm511p.o: pm511p.c
	$(CC) $(CFLAGS) -o $@ -c $<
dds_da.o: dds_da.c
	$(CC) $(CFLAGS) -o $@ -c $<
closeLoop.o: closeLoop.c
	$(CC) $(CFLAGS) -o $@ -c $<
closeLoopISR.o:./closeLoopISR.asm
	$(NASM) $(NASM_FLAGS) -o $@ $<
display.o:./display.c
	$(CC) $(CFLAGS) -o $@ -c $<
boot.o:./boot.S
	$(CC) $(CFLAGS) -o $@ -c $<


install :  $(IMG)
	mount ./floppy.img ./mnt -o loop
	cp ./kernel ./mnt/
	umount ./mnt


clean:
	rm *.o $(TARGETS)
