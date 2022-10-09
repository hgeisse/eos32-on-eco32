ROOTFS=/usr/install/rootfs

echo "This script prepares the minimal needed userland."
echo "## Step 3.1: Compile the init binary"
cd /usr/src/bin/init; make
cd /usr/install

echo "## Step 3.2: Compile getty"
cd /usr/src/bin/getty; make
cd /usr/install

echo "## Step 3.3: Compile the shell (sh)"
cd /usr/src/bin/sh; make
cd /usr/install

echo "## Step 3.99: Copy binaries to the new partition"
mkdir "${ROOTFS}/etc"
cp /usr/src/bin/init/init "${ROOTFS}/etc/init"
cp /usr/src/bin/getty/getty "${ROOTFS}/etc/getty"
mkdir "${ROOTFS}/bin"
cp /usr/src/bin/sh/sh "${ROOTFS}/bin/sh"
sync

echo "## Step 3.100: Copy config files to the new partition"
cp /etc/rc "${ROOTFS}/etc/rc"
cp /etc/ttys "${ROOTFS}/etc/ttys"