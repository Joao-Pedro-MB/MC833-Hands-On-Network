mkdir ./client/bin
mkdir ./server/bin
cd ./client/bin
cmake ..
make
cd ../../server/bin
cmake ..
make