export TARGET=i586-pc-chiffos
export PREFIX=/home/kosmick/chiffOS_newlib/newlib-build/
cp /home/kosmick/chiffos/ChiffOS/libc/syscalls.c /home/kosmick/chiffOS_newlib/newlib-1.20.0/newlib/libc/sys/chiffos/
cp /home/kosmick/chiffos/ChiffOS/libc/syscalls.h /home/kosmick/chiffOS_newlib/newlib-1.20.0/newlib/libc/sys/chiffos/

cd /home/kosmick/chiffOS_newlib/newlib-build/
../newlib-1.20.0/configure --target=$TARGET --prefix=$PREFIX
echo Building Newlib...
make -j8 
make install

cp /home/kosmick/chiffOS_newlib/newlib-build/i586-pc-chiffos/lib/libc.a /home/kosmick/chiffos/ChiffOS/libc/
cp /home/kosmick/chiffOS_newlib/newlib-build/i586-pc-chiffos/lib/libg.a /home/kosmick/chiffos/ChiffOS/libc/
cp /home/kosmick/chiffOS_newlib/newlib-build/i586-pc-chiffos/lib/libm.a /home/kosmick/chiffos/ChiffOS/libc/
