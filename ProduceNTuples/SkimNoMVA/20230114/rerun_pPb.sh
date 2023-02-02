#!/bin/bash
ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP} \
    ${OPENHF2020TOP}/Training/Macros/skimTree.py --pTMin 1.9 \
    --wantAbort --selectDeDx \
    -i $1
