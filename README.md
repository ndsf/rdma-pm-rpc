# rdma-pm-rpc

Remote process call based on RDMA and PM.

```bash
# install protobuf-3.14.0
wget https://github.com/protocolbuffers/protobuf/releases/download/v3.14.0/protobuf-all-3.14.0.tar.gz
tar zxvf protobuf-all-3.14.0.tar.gz 
cd protobuf-3.14.0/
./configure 
make
sudo make install
sudo ldconfig
# build test
mkdir build && cd build
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
./server
./client
```