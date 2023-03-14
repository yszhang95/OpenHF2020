#!/bin/bash
root -b -q 'get_feeddown_pp.C("FONLL", true)'
root -b -q 'get_feeddown_pPb.C("FONLL", false, true)'
