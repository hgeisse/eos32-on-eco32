ROOTFS=/usr/install/rootfs

echo "### W A R N I N G"
echo "This CHEAT MODE install script will use the cross-compiled binaries from the"
echo "host system to assemble the new installation. Its only purpose is time-saving."
echo ""

echo "## Step 2.0: Compile kernel sources..."
cp /boot/eos32.bin /usr/src/kernel/src/eos32.bin
cp /boot/eos32.map /usr/src/kernel/src/eos32.map

echo "## Step 2.1: Compile the init binary"
cd /usr/src/bin/init; make

echo "## Step 2.2: Compile getty"
cd /usr/src/bin/getty; make

echo "## Step 2.3: Compile userland commands"
echo " sh..."
cp /bin/sh /usr/src/bin/sh/sh
echo " ls..."
cp /bin/ls /usr/src/bin/ls/ls
echo " echo..."
cp /bin/echo /usr/src/bin/echo/echo
echo " cat..."
cp /bin/cat /usr/src/bin/cat/cat
echo " joe..."
cp /bin/joe /usr/src/bin/joe/joe