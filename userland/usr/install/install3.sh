: PLEASE MODIFY THE FOLLOWING 4 LINES
PART_ROOT=4
SIZE_ROOT=64000
ROOTFS=/usr/install/rootfs

: INTERNAL
DISK_FILE=/dev/idedisk
DISK_MAJOR=0

echo "## Step 3.1: Create devices under /dev for the new partitions..."
mknod "${DISK_FILE}p${PART_ROOT}" b $DISK_MAJOR $PART_ROOT

echo "## Step 3.2: Compile the partition bootstrap..."
cd /usr/src/bin/mkpboot; make
cd /usr/install

echo "## Step 3.3: Create root partition..."
mkfs "${DISK_FILE}p${PART_ROOT}" ./root.fsys

echo "## Step 3.4: Mount new root partition..."
mkdir ./rootfs
mount "${DISK_FILE}p${PART_ROOT}" ./rootfs

echo "## Step 3.5: Generate welcome message..."
echo "This version of EOS32 was installed on: " > ./rootfs/WELCOME
date >> ./rootfs/WELCOME