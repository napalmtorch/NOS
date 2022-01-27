# vars
cmsgl="GCC   :   '"
nmsgl="NASM  :   '"
cmsgm="' -> '"
cmsge="'"
outdir="bin/objs/"
cdir="src/kernel/"
inf=""
outf=""

# Clear the console
clear

# Setup build directory
rm -r "bin"
mkdir "bin"
mkdir "bin/objs"

# boot.asm
cdir="src/asm/" outdir="bin/" inf="boot.asm" outf="boot.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# irqs.asm
cdir="src/asm/" outdir="bin/objs/" inf="irqs.asm" outf="irqs.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# registers.asm
cdir="src/asm/" outdir="bin/objs/" inf="registers.asm" outf="registers.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# tswitch.asm
cdir="src/asm/" outdir="bin/objs/" inf="tswitch.asm" outf="tswitch.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# ----------------------------------------------- KERNEL --------------------------------------------------------------
cdir="src/kernel/"
outdir="bin/objs/"
files=$(find $cdir -name "*.c" -print)

for file in $files; do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-gcc -Iinclude -c "$file" -o "$outdir$outf" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
    echo "$cmsgl$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- LIB --------------------------------------------------------------
cdir="src/lib/"
outdir="bin/objs/"
i686-elf-gcc -Iinclude -c "$cdir/ctype.c" -o "$outdir/ctype.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
i686-elf-gcc -Iinclude -c "$cdir/stddef.c" -o "$outdir/stddef.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
i686-elf-gcc -Iinclude -c "$cdir/stdio.c" -o "$outdir/stdio.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
i686-elf-gcc -Iinclude -c "$cdir/stdlib.c" -o "$outdir/stdlib.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
i686-elf-gcc -Iinclude -c "$cdir/string.c" -o "$outdir/string.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
i686-elf-gcc -Iinclude -c "$cdir/time.c" -o "$outdir/time.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
i686-elf-gcc -Iinclude -c "$cdir/graphics/font.c" -o "$outdir/font.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
echo "Compiled kernel libraries"

# Link all files
cd 'bin/objs'
i686-elf-ld -T '../../build/linker.ld' -o '../kernel.bin' "../boot.o" *.o -O0
cd '../../'

objdump -t "bin/kernel.bin" > "build/dumps/kernel.objdump"

./ndumper build/dumps/kernel.objdump build/dumps/kernel.dump