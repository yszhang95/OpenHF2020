#!/bin/bash
configName=$1
nEvents=$2
tempDir=$3

if [[ "$configName" = "" ]]; then
  echo "Name of input file is not given"
  echo "Usage: ./generate.sh configName nEvents tempDir"
  exit -1
fi


if [[ "$nEvents" = "" ]]; then
  nEvents=100
fi

if [[ "$tempDir" = "" ]]; then
  tempDir="temp"
fi

# preparing
fBaseName=$(basename $configName)
fLabelName=${fBaseName%*.py}
#echo $fLabelName

# start running

if [[ ! -d $tempDir ]]; then
  mkdir $tempDir
  echo
fi

cd $tempDir
echo "Current directory is ${PWD}"

cp ../$configName .
echo "Current directory:"
ls

source /cvmfs/cms.cern.ch/cmsset_default.sh
#scramv1 list -a | grep CMSSW_10_6_32_patch1
export SCRAM_ARCH=slc7_amd64_gcc700
if [[ ! -d CMSSW_10_6_32_patch1 ]]; then
  scramv1 project CMSSW_10_6_32_patch1
fi
cd CMSSW_10_6_32_patch1/src/
#scramv1 project CMSSW_10_6_19
#cd CMSSW_10_6_19/src/
eval `scramv1 runtime -sh`
mkdir -p Configuration/GenProduction/python
cp ../../$fBaseName Configuration/GenProduction/python/
scram b clean
scram b
cmsDriver.py Configuration/GenProduction/python/$configName \
  --python_filename ${fLabelName}_cfg.py \
  --eventcontent RAWSIM --customise Configuration/DataProcessing/Utils.addMonitoring \
  --datatier GEN \
  --fileout file:${fLabelName}.root \
  --conditions 106X_upgrade2018_realistic_v4 \
  --beamspot Realistic25ns13TeVEarly2018Collision --step GEN \
  --geometry DB:Extended --era Run2_2018 --no_exec --mc -n ${nEvents}

cmsRun ${fLabelName}_cfg.py
