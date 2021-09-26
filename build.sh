mkdir -p build_dir/
CXX=g++-10 CC=gcc-10 cmake -H. -Bbuild_dir $1
cd build_dir 
make -j `nproc`                 
cd - 
