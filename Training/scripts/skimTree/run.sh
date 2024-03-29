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

Exe=""
if [[ "$2" = "MC" ]]; then
  Exe="skimMCTree.py"
elif [[ "$2" = "data" ]]; then
  Exe="skimTree.py"
fi
echo "./OpenHF2020/Training/Macros/${Exe} -i $3"
./OpenHF2020/Training/Macros/${Exe} -i $3

ls
FileName=$(basename -- "$3")
FileNameHead="${FileName%.*}"
FileNameTail="${FileName#*.list}"
TreeDir=""
if [[ "$2" = "MC" ]]; then
  TreeDir="lambdacAna_mc"
elif [[ "$2" = "data" ]]; then
  TreeDir="lambdacAna"
fi
OutFile="${FileNameHead}_${FileNameTail}${TreeDir}_AllEntries_LamCKsP.root"

UseLumi=""
Weight=""
if [[ "$2" = "MC" ]]; then
  UseLumi="False"
  Boost="${FileNameHead#MC_}"
  if [[ "$Boost" = "pPb" ]]; then
    Weight="1"
  elif [[ "$Boost" = "Pbp" ]]; then
    Weight="1.7865"
  fi
elif [[ "$2" = "data" ]]; then
  UseLumi="False"
  Boost="${FileNameHead#data_}"
  if [[ "$Boost" = "pPb" ]]; then
    Weight="1"
  elif [[ "$Boost" = "Pbp" ]]; then
    Weight="1.8363662425527623"
  fi
fi

echo "./OpenHF2020/Training/Macros/addWeight.py -i $OutFile --treeDir ${TreeDir} --weight ${Weight} --uselumi ${UseLumi}"
./OpenHF2020/Training/Macros/addWeight.py -i $OutFile --treeDir ${TreeDir} --weight ${Weight} --uselumi ${UseLumi}
OutWeightedFile="${FileNameHead}_${FileNameTail}${TreeDir}_AllEntries_LamCKsP_weighted.root"

xrdcp -f ${OutFile} ${4}/${OutFile}
xrdcp -f ${OutWeightedFile} ${4}/${OutWeightedFile}
echo "Finished"
