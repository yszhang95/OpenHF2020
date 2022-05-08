#!/usr/bin/bash
./skimTree.py --outdir /eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/DataNTuples/ \
  -i /eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV2/dataHM1_pPb.list.005 \
  --selectDeDx --pTMin=2. --pTMax=3. --absMassLw=0.06 --absMassUp=0.11
