#!/bin/bash
#myArray=( "$@" )
#for e in ${myArray[@]}
for e in "$@"
do
  cp "${CMSSW_BASE}/src/VertexCompositeAnalysis/VertexCompositeProducer/test/"${e} .
done
