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
cp -r ${TopDir}/dataset .

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/NtrkEffTable/eff.root .

pTLabel="3to4"

Options="saveTree:selectMVA:saveDau:selectDeDx:wantAbort"
if [[ $2 == *"Pbp"* ]]; then
  Options="saveTree:selectMVA:saveDau:selectDeDx:wantAbort:flipEta"
fi

Trig=""
TrigIndex=""
MVALabel=""
MVAXML=""
weightBranches=""
if [[ $2 == *"HM"* ]]; then
  Trig="HM"
  TrigIndex="2"
  MVAXML="MLP_vars3D_pT${pTLabel}_Np2N_noDR.xml"
  Options="${Options}:trigReweight"
  weightBranches="eventWeight:eventWeightUp:eventWeightLo"
fi
if [[ $2 == *"MB"* ]]; then
  Trig="MB"
  TrigIndex="4"
  MVALabel="MB"
  MVAXML="MLP_vars3D_pT${pTLabel}_${Trig}_Np2N_noDR.xml"
fi

cat > run.xml << _EOF_
<root>
  <effTable>
    eff.root:Ntrk185:TGraphAsymmErrors
  </effTable>
  <training_variables>
  dedxRes := dau1_dEDxRes ? [0] ? trk_dau1_dEdxRes
  cos(cand_angle3D)? cos([0])? cand_angle3D
  cand_dau1_p ? [0] * cosh([1])? cand_pTDau1:cand_etaDau1
  cand_etaDau1 ? [0] ? cand_etaDau1
  </training_variables>
  <spectator_variables>
    cand_pT? [0] ? cand_pT
  #  cand_mass? [0] ? cand_mass
  </spectator_variables>
  <outfileName>
    LambdaCKsP${pTLabel}${Trig}App
  </outfileName>
  <outputDir>
    ./
  </outputDir>
  <trainXML>
  ${MVAXML}
  </trainXML>
  <treeInfo>
    lambdacAna  ? 4122 ? -1
  </treeInfo>
  <methods>
    MLP? MLP${pTLabel}${MVALabel}Np2N_noDR? !H:!V:NeuronType=tanh:VarTransform=N,G:NCycles=200:HiddenLayers=N+2,N:TestRate=5:UseRegulator=True:ConvergenceTests=5:ConvergenceImprove=1E-3
  </methods>
  <mvaCutMin>
    0.001
    #0.0041
  </mvaCutMin>
  <options>
    ${Options}
  </options>
  <common_cuts>
    cand_pT &gt; 3. ? [0] &gt; 3 ? cand_pT
    cand_pT &lt; 4. ? [0] &lt; 4 ? cand_pT
    abs(cand_y) &lt; 1. ? abs([0]) &lt; 1. ? cand_y
    cand_dau0_decayLength3D/cand_dau0_decayLengthError3D &gt; 5. ? [0]/[1] &gt; 5 ? cand_dau0_decayLength3D:cand_dau0_decayLengthError3D
    cand_dau0_dca &lt; 0.5 ? [0] &lt; 0.5 ? cand_dau0_dca
    cos(cand_dau0_angle3D) &gt; 0.999 ? cos([0]) &gt; 0.999 ? cand_dau0_angle3D
  </common_cuts>
  <inputFiles>
    $2
  </inputFiles>
  <triggerIndex>
    ${TrigIndex}
  </triggerIndex>
  <filterIndex>
    4
  </filterIndex>
  <KeptBranchNames>
    trk_dau1_dEdxRes:trk_dau1_dEdx_dedxHarmonic2
    cand_massDau0:cand_pTDau0:cand_etaDau0:cand_phiDau0:
    cand_massDau1:cand_pTDau1:cand_etaDau1:cand_phiDau1:
    cand_dau0_pT:cand_dau0_eta:cand_dau0_phi:
    cand_dau1_pT:cand_dau1_eta:cand_dau1_phi:
    cand_mass:cand_pT:cand_eta:cand_y:cand_phi
    cand_angle3D:cand_dauCosOpenAngle3D:cand_Ntrkoffline
    cand_decayLength3D:cand_decayLengthError3D:
    Ntrkoffline:trigBit_2:trigBit_4:filterBit_4:filterBit_5:
    MLP${pTLabel}${MVALabel}Np2N_noDR
    ${weightBranches}
  </KeptBranchNames>
  <RooWorkspace>
    ws${Trig}Pt${pTLabel}:ws${Trig}Pt${pTLabel}
  </RooWorkspace>
  <RooDataSet>
    ds${Trig}Pt${pTLabel}:ds${Trig}Pt${pTLabel}
  </RooDataSet>
</root>
_EOF_

Exe="${OPENHF2020TOP}/Training/bin/TMVAClassificationApp"

echo 'LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib' "${Exe}" run.xml
#ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP}/Utilities LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib ${Exe} run.xml || exit $?
ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP} LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib ${Exe} run.xml || exit $?

ls -lsth
InputSample="${2}"
OutFileSuffix=${InputSample#*.list.}
OutFileLabel=${InputSample%.list.*}

OriginalName="TMVA_LambdaCKsP${pTLabel}${Trig}App"
if [[ $2 == *"Pbp"* ]]; then
  OriginalName="TMVA_LambdaCKsP${pTLabel}${Trig}App_etaFlipped"
fi

xrdcp -f ${OriginalName}.root "${Dest}/${OriginalName}_${OutFileLabel}_${OutFileSuffix}.root"
echo "Finished"
