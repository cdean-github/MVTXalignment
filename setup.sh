#!/bin/bash
echo "Building MVTX alignment package"
echo "Author: C. Dean, LANL, 2021"

function buildThisProject()
{
  mkdir build
  cd build
  ../autogen.sh --prefix=$MYINSTALL
  make
  make install
  cd ../
}

export MAINDIR=$(pwd)

source /cvmfs/sphenix.opensciencegrid.org/gcc-8.3/opt/sphenix/core/bin/sphenix_setup.sh -n

export MYINSTALL=$MAINDIR/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

echo "Building the MVTX simulation"
cd mvtxSimulation
buildThisProject 

cd $MAINDIR

echo "Building the hit extractor"
cd writeHits 
buildThisProject 

cd $MAINDIR

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

echo "MVTX alignment package built"
echo "Run simulation with:"
echo "cd macros"
echo "root Fun4All_G4_sPHENIX.C"
