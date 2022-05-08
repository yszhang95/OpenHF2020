#!/usr/bin/bash
dir=$(pwd)
cfg=$1

cd ../CMSSW_8_0_36/src
eval `scramv1 runtime -sh`

cp $dir/$cfg ${CMSSW_BASE}/src/Configuration/Generator/python
scram b

cd ${CMSSW_BASE}/../pythons

EVENTS=1000000
NLOG=10000

label="${cfg%*.py}"
#echo $file


cmsDriver.py $cfg  --python_filename ${label}_GENSIM.py --eventcontent RAWSIM --pileup HiMixGEN \
  --customise Configuration/StandardSequences/SimWithCastor_cff.py,Configuration/DataProcessing/Utils.addMonitoring \
  --datatier GEN-SIM \
  --fileout file:${label}_GENSIM.root \
  --pileup_input "dbs:/ReggeGribovPartonMC_EposLHC_pPb_4080_4080_DataBS/pPb816Spring16GS-MB_80X_mcRun2_pA_v4-v2/GEN-SIM" \
  --nThreads 8 \
  --conditions 80X_mcRun2_pA_v4 --beamspot MatchPbPBoost --step GEN,SIM --scenario HeavyIons --era Run2_2016_pA --no_exec --mc -n $EVENTS || exit $?
echo "process.MessageLogger.cerr.FwkReport.reportEvery = $NLOG" >> ${label}_GENSIM.py
