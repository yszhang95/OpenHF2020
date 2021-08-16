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

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV2/${2} ${WorkDir}/$2

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/Package/OpenHF2020.tar.gz .
tar zxf OpenHF2020.tar.gz

rm OpenHF2020.tar.gz

cd ${WorkDir}/OpenHF2020
source setup.sh --enable-lib
echo ${OPENHF2020TOP}
cd ${OPENHF2020TOP}/Utilities
make -f Makefile

cd $WorkDir
cat > .rootrc <<- _EOF_
Unix.*.Root.MacroPath:    .:${OPENHF2020TOP}/Training/Macros/
ACLiC.BuildDir:        .compiles
ACLiC.IncludePaths:     -I${OPENHF2020TOP}/Utilities
_EOF_

Exe="skimTree.py"
TreeDir=""
if [[ $2 == *"data"* ]]; then
  TreeDir="lambdacAna"
fi
if [[ $2 == *"MC"* ]]; then
  TreeDir="lambdacAna_mc"
fi

Options=" --selectDeDx --pTMin=3. --pTMax=4. --absMassLw=0.06 --absMassUp=0.11"
if [[ $2 == *"data"* ]]; then
  Options="${Options}"
fi
if [[ $2 == *"MC"* ]]; then
  Options="--mc ${Options}"
fi

echo "./OpenHF2020/Training/Macros/${Exe} -i $2 ${Options}"
./OpenHF2020/Training/Macros/${Exe} -i $2 ${Options}

ls
FileName=$(basename -- "$2")
FileNameHead="${FileName%.list.*}"
FileNameTail="${FileName#*.list}"
NewFileNameTail="${FileName#*.list.}"
OutFile="${FileNameHead}_${FileNameTail}${TreeDir}_AllEntries_LamCKsP.root"

xrdcp -f ${OutFile} ${3}/"${FileNameHead}_${NewFileNameTail}_${TreeDir}_AllEntries_LamCKsP.root"
echo "Finished"
