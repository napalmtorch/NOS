# vars
cmsgl="GCC   :   '"
nmsgl="NASM  :   '"
cmsgm="' -> '"
cmsge="'"
outdir="bin/objs/"
cdir="src/kernel/"
inf=""
outf=""

mkdir "bin/objs/userland"
mkdir "bin/objs/services/"

echo "Compiling userland..."

# ----------------------------------------------- LIB -------------------------------------------------------------------
cdir="src/lib/"
outdir="bin/objs/userland/"
i686-elf-gcc -Iinclude -c "$cdir/nos.c" -o "$outdir/nos.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums


cdir="src/lib/wrappers"
files=$(find $cdir -name "*.c" -print)
for file in $files; do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-gcc -Iinclude -c "$file" -o "$outdir$outf" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
    echo "$cmsgl$inf$cmsgm$outdir$outf$cmsge"
done

echo "Successfully compiled 'libuser.so'"

# ----------------------------------------------- SERVICES --------------------------------------------------------------
#files=$(find $cdir -name "*.c" -print)
#for file in $files; do
#    inf="$(basename $file)"
#    outf="$(echo ${inf%.*}.o)"
#    i686-elf-gcc -Iinclude -c "$file" -o "$outdir$outf" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
#    echo "$cmsgl$inf$cmsgm$outdir$outf$cmsge"
#done

mkdir "bin/objs/services/vesa/"
i686-elf-gcc -Iinclude -c "src/services/vesa/vesa.c" -o "bin/objs/services/vesa/vesa.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums
i686-elf-gcc -Iinclude -c "src/services/vesa/main.c" -o "bin/objs/services/vesa/main.o" -w -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions -fshort-enums

cd 'bin/objs/userland/'
i686-elf-ld -T '../../../build/linker_user.ld' -o '../services/vesa.bin' "../services/vesa/main.o" "../services/vesa/vesa.o" *.o -O0
cd '../../../'