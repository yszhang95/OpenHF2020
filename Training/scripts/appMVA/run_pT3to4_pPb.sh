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

InFileDir=""
if [[ "$3" == "2" ]]; then
  InFileDir="SplitFileListsV3"
fi
if [[ "$3" == "4" ]]; then
  InFileDir="SplitFileListsV4"
fi

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/${InFileDir}/${2} ${TopDir}/$2
xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/${InFileDir}/${2} ${WorkDir}/$2

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/Package/OpenHF2020.tar.gz .
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

cat > run.xml <<- _EOF_
<root>
  <training_variables>
    dedxRes := dau1_dEDxRes ? [0] ? cand_dau1_dEdxRes
    cand_dau_dR ? [0] ? cand_dau_dR
    cos(cand_angle3D)? cos([0])? cand_angle3D
    cand_dau1_p ? [0] * cosh([1])? cand_dau1_pT:cand_dau1_eta
    cand_etaDau1 ? [0] ? cand_etaDau1
  </training_variables>
  <spectator_variables>
    cand_pT? [0] ? cand_pT
  </spectator_variables>
  <outfileName>
    pPb_LambdaCKsP3to4App
  </outfileName>
  <outputDir>
    .
  </outputDir>
  <histoBinning>
    120, 2.15, 2.45, 10, 0., 10., 120, -0.2, 1.
    120, 2.15, 2.45, 10, 0., 10., 500, 0.0, 0.01
  </histoBinning>
  <trainXML>
    BDTG_vars3D.xml
    MLP_vars3D_pT3to4_Np2N.xml
  </trainXML>
  <treeInfo>
    lambdacAna  ? 4122 ? -1
  </treeInfo>
  <methods>
    BDT? BDTG_3D? !H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=30:MaxDepth=2
    MLP? MLP3to4Np2N? !H:!V:NeuronType=tanh:VarTransform=N,G:NCycles=200:HiddenLayers=N+2,N:TestRate=5:UseRegulator=True:ConvergenceTests=5:ConvergenceImprove=1E-3
  </methods>
  <options>
    saveTree:selectDeDx:saveDau:selectMVA
  </options>
  <mvaCutMin>
    1.0
    0.002
  </mvaCutMin>
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
  $3
  </triggerIndex>
  <filterIndex>
  $4
  </filterIndex>
  <KeptBranchNames>
    trk_dau1_dEdxRes:trk_dau1_dEdx_dedxHarmonic2
    cand_massDau0:cand_pTDau0:cand_etaDau0:cand_phiDau0:
    cand_massDau1:cand_pTDau1:cand_etaDau1:cand_phiDau1:
    cand_mass:cand_pT:cand_eta:cand_mass:cand_y
    cand_angle3D:cand_dauCosOpenAngle3D:cand_Ntrkoffline
    MLP3to4Np2N
  </KeptBranchNames>
</root>
_EOF_


Exe="${OPENHF2020TOP}/Training/bin/TMVAClassificationApp"

echo 'LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib' "${Exe}" run.xml 1
ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP}/Utilities LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib ${Exe} run.xml

ls -lsth
InputSample="${2}"
OutFileSuffix=${InputSample#*.list.}
OutFileLabel=${InputSample%.list.*}
xrdcp -f TMVA_pPb_LambdaCKsP3to4App.root "${5}/TMVA_pPb_LambdaCKsP3to4App_${OutFileLabel}_${OutFileSuffix}.root"
echo "Finished"
