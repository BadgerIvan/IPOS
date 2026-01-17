# .gdbinit
set disassembly-flavor intel
set architecture i386

define connect
    target remote localhost:1234
    symbol-file isodir/boot/myos.kernel
end

define pdump
    x/16wx 0x00000000
end

define ptinfo
    printf "CR0: 0x%x\n", $cr0
    printf "CR3: 0x%x\n", $cr3
end

define stack
    x/20wx $esp
end

connect