#!/usr/bin/bash
dataMB1=( 718 844 865 867 876 877 879 888 889 976 )
dataMB2=( 2 3 4 )
dataMB3=( 0 1 2 4 53 60 64 68 69 70 71 )

declare -A ds=( [1]=$dataMB1 [2]=$dataMB2 [3]=$dataMB3 )


for i in ${dataMB1[@]}; do
  y=$(printf "%04d" $i)
  #echo $y
  eos root://cmseos.fnal.gov ls "/store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB1_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB1_pPb_${y}.root"
  echo "will delete /store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB1_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB1_pPb_${y}.root"
  eos root://cmseos.fnal.gov rm "/store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB1_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB1_pPb_${y}.root"
done

for i in ${dataMB2[@]}; do
  y=$(printf "%04d" $i)
  #echo $y
  eos root://cmseos.fnal.gov ls "/store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB2_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB2_pPb_${y}.root"
  echo "will delete /store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB2_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB2_pPb_${y}.root"
  eos root://cmseos.fnal.gov rm "/store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB2_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB2_pPb_${y}.root"
done

for i in ${dataMB3[@]}; do
  y=$(printf "%04d" $i)
  #echo $y
  eos root://cmseos.fnal.gov ls "/store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB3_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB3_pPb_${y}.root"
  echo "will delete /store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB3_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB3_pPb_${y}.root"
  eos root://cmseos.fnal.gov rm "/store/user/yousen/RiceHIN/OpenHF2020_LamCKsP/AppMVA/dataMB3_pPb/pT_6.0_8.0/TMVA_LambdaCKsP6to8MBApp_dataMB3_pPb_${y}.root"
done
