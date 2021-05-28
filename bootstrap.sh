#!/bin/bash

function usage ()
{
    echo "Usage : ./bootstrap.sh [option...]
Run Bootstrap from the top directory
This script will create a build folder where the make command can be run

Options:
-t | --trace
Build with tracing enabled.
A program will print detailed output of that it is doing on the standard output
 
-m | --measures
Build with measured enabled.
A program will print performance measures of internal parts of the library
         
-d | --debug
Build in debug mode with debug symbols and no optimization
 
-r | --release
Build in release mode with optimizations and debug symbols
"
}

build_dir=build
build_dir=$(realpath $build_dir)

src_dir=src
src_dir=$(realpath $src_dir)

cmake_args=

compile_flags="-Wall -Wextra -Wno-unused-variable  -Wno-unused-parameter -Wno-comment"

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -h|--help)
    usage
    exit 0
    shift
    ;;
    -t|--trace)
    cmake_args="$cmake_args -DENABLE_LOG_TRACE=ON"
    shift
    ;;
    -m|--measures)
    cmake_args="$cmake_args -DENABLE_LOG_MEASURE=ON"
    shift # past argument
    ;;
    -d|--debug)
    cmake_args="$cmake_args -DCMAKE_BUILD_TYPE=Debug"
    shift # past argument
    ;;
    -r|--release)
    cmake_args="$cmake_args -DCMAKE_BUILD_TYPE=Release"
    shift # past argument
    ;;
    -i|--itac)
    cmake_args="$cmake_args -DCMAKE_CXX_COMPILER=mpiicpc -DCMAKE_CXX_FLAGS=\"-tcollect -trace\""
    shift
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

if [ -e src/thirdparty/googletest/README.md ]; then
  echo "** Google Test was already downloaded"
else
  echo "** Downloading Google Test from https://github.com/google/googletest"
  mkdir -p src/thirdparty
  wget ${WGET_FLAGS} -O src/thirdparty/googletest.zip \
    https://github.com/google/googletest/archive/a325ad2db5deb623eab740527e559b81c0f39d65.zip
  pushd src/thirdparty
  unzip googletest.zip
  mv googletest-a325ad2db5deb623eab740527e559b81c0f39d65 googletest
  popd
  echo 
  echo "*** Please review Google Testing Framework licence at ***"
  echo "**  src/thirdparty/googletest/LICENSE"
  echo 
fi

if [ -e src/examples/innerproduct/src/bspedupack.c ]; then
  echo "** BSPEdupack was already downloaded"
else
  echo "** Downloading BSPedupack"
  wget ${WGET_FLAGS} -O src/examples/innerproduct/src/BSPedupack.tar \
    http://www.staff.science.uu.nl/~bisse101/Book/Edupack/BSPedupack1.02.tar
  pushd src/examples/innerproduct/src
  tar xvf BSPedupack.tar
  for p in *.patch
  do
    echo " - Patching ${p%%.patch}"
    patch BSPedupack1.02/${p%%.patch} -o ${p%%.patch} -i $p
  done
  mv bspinprod.c InnerProductActor.cpp

  popd

  echo "*** Please review BSPedupack licence at ***"
  echo "**   src/examples/innerproduct/src/BSPedupack1.02"
  echo 
fi

if [ -e src/NewActiveBSPonMPIWrapper/bsponmpi/README ]; then
  echo "** BSPonMPI v1.1 was already installed"
else
  echo "** Downloading BSPonMPI v1.1 from Github"
  wget ${WGET_FLAGS} -O src/NewActiveBSPonMPIWrapper/bompiv1.1.tar.gz \
    https://github.com/wijnand-suijlen/bsponmpi/archive/v1.1.tar.gz
  pushd src/NewActiveBSPonMPIWrapper
  tar xzvf bompiv1.1.tar.gz
  for f in `find bsponmpi-1.1 -type f`
  do
    dst=bsponmpi${f##bsponmpi-1.1}
    if [ ! -d `dirname $dst` ]; then
      mkdir -p `dirname $dst`
    fi
    patch=bsponmpi/patches/${f##bsponmpi-1.1}.patch
    if [ -e $patch ]; then
      echo " - PATCHING file $f to $dst"
      patch $f -o $dst -i $patch
    else
      echo " - Copying file $f to $dst"
      cp $f $dst
    fi
  done
  popd

  echo "*** Please review BSPonMPI licence at ***"
  echo "**   src/NewActiveBSPonMPIWrapper/bsponmpi/LICENSE"
  echo 
fi

cmake_args="$cmake_args -DCMAKE_CXX_FLAGS=$compile_flags"

echo "** Using $build_dir as build directory"

if [ -d $build_dir ]; then
    echo "Build directory already exists, cleaning it"
    rm -r $build_dir
fi

echo "** Creating build directory"
mkdir $build_dir

cd $build_dir

echo "** Running cmake command"

${CMAKE:-cmake} $cmake_args $src_dir "$@"

echo "bootstrap finished. Now run make from the build directory"

