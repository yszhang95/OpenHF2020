#!/usr/bin/bash
echo "ls -l $PWD"
ls -l $PWD
export X509_USER_PROXY=$1
voms-proxy-info -all
voms-proxy-info -all -file $1


echo
TopDir=${PWD}
git clone https://github.com/yszhang95/OpenHF2020.git
cd OpenHF2020
source setup.sh
echo ${OPENHF2020TOP}
cd Utilities
make -f Makefile

cd ../Training
make

cd $TopDir
echo "Current directory is ${PWD}"

LD_LIBRARY_PATH=${OPENHF2020TOP}/Utilities/lib:${LD_LIBRARY_PATH} ./OpenHF2020/Training/bin/TMVACrossValidation $2

FileName=$(basename -s .xml $2)
FileName="TMVA_${FileName}.root"
Folds=$(ls *_fold*.root)
xrdcp -r -f ${Folds} ${FileName} dataset/ $3

echo
echo "Done"
