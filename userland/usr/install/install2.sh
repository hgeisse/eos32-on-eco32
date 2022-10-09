: PLEASE MODIFY THE FOLLOWING 4 LINES
PART_ROOT=4
SIZE_ROOT=64000
ROOTFS=/usr/install/rootfs

: INTERNAL
DISK_FILE=/dev/idedisk
DISK_MAJOR=0

echo "## Step 2.1: Create devices under /dev for the new partitions..."
mknod "${DISK_FILE}p${PART_ROOT}" b $DISK_MAJOR $PART_ROOT

echo "## Step 2.2: Compile the partition bootstrap..."
cd /usr/src/bin/mkpboot; make
cd /usr/install

echo "## Step 2.3: Format root partition..."
mkfs "${DISK_FILE}p${PART_ROOT}" ./root.fsys

echo "## Step 2.4: Mount new root partition..."
mkdir ./rootfs
mount "${DISK_FILE}p${PART_ROOT}" ./rootfs

echo "## Step 2.5: Compile kernel sources..."
cd /usr/src/kernel; make
cd /usr/install

echo "## Step 2.6: Copy kernel binaries to rootfs..."
mkdir "${ROOTFS}/boot"
cp /usr/src/kernel/src/eos32.map "${ROOTFS}/boot/eos32.map"
cp /usr/src/kernel/src/eos32.bin "${ROOTFS}/boot/eos32.bin"
sync