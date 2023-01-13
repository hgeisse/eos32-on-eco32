ROOTFS=/usr/install/rootfs

echo "## Step 2.0: Compile kernel sources..."
cd /usr/src/kernel; make

echo "## Step 2.1: Compile the init binary"
cd /usr/src/bin/init; make

echo "## Step 2.2: Compile getty"
cd /usr/src/bin/getty; make

echo "## Step 2.3: Compile userland commands"
echo " sh..."
cd /usr/src/bin/sh; make
echo " ls..."
cd /usr/src/bin/ls; make
echo " echo..."
cd /usr/src/bin/echo; make
echo " cat..."
cd /usr/src/bin/cat; make
echo " joe..."
cd /usr/src/bin/joe; make