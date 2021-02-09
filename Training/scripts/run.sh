#!/bin/bash
echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
echo "System software: `cat /etc/redhat-release`" #Operating System on that node
# CMSSW
source /cvmfs/cms.cern.ch/cmsset_default.sh 
export SCRAM_ARCH=slc7_amd64_gcc530
scramv1 project CMSSW CMSSW_8_0_35
cd CMSSW_8_0_35/src/
eval `scramv1 runtime -sh`
echo $CMSSW_BASE "is the CMSSW we have on the local worker node"

cd ${_CONDOR_SCRATCH_DIR}
pwd

echo "Arguments passed to the job: $1"

git clone https://github.com/yszhang95/OpenHF2020.git
cd OpenHF2020/Training
mkdir bin
make
cd test
../bin/tmvaClassfication ../Configs/$1
cd ../

cd ${_CONDOR_SCRATCH_DIR}
pwd
rm -rf CMSSW_8_0_35
