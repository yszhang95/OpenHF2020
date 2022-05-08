#!/usr/bin/bash
echo "ls -l $PWD"
ls -l $PWD
export X509_USER_PROXY=$1
voms-proxy-info -all
voms-proxy-info -all -file $1

TopDir=${PWD}
TopDir=${PWD}
mkdir Test
WorkDir=${TopDir}/Test
cd ${WorkDir}

cp ${TopDir}/$1 .

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV3/${2} ${TopDir}/$2
xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV3/${2} ${WorkDir}/$2

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/Package/OpenHF2020.tar.gz .
tar zxf OpenHF2020.tar.gz

rm OpenHF2020.tar.gz

RunDir=${WorkDir}/OpenHF2020
cd ${RunDir}
source setup.sh
echo OPENHF2020TOP ${OPENHF2020TOP}

cd ${RunDir}
cp ${TopDir}/$1 .
cp ${TopDir}/$2 .

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/NtrkEffTable/eff.root .

Exe="skimTreeBatch.py"
TreeDir=""
if [[ $2 == *"data"* ]]; then
  TreeDir="lambdacAna"
fi
if [[ $2 == *"MC"* ]]; then
  TreeDir="lambdacAna_mc"
fi

Options=" --selectDeDx --absMassUp=0.15"
if [[ $2 == *"data"* ]]; then
  Options="${Options} --effFile eff.root"
fi
if [[ $2 == *"MC"* ]]; then
  Options="--mc ${Options}"
fi

if [[ $2 == *"HM"* ]]; then
  Options="${Options} --triggerIndex 2"
fi

if [[ $2 == *"MB"* ]]; then
  Options="${Options} --triggerIndex 4"
fi

echo "${OPENHF2020TOP}/Training/Macros/${Exe} -i $2 ${Options}"
${OPENHF2020TOP}/Training/Macros/${Exe} -i $2 ${Options}

ls -lsth
OutFile=$(ls *lambdacAna_AllEntries_pT0p9to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root)
xrdcp -f $OutFile "${3}/$OutFile"
echo "Finished"
