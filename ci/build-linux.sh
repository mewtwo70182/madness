#! /bin/sh

# Exit on error
set -ev

# Environment variables
export CXXFLAGS="-std=c++11 -mno-avx"
export CPPFLAGS=-DDISABLE_SSE3
if [ "$CXX" = "g++" ]; then
    export CC=/usr/bin/gcc-$GCC_VERSION
    export CXX=/usr/bin/g++-$GCC_VERSION
fi
export F77=/usr/bin/gfortran-$GCC_VERSION
export MPICH_CC=$CC
export MPICH_CXX=$CXX
export MPICC=/usr/bin/mpicc.mpich2
export MPICXX=/usr/bin/mpicxx.mpich2
export LD_LIBRARY_PATH=/usr/lib/lapack:/usr/lib/openblas-base:$LD_LIBRARY_PATH

# Configure and build MADNESS
./autogen.sh 
./configure \
    --enable-debugging --disable-optimization --enable-warning --disable-optimal \
    --with-google-test \
    --enable-never-spin \
    LIBS="-L/usr/lib/lapack -L/usr/lib/openblas-base -llapack -lopenblas -lpthread"
    
make -j2 libraries

# Run unit tests
export MAD_NUM_THREADS=2
make -j2 -k check # run all tests, even if some fail