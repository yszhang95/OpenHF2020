#!/bin/bash
myArray=( "$@" )
for e in ${myArray[@]}
do
  cp "${CMSSW_BASE}/src/VertexCompositeAnalysis/VertexCompositeProducer/test/$e" $e
done
