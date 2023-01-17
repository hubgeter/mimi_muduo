set -e

if [ ! -d `pwd`/build ];then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/*
rm -rf `pwd`/lib/*

cd `pwd`/build &&
    cmake .. &&
    make

cd ..

#头文件 -> /usr/include/mymuduo   so -> /usr/lib
if [ ! -d /usr/include/mymuduo ];then
    mkdir /usr/include/mymuduo
fi

for header in ` ls *.h `
do
    cp $header /usr/include/mymuduo
done

cp `pwd`/lib/libmymuduo.so  /usr/lib/

ldconfig
