DISKNAME="disk.img"

sh scripts/multiboot-check.sh $DISKNAME
if [ $? -eq 1 ]; then
	echo "Error:" $DISKNAME "must be multiboot compliant"
	return 1
fi

kvm	-m 4G \
	-curses \
	-cpu kvm32 \
	-drive format=raw,file=$DISKNAME,media=disk \
	-gdb tcp::1234