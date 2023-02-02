#!/usr/bin/bash
# for i in {1..20}; do
#   hadd dataMB${i}_Pbp_pT10p0to50p0_yAbs1p0_etaFlipped.root /eos/cms/store/group/phys_heavyions/ec/yousen/OpenHF2020Storage/HighpT/dataMB${i}_Pbp/*root \
#     >& dataMB${i}_Pbp_pT10p0to50p0_yAbs1p0_etaFlipped.log
# done

# for i in {19..20}; do
#  hadd -f dataMB${i}_Pbp_pT10p0to50p0_yAbs1p0_etaFlipped.root /eos/cms/store/group/phys_heavyions/ec/yousen/OpenHF2020Storage/HighpT/dataMB${i}_Pbp/*root \
#    >& dataMB${i}_Pbp_pT10p0to50p0_yAbs1p0_etaFlipped.log
# done

# for i in {1..8}; do
#   hadd -f dataMB${i}_pPb_pT10p0to50p0_yAbs1p0.root /eos/cms/store/group/phys_heavyions/ec/yousen/OpenHF2020Storage/HighpT/dataMB${i}_pPb/*root \
#     >& dataMB${i}_pPb_pT10p0to50p0_yAbs1p0.log
# done

for i in {4..8}; do
  hadd -f dataMB${i}_pPb_pT10p0to50p0_yAbs1p0.root /eos/cms/store/group/phys_heavyions/ec/yousen/OpenHF2020Storage/HighpT/dataMB${i}_pPb/*root \
    >& dataMB${i}_pPb_pT10p0to50p0_yAbs1p0.log
  echo "Done dataMB${i}_pPb_pT10p0to50p0_yAbs1p0.log"
done
