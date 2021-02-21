#!/bin/bash
myArray=( "$@" )
for e in ${myArray[@]}
do
  cp $e "${CMSSW_BASE}/src/VertexCompositeAnalysis/VertexCompositeProducer/test/"
done
