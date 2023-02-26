#!/bin/bash
xmlFile=$1
inputFiles=$2

echo $inputFiles
boost=
if [[ $inputFiles =~ ^.*Pb[pP].*$ ]]; then
  boost="PbP"
else
  boost="PPb"
fi

given=
if [[ $xmlFile =~ ^.*Pb[pP].*$ ]]; then
  given="PbP"
else
  given="PPb"
fi

match=0
if [[ $boost = $given ]]; then
  match=1
fi

if [[ "${OPENHF2020TOP}" = "" ]]; then
  echo "Not found environment"
  exit -1
fi

if [[ ! -d tmp ]]; then
  mkdir tmp
else
  echo "Directory tmp exists"
fi

cp $xmlFile tmp/
inputXML=tmp/$(basename "${xmlFile}")
# use p instead of n
# 0 is for seperator
# A backup named .bak will be created

# unmatch but data is pPb boost
if [[ $match -eq 0 && "$boost" = "PPb" ]]; then
  echo "Input and configuration are unmatched."
  perl -i -pe "s/flipEta//" $inputXML
fi

# unmatch but data is Pbp boost
if [[ $match -eq 0 && "$boost" = "PbP" ]]; then
  echo "Input and configuration are unmatched."
  perl -i -p0e "s/<options>(.*)<\/options>/<options>\n\1:flipEta\n<\/options>\n/s" $inputXML
fi

# modify the input
perl -i -p0e "s/<inputFiles>.*<\/inputFiles>//s" $inputXML

head -n -1 $inputXML > tmp.xml
mv tmp.xml $inputXML

inputFiles=$(cat $inputFiles)

cat >> $inputXML <<- _EOF
<inputFiles>
${inputFiles[@]}
</inputFiles>
</root>
_EOF

${OPENHF2020TOP}/Training/bin/TMVAClassificationApp $inputXML
# rm -r tmp
mv $inputXML tmp/${boost}_$(basename "${xmlFile}")
