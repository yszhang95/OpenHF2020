#!/usr/bin/bash
echo "ls -l $PWD"
ls -l $PWD
export X509_USER_PROXY=$1
voms-proxy-info -all
voms-proxy-info -all -file $1


TopDir=${PWD}
git clone https://github.com/yszhang95/OpenHF2020.git
cd OpenHF2020
source setup.sh
echo ${OPENHF2020TOP}
cd Utilities
make -f Makefile

cd $TopDir
cat > .rootrc <<- _EOF_
Unix.*.Root.MacroPath:    .:${OPENHF2020TOP}/Training/Macros/
ACLiC.BuildDir:        .compiles
ACLiC.IncludePaths:     -I${OPENHF2020TOP}/Utilities
_EOF_

TreeDir=""
if [[ $2 == *"data"* ]]; then
  TreeDir="lambdacAna"
fi
if [[ $2 == *"MC"* ]]; then
  TreeDir="lambdacAna_mc"
fi

Options=""
if [[ $2 == *"data"* ]]; then
  Options="--treeDir $TreeDir"
fi
if [[ $2 == *"MC"* ]]; then
  Options="--treeDir $TreeDir --mc"
fi

Exe=skimTreeRectCuts.py
echo "./OpenHF2020/Training/Macros/${Exe} -i $2 $Options"
./OpenHF2020/Training/Macros/${Exe} -i $2 $Options

ls
FileName=$(basename -- "$2")
FileNameHead="${FileName%.list.*}"
FileNameTail="${FileName#*.list}"
OutFile="${FileNameHead}_${FileNameTail}${TreeDir}_AllEntries_LamCKsP.root"

xrdcp -f ${OutFile} ${3}/${OutFile}
echo "Finished"
