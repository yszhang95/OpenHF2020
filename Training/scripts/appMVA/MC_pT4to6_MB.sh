pTLabel="4to6"

Options="saveTree:selectMVA:saveDau:selectDeDx:wantAbort"
if [[ $1 == *"Pbp"* ]]; then
  Options="saveTree:selectMVA:saveDau:selectDeDx:wantAbort:flipEta"
fi

Trig=""
TrigIndex=""
if [[ $1 == *"HM"* ]]; then
  Trig="HM"
  TrigIndex="2"
fi
if [[ $1 == *"MB"* ]]; then
  Trig="MB"
  TrigIndex="4"
fi

cat > run.xml << _EOF_
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
    LambdaCKsP${pTLabel}${Trig}App
  </outfileName>
  <outputDir>
    ./
  </outputDir>
  <histoBinning>
    120, 2.15, 2.45, 10, 0., 10., 500, 0.0, 0.01
  </histoBinning>
  <trainXML>
    MLP_vars3D_pT${pTLabel}_MB_Np2N_noDR.xml
  </trainXML>
  <treeInfo>
    lambdacAna  ? 4122 ? -1
  </treeInfo>
  <methods>
    MLP? MLP${pTLabel}MBNp2N_noDR? !H:!V:NeuronType=tanh:VarTransform=N,G:NCycles=200:HiddenLayers=N+2,N:TestRate=5:UseRegulator=True:ConvergenceTests=5:ConvergenceImprove=1E-3
  </methods>
  <mvaCutMin>
    0.0005
  </mvaCutMin>
  <options>
    ${Options}
  </options>
  <common_cuts>
    cand_pT &gt; 4. ? [0] &gt; 4 ? cand_pT
    cand_pT &lt; 6. ? [0] &lt; 6 ? cand_pT
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
  <RooWorkspace>
    ws${Trig}Pt${pTLabel}:ws${Trig}Pt${pTLabel}
  </RooWorkspace>
  <RooDataSet>
    ds${Trig}Pt${pTLabel}:ds${Trig}Pt${pTLabel}
  </RooDataSet>
</root>
_EOF_
