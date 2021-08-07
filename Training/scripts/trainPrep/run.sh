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

Exe=""
if [[ $2 == *"data"* ]]; then
  Exe=skimTree.py
fi
if [[ $2 == *"MC"* ]]; then
  Exe=skimMCTree.py
fi
echo "./OpenHF2020/Training/Macros/${Exe} -i $2"
./OpenHF2020/Training/Macros/${Exe} -i $2

ls
FileName=$(basename -- "$2")
FileNameHead="${FileName%.list.*}"
FileNameTail="${FileName#*.list}"
OutFile="${FileNameHead}_${FileNameTail}${TreeDir}_AllEntries_LamCKsP.root"
SlimOutFile="${FileNameHead}_${FileNameTail}${TreeDir}_AllEntries_LamCKsP_slimmed.root"

echo "./OpenHF2020/Training/Macros/slimTree.py -i ${OutFile} --cuts \"abs(cand_y)<1 && cand_pT<4 && cand_pT>3\""
./OpenHF2020/Training/Macros/slimTree.py -i ${OutFile} --cuts "abs(cand_y)<1 && cand_pT<4 && cand_pT>3 && abs(cand_mass-2.286)>0.09 && abs(cand_mass-2.286)<0.11"
xrdcp -f ${SlimOutFile} ${3}/${SlimOutFile}
echo "Finished"
