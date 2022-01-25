qemu-system-i386 -D "build/logs/qemulog.txt" -d int,cpu_reset -no-reboot -no-shutdown -m 1024M -vga std -cdrom 'NOS.iso' -serial stdio -boot d -soundhw ac97 -rtc base=localtime -enable-kvm
