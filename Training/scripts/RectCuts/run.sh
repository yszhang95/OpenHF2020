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

FileDir=""
TrigIdx=""
if [[ $2 = *"HM"* ]]; then
    FileDir="SplitFileListsV3"
    TrigIdx="2"
elif [[ $2 = *"MB"* ]]; then
    FileDir="SplitFileListsV4"
    TrigIdx="4"
fi

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/${FileDir}/${2} ${TopDir}/$2
xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/${FileDir}/${2} ${WorkDir}/$2

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/Package/OpenHF2020.tar.gz .
tar zxf OpenHF2020.tar.gz
rm OpenHF2020.tar.gz

RunDir=${WorkDir}/OpenHF2020
cd ${WorkDir}/OpenHF2020
cd ${RunDir}
source setup.sh --enable-lib
echo OpenHF2020TOP ${OPENHF2020TOP}


cd ${RunDir}
cp ${TopDir}/$1 .
cp ${TopDir}/$2 .

TreeDir=""
if [[ $2 == *"data"* ]]; then
  TreeDir="lambdacAna"
fi
if [[ $2 == *"MC"* ]]; then
  TreeDir="lambdacAna_mc"
fi

Options="--triggerIndex $TrigIdx --filterIndex 4"
if [[ $2 == *"data"* ]]; then
  Options="--treeDir $TreeDir ${Options}"
fi
if [[ $2 == *"MC"* ]]; then
  Options="--treeDir $TreeDir --mc ${Options}"
fi

Exe=skimTreeRectCuts.py
echo "${OPENHF2020TOP}/Training/Macros/${Exe} -i $2 $Options"
ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP}/Utilities ${OPENHF2020TOP}/Training/Macros/${Exe} -i $2 $Options


ls
FileName=$(basename -- "$2")
FileNameHead="${FileName%.list.*}"
FileNameTail="${FileName#*.list}"
NewFileNameTail="${FileName#*.list.}"
OutFile="${FileNameHead}_${FileNameTail}${TreeDir}_AllEntries_LamCKsP.root"
NewOutFile="${FileNameHead}_${NewFileNameTail}_${TreeDir}_AllEntries_LamCKsP.root"

xrdcp -f ${OutFile} ${3}/${NewOutFile}
echo "Finished"
