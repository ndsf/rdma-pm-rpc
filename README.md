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
# build test
mkdir build && cd build
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
./server
./client
```

## Benchmark

Client thread num: 1
Cost 404 ms for 10000 requests

Client thread num: 5
Cost 32294 ms for 3000000 requests

Client thread num: 10
Cost 29889 ms for 3000000 requests
Requests per second: 100371

Client thread num: 15
Cost 31550 ms for 3000000 requests

Client thread num: 20
Cost 32461 ms for 3000000 requests

Client thread num: 30
Cost 41646 ms for 3000000 requests