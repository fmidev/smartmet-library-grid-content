#!/bin/sh
# Generate the ensemble fixtures of QueryServerTest from the PAL temperature of smartmet-test-data
# (needs the ecCodes tools). Every forecast has a constant value telling it apart: the control
# forecast is 4 and has no 06 UTC step, the perturbed members 1 and 2 have their own number, and
# the deterministic forecast is 9.
set -e
PAL=/usr/share/smartmet/test/data/grib/pal/200808050729_pal_skandinavia_pinta.grib
ENS="packingType=grid_simple,productDefinitionTemplateNumber=1,numberOfForecastsInEnsemble=3"
grib_copy -w discipline=192,parameterCategory=0,parameterNumber=4,step=0/1/2 "$PAL" t.grib
grib_copy -w step=0/2 t.grib cf.grib
grib_set -s $ENS,typeOfProcessedData=3,typeOfEnsembleForecast=1,perturbationNumber=0 -d 4 cf.grib control.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=1 -d 1 t.grib member1.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=2 -d 2 t.grib member2.grib
grib_set -s packingType=grid_simple -d 9 t.grib deterministic.grib
rm -f t.grib cf.grib
