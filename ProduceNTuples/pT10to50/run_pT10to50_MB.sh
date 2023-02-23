#!/usr/bin/bash
echo "ls -l $PWD"
ls -l $PWD
export X509_USER_PROXY=$1
voms-proxy-info -all
voms-proxy-info -all -file $1

Dest=$3

TopDir=${PWD}
TopDir=${PWD}
mkdir Test
WorkDir=${TopDir}/Test
cd ${WorkDir}

cp ${TopDir}/$1 .

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/Package/OpenHF2020.tar.gz . 2> /dev/null
if [[ ! -f OpenHF2020.tar.gz ]]; then
  xrdcp root://cmseos.fnal.gov///store/user/yousen//RiceHIN/OpenHF2020_LamCKsP/Package/OpenHF2020.tar.gz .
fi
tar zxf OpenHF2020.tar.gz
rm OpenHF2020.tar.gz

RunDir=${WorkDir}/OpenHF2020
cd ${RunDir}
source setup.sh
echo OPENHF2020TOP ${OPENHF2020TOP}

cd ${RunDir}
cp ${TopDir}/$1 .
cp ${TopDir}/$2 .

Options=""
if [[ $2 == *"Pbp"* ]]; then
  Options="--flipEta"
fi
Trig=""
TrigIndex=""
if [[ $2 == *"HM"* ]]; then
  Trig="HM"
  TrigIndex="2"
fi
if [[ $2 == *"MB"* ]]; then
  Trig="MB"
  TrigIndex="4"
fi

cat > .rootrc <<- _EOF_
Unix.*.Root.DynamicPath:    .:${OPENHF2020TOP}/Utilities/lib:${OPENHF2020TOP}/Training/Macros/
Unix.*.Root.MacroPath:    .:${OPENHF2020TOP}/Training/Macros/
#ACLiC.IncludePaths:     -I${OPENHF2020TOP}/Utilities
ACLiC.IncludePaths:     -I${OPENHF2020TOP}
_EOF_

ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP} \
  LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib \
  ${OPENHF2020TOP}/Training/Macros/skimTree.py -i ${2} --selectDeDx --wantAbort ${Options} --triggerIndex ${TrigIndex} \
  --pTMin 10 --pTMax 50 \
  || exit $?


ls -lsth
InputSample="${2}"
OutFileSuffix=${InputSample#*.list.}
OutFileLabel=${InputSample%.list.*}

FlipLabel=""
OriginalName="${OutFileLabel}_${OutFileSuffix}_lambdacAna_AllEntries_pT10p0to50p0_yAbs1p0_absMassDiff20p000_LamCKsP"
if [[ $2 == *"Pbp"* ]]; then
  OriginalName="${OriginalName}_etaFlipped"
  FlipLabel="_etaFlipped"
fi
OriginalName="${OriginalName}.root"

#echo $OriginalName

xrdcp -f ${OriginalName} "${Dest}/${OutFileLabel}_${OutFileSuffix}_pT10p0to50p0_yAbs1p0${FlipLabel}.root"
echo "Finished"
