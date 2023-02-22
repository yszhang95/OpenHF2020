#!/bin/bash

outlog=$(printf "pT%.1fto%.1f_MB.log\n" $1 $2)
root -b -q "proj1D.C($1,$2)" &> logs/${outlog}
