#!/bin/bash

if [[ -z "$OPENHF2020TOP" ]]; then
  echo 'Not found $OPENHF2020TOP'
  source ../../setup.sh
  echo '$OPENHF2020TOP is set to ' ${OPENHF2020TOP}
else
  echo '$OPENHF2020TOP is set to ' ${OPENHF2020TOP}
fi

if [[ ! -d NTreesBDTA ]]; then
  mkdir NTreesBDTA
  cd NTreesBDTA
else
  cd NTreesBDTA
fi

BaseDir="${OPENHF2020TOP}/Training/Configs/NTreesBDTA/"
Configs=`ls ${OPENHF2020TOP}/Training/Configs/NTreesBDTA/`
echo
echo "Files under ${OPENHF2020TOP}/Training/Configs/NTreesBDTA/:"
echo ${Configs[@]}
echo

for xml in ${Configs[@]}
do
  LogFile=${xml%xml}log
  echo "Execute TMVAClassification on ${xml}, ${LogFile} will be created"
  echo ../../bin/TMVAClassification ${BaseDir}/$xml '>&' ${LogFile}
  ../../bin/TMVAClassification ${BaseDir}/$xml >& ${LogFile}
  echo "${LogFile} has been created"
  echo
done
