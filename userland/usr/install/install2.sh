ROOTFS=/usr/install/rootfs

echo "## Step 2.0: Compile kernel sources..."
cd /usr/src/kernel; make

echo "## Step 2.1: Compile the init binary"
cd /usr/src/bin/init; make

echo "## Step 2.2: Compile getty"
cd /usr/src/bin/getty; make

echo "## Step 2.3: Compile userland commands"
echo " as..."
cd /usr/src/bin/as; make
echo " cat..."
cd /usr/src/bin/cat; make
echo " echo..."
cd /usr/src/bin/echo; make
echo " joe..."
cd /usr/src/bin/joe; make
echo " lcc (includes cpp, rcc, lburg)..."
cd /usr/src/bin/lcc; make
echo " ld..."
cd /usr/src/bin/ld; make
echo " ls..."
cd /usr/src/bin/ls; make
echo " sh..."
cd /usr/src/bin/sh; make
echo " stty..."
cd /usr/src/bin/stty; make

echo "## Step 2.4: Compile the C standard library"
cd /usr/src/lib; make