#!/usr/bin/bash
git clone https://github.com/yszhang95/2017FlowMCRequest.git
source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc7_amd64_gcc530
scramv1 project CMSSW_8_0_35
cd CMSSW_8_0_35/src
eval `scramv1 runtime -sh`
cp -r ../../2017FlowMCRequest/Configuration .
scram b
sed -i 's/BetaBoost = cms\.untracked\.double(0\.434)/BetaBoost = cms.untracked.double(0.)/' \
  Configuration/GenProduction/python/Pythia8_TuneCUETP8M1_8160GeV_LambdaC_KsProton_pt0p9_y2p4_pPb_embed_cfi.py
cmsDriver.py Configuration/GenProduction/python/Pythia8_TuneCUETP8M1_8160GeV_LambdaC_KsProton_pt0p9_y2p4_pPb_embed_cfi.py  \
  --python_filename LambdaC_KsProton_pt0p9_y2p4_pPb.py \
  --fileout file:LambdaC_KsProton_pt0p9_y2p4_pPb.root \
  --mc --eventcontent AODSIM \
  --datatier GEN --conditions 80X_mcRun2_pA_v4 \
  --step GEN \
  --era Run2_2016_pA --number=100000 --no_exec
echo "cmsRun LambdaC_KsProton_pt0p9_y2p4_pPb.py >& log"
cmsRun LambdaC_KsProton_pt0p9_y2p4_pPb.py >& log
cp ../../checkKsDecay.py .
echo "python checkKsDecay.py -b >& KsDecay.log"
python checkKsDecay.py -b >& KsDecay.log
