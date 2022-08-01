#!/usr/bin/bash
pTMin=( 2 3 4 5 6 7 8 9 )
pTMax=( 3 4 5 6 7 8 9 10 )
for i in {0..7}; do
  #echo $i
  #echo ${pTMin[$i]}
  #echo ${pTMax[$i]}
  cp ../D0wKKPiPi_Ntrk0to35_std.txt D0wKKPiPi_pT${pTMin[$i]}to${pTMax[$i]}_Ntrk0to35_std.txt
  cp ../D0wKKPiPi_Ntrk35to60_std.txt D0wKKPiPi_pT${pTMin[$i]}to${pTMax[$i]}_Ntrk35to60_std.txt
  cp ../D0wKKPiPi_Ntrk60to90_std.txt D0wKKPiPi_pT${pTMin[$i]}to${pTMax[$i]}_Ntrk60to90_std.txt
  cp ../D0wKKPiPi_Ntrk90to120_std.txt D0wKKPiPi_pT${pTMin[$i]}to${pTMax[$i]}_Ntrk90to120_std.txt
  cp ../D0wKKPiPi_Ntrk120to185_std.txt D0wKKPiPi_pT${pTMin[$i]}to${pTMax[$i]}_Ntrk120to185_std.txt
done
