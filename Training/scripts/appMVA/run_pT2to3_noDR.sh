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

xrdcp root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/NtrkEffTable/eff.root .

Options="saveTree:saveDau:selectDeDx:selectMVA"
if [[ ${2} = *"Pbp"* ]]; then
  Options="saveTree:saveDau:selectDeDx:selectMVA:flipEta"
fi

cat > run.xml <<- _EOF_
<root>
  <training_variables>
    dedxRes := trk_dau1_dEdxRes ? [0] ? trk_dau1_dEdxRes
    cos(cand_angle3D)? cos([0])? cand_angle3D
    cand_dau1_p:=cand_pTDau1 * cosh(cand_etaDau1) ? [0] * cosh([1])? cand_pTDau1:cand_etaDau1
    cand_etaDau1 ? [0] ? cand_etaDau1
  </training_variables>
  <spectator_variables>
    cand_pT? [0] ? cand_pT
    cand_mass? [0] ? cand_mass
  </spectator_variables>
  <outfileName>
    pPb_LambdaCKsP2to3App
  </outfileName>
  <outputDir>
    .
  </outputDir>
  <histoBinning>
    120, 2.15, 2.45, 10, 0., 10., 400, 0.0, 0.002
  </histoBinning>
  <trainXML>
    MLP_vars3D_pT2to3_MB_Np2N_noDR.xml
  </trainXML>
  <treeInfo>
    lambdacAna  ? 4122 ? -1
  </treeInfo>
  <methods>
    MLP? MLP2to3MBNp2N_noDR? !H:!V:NeuronType=tanh:VarTransform=N,G:NCycles=200:HiddenLayers=N+2,N:TestRate=5:UseRegulator=True:ConvergenceTests=5:ConvergenceImprove=1E-3
  </methods>
  <options>
    ${Options}
  </options>
  <mvaCutMin>
    1.5E-4
  </mvaCutMin>
  <common_cuts>
    cand_pT &gt; 2. ? [0] &gt; 2 ? cand_pT
    cand_pT &lt; 3. ? [0] &lt; 3 ? cand_pT
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
    MLP2to3MBNp2N_noDR
  </KeptBranchNames>
</root>
_EOF_


Exe="${OPENHF2020TOP}/Training/bin/TMVAClassificationApp"

echo 'LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib' "${Exe}" run.xml 1
ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP}/Utilities LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib ${Exe} run.xml 1

ls -lsth
InputSample="${2}"
OutFileSuffix=${InputSample#*.list.}
OutFileLabel=${InputSample%.list.*}
if [[ ${2} = *"Pbp"* ]]; then
  xrdcp -f TMVA_pPb_LambdaCKsP2to3App_etaFlipped.root "${5}/TMVA_Pbp_LambdaCKsP2to3App_${OutFileLabel}_${OutFileSuffix}.root"
else
  xrdcp -f TMVA_pPb_LambdaCKsP2to3App.root "${5}/TMVA_pPb_LambdaCKsP2to3App_${OutFileLabel}_${OutFileSuffix}.root"
fi
echo "Finished"
