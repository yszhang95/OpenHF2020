#!/usr/bin/bash
if [[ ! -d output ]]; then
    mkdir output
fi
python scanTree.py -i lambdacana_mc_ntuple.root -o output
cat output/cand.txt output/cand_dau0.txt output/cand_dau1.txt output/cand_gdau0.txt output/cand_gdau1.txt > output/ntuple.txt
rm output/cand.txt output/cand_dau0.txt output/cand_dau1.txt output/cand_gdau0.txt output/cand_gdau1.txt
python test.py
python scanTree.py -i output/test_lambdacAna_mc_AllEntries_LambdaCKsProton.root -o output
cat output/cand.txt output/cand_dau0.txt output/cand_dau1.txt output/cand_gdau0.txt output/cand_gdau1.txt > output/tree.txt
rm output/cand.txt output/cand_dau0.txt output/cand_dau1.txt output/cand_gdau0.txt output/cand_gdau1.txt
