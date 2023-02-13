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
echo " as..."
cp /bin/as /usr/src/bin/as/as
echo " cat..."
cp /bin/cat /usr/src/bin/cat/cat
echo " cp..."
cp /bin/cp /usr/src/bin/cp/cp
echo " date..."
cp /bin/date /usr/src/bin/date/date
echo " hangman..."
cp /bin/hangman /usr/src/bin/hangman/hangman
echo " echo..."
cp /bin/echo /usr/src/bin/echo/echo
echo " joe..."
cp /bin/joe /usr/src/bin/joe/joe
echo " lcc (includes cpp, rcc, lburg)..."
mkdir /usr/src/bin/lcc/build
cp /bin/lcc /usr/src/bin/lcc/build/lcc
cp /bin/cpp /usr/src/bin/lcc/build/cpp
cp /bin/rcc /usr/src/bin/lcc/build/rcc
cp /bin/lburg /usr/src/bin/lcc/build/lburg
echo " ld..."
cp /bin/ld /usr/src/bin/ld/ld
echo " login..."
cp /bin/login /usr/src/bin/login/login
echo " ls..."
cp /bin/ls /usr/src/bin/ls/ls
echo " make..."
cp /bin/make /usr/src/bin/make/make
echo " mkdir..."
cp /bin/mkdir /usr/src/bin/mkdir/mkdir
echo " mv..."
cp /bin/mv /usr/src/bin/mv/mv
echo " passwd..."
cp /bin/passwd /usr/src/bin/passwd/passwd
echo " rm..."
cp /bin/rm /usr/src/bin/rm/rm
echo " sh..."
cp /bin/sh /usr/src/bin/sh/sh
echo " stty..."
cp /bin/stty /usr/src/bin/stty/stty
echo " sync..."
cp /bin/sync /usr/src/bin/sync/sync

echo "## Step 2.4: Compile the C standard library"
cp /usr/lib/libc.a /usr/src/lib/libc.a
cp /usr/lib/crt0.o /usr/src/lib/startup/crt0.o