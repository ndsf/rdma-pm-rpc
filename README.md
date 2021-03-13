# rdma-pm-rpc

Remote process call based on RDMA and PM.

## Build

```bash
# install protobuf-3.14.0
wget https://github.com/protocolbuffers/protobuf/releases/download/v3.14.0/protobuf-all-3.14.0.tar.gz
tar zxvf protobuf-all-3.14.0.tar.gz 
cd protobuf-3.14.0/
./configure 
make
sudo make install
sudo ldconfig
# install PMDK-1.9.2 
wget https://github.com/pmem/pmdk/releases/download/1.9.2/pmdk-1.9.2.tar.gz
tar zxvf pmdk-1.9.2.tar
cd pmdk-1.9.2/
sudo yum install -y autoconf automake pkgconfig glib2-devel libfabric-devel pandoc ncurses-devel libfabric
make
sudo make install
# build test
mkdir build && cd build
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
./server
./client
```

## Benchmark

Client thread num: 10
Cost 17435 ms for 3000000 requests
Requests per second: 172068