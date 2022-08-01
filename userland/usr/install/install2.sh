:!/bin/sh

: PLEASE MODIFY THE FOLLOWING 4 LINES
PART_ROOT=4
PART_SWAP=5
PART_USR=6
PART_HOME=7

: INTERNAL
DISK_FILE=/dev/idedisk
DISK_MAJOR=0


echo "## Step 2.1: Write new partition table to disk..."
mkpart $DISK_FILE ./disk.part
echo "## Step 2.2: Create devices under /dev for the new partitions..."
mknod "${DISK_FILE}p${PART_ROOT}" b $DISK_MAJOR $PART_ROOT
mknod "${DISK_FILE}p${PART_SWAP}" b $DISK_MAJOR $PART_SWAP
mknod "${DISK_FILE}p${PART_USR}" b $DISK_MAJOR $PART_USR
mknod "${DISK_FILE}p${PART_HOME}" b $DISK_MAJOR $PART_HOME
