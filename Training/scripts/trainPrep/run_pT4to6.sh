#!/usr/bin/bash
echo "ls -l $PWD"
ls -l $PWD
export X509_USER_PROXY=$1
voms-proxy-info -all
voms-proxy-info -all -file $1

TopDir=${PWD}
mkdir Test
WorkDir=${TopDir}/Test
cd ${WorkDir}

cp ${TopDir}/$1 .

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV2/${2} ${TopDir}/$2
xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV2/${2} ${WorkDir}/$2

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/Package/OpenHF2020.tar.gz .
tar zxf OpenHF2020.tar.gz

rm OpenHF2020.tar.gz

RunDir=${WorkDir}/OpenHF2020
cd ${RunDir}
source setup.sh
echo OPENHF2020TOP ${OPENHF2020TOP}
#export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP}/Utilities
#export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Training/Macros:${OPENHF2020TOP}/Utilities/lib

cd ${RunDir}
cp ${TopDir}/$1 .
cp ${TopDir}/$2 .

Exe="skimTreeBatch.py"
TreeDir=""
if [[ $2 == *"data"* ]]; then
  TreeDir="lambdacAna"
fi
if [[ $2 == *"MC"* ]]; then
  TreeDir="lambdacAna_mc"
fi

Options=" --selectDeDx --pTMin=4. --pTMax=6. --absMassLw=0.06 --absMassUp=0.11"
if [[ $2 == *"data"* ]]; then
  Options="${Options}"
fi
if [[ $2 == *"MC"* ]]; then
  Options="--mc ${Options}"
fi

echo "${OPENHF2020TOP}/Training/Macros/${Exe} -i $2 ${Options}"
${OPENHF2020TOP}/Training/Macros/${Exe} -i $2 ${Options}
#./${Exe} -i $2 ${Options}

ls -lsth
OutFile=$(ls *AllEntries_pT4p0to6p0_yAbs1p0_absMassDiff0p060to0p110_LamCKsP.root)
xrdcp $OutFile "${3}/$OutFile"
echo "Finished"
