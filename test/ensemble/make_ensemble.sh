#!/bin/sh
# Generate the ensemble fixtures of QueryServerTest from the PAL temperature of smartmet-test-data
# (needs the ecCodes tools). Every forecast has a constant value telling it apart: the control
# forecast is 4, the perturbed members 1 and 2 have their own number, and the deterministic
# forecast is 9 and a second control forecast (number 1) is 5. The next run (06 UTC, steps
# 06-08 UTC) has the members 11 and 12 and the control forecast 14.
set -e
PAL=/usr/share/smartmet/test/data/grib/pal/200808050729_pal_skandinavia_pinta.grib
ENS="packingType=grid_simple,productDefinitionTemplateNumber=1,numberOfForecastsInEnsemble=3"
grib_copy -w discipline=192,parameterCategory=0,parameterNumber=4,step=0/1/2 "$PAL" t.grib
grib_set -s $ENS,typeOfProcessedData=3,typeOfEnsembleForecast=1,perturbationNumber=0 -d 4 t.grib control.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=1 -d 1 t.grib member1.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=2 -d 2 t.grib member2.grib
grib_set -s packingType=grid_simple -d 9 t.grib deterministic.grib
grib_set -s $ENS,typeOfProcessedData=3,typeOfEnsembleForecast=1,perturbationNumber=1 -d 5 t.grib control2.grib
grib_set -s dataTime=600 t.grib t6.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=1 -d 11 t6.grib next_member1.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=2 -d 12 t6.grib next_member2.grib
grib_set -s $ENS,typeOfProcessedData=3,typeOfEnsembleForecast=1,perturbationNumber=0 -d 14 t6.grib next_control.grib
rm -f t.grib t6.grib
