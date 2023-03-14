#!/usr/bin/bash
# set up variables for locating files
#source /cvmfs/sft.cern.ch/lcg/views/LCG_99/x86_64-centos7-gcc10-opt/setup.sh
# source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc10-opt/setup.sh
echo -e "\nSetting up environment\n"
if [ -z ${CMSSW_BASE+x} ]; then
    echo "CMSSW ENV is unset. Use LCG instead."
    lcgPath="/cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc10-opt/setup.sh"
    source ${lcgPath}
    echo -e "\e[0;36msource ${lcgPath}\e[m"
    export OPENHF2020TOP=`git rev-parse --show-toplevel`
else
    echo "Will work under CMSSW ENV. This will (probably) contaminate CMSSW."
    echo -e "\e[4;33mPlease note that the command scram b will delete several macros in this repository.\e[m"
    echo -e "\e[4;33mDo not use scram b anymore\e[m"
    export OPENHF2020TOP=${CMSSW_BASE}/src/OpenHF2020
    # HepMC3
    temp_location="${CMS_PATH}/${SCRAM_ARCH}/external/hepmc3"
    locations=( `ls -t $temp_location` )
    latest_hepmc3="${temp_location}/${locations[0]}"
    if [ -f ${OPENHF2020TOP}/Utilities/bin/HepMC3-config ]; then
        echo "HepMC3-config exists!"
    else
        ln -s ${latest_hepmc3}/bin/HepMC3-config ${OPENHF2020TOP}/Utilities/bin
    fi
fi

echo
echo '$OPENHF2020TOP is set to' ${OPENHF2020TOP}
if [[ -z $1 ]]; then
  echo -e "\e[0;32m"
  echo "If you want to use compiled libraries under" '${OPENHF2020TOP}/Utilities/lib'
  echo "inside ROOT prompt, you may want to modifiy the value of" '${LD_LIBRARY_PATH}.'
  echo -e "To make it in effect, run\e[m"
  echo -e "    \e[0;36msource setup.sh --enable-lib\e[m"
  echo -e "\e[0;32mSee more in Utilities/README.org"
  echo -e "\e[m"

elif [[ $1 == "--enable-lib" ]]; then
  export LD_LIBRARY_PATH=${OPENHF2020TOP}/Utilities/lib:${LD_LIBRARY_PATH}
  # valid for v6.06; I do not use it anymore
  # export LD_LIBRARY_PATH=${OPENHF2020TOP}/Utilities:${LD_LIBRARY_PATH}

  echo "Prepend ${OPENHF2020TOP}/Utilities/lib to" '$LD_LIBRARY_PATH, for root'
  # echo "Prepend ${OPENHF2020TOP}/Utilities/to" '$LD_LIBRARY_PATH, for root_v6.06'
  echo -e "\e[0;33m"
  echo "Warning: NEVER run any cmsRun scripts after sourcing this script"
  echo '         ${LD_LIBRARY_PATH}' "has been contaminated"
  echo -e "\e[m"
else
    echo -e "\e[0;31mUnrecognized option $1\e[m\n"
fi

export PATH=$PATH:$OPENHF2020TOP/Utilities/bin
echo -e "\e[0;33mWarning: Utilities/bin has been appended to PATH\e[m"
