cp "build/dumps/kernel.dump" "external/ramdisk/kernel.dump"

cp "bin/objs/services/vesa.bin" "external/ramdisk/vesa.bin"

./ramfsmgr build/makeramdisk