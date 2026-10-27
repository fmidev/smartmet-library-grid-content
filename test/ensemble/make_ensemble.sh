#!/bin/sh
# Generate the ensemble fixtures of QueryServerTest from the PAL temperature of smartmet-test-data
# (needs the ecCodes tools). Every forecast has a constant value telling it apart: the control
# forecast is 4 and has no 06 UTC step, the perturbed members 1 and 2 have their own number, and
# the deterministic forecast is 9 and a second control forecast (number 1) is 5. The next run
# (06 UTC, steps 06-08 UTC) has the members 11 and 12 and a control forecast 14 with only its
# first step, as if the rest had not arrived yet. The pressure level fixtures have member 1 on
# 850 and 500 hPa and the control forecast only on 500 hPa.
set -e
PAL=/usr/share/smartmet/test/data/grib/pal/200808050729_pal_skandinavia_pinta.grib
ENS="packingType=grid_simple,productDefinitionTemplateNumber=1,numberOfForecastsInEnsemble=3"
grib_copy -w discipline=192,parameterCategory=0,parameterNumber=4,step=0/1/2 "$PAL" t.grib
grib_copy -w step=0/2 t.grib cf.grib
grib_set -s $ENS,typeOfProcessedData=3,typeOfEnsembleForecast=1,perturbationNumber=0 -d 4 cf.grib control.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=1 -d 1 t.grib member1.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=2 -d 2 t.grib member2.grib
grib_set -s packingType=grid_simple -d 9 t.grib deterministic.grib
grib_set -s $ENS,typeOfProcessedData=3,typeOfEnsembleForecast=1,perturbationNumber=1 -d 5 t.grib control2.grib
grib_set -s dataTime=600 t.grib t6.grib
grib_copy -w step=0 t6.grib cf6.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=1 -d 11 t6.grib next_member1.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=2 -d 12 t6.grib next_member2.grib
grib_set -s $ENS,typeOfProcessedData=3,typeOfEnsembleForecast=1,perturbationNumber=0 -d 14 cf6.grib next_control.grib
grib_set -s typeOfFirstFixedSurface=100,scaleFactorOfFirstFixedSurface=0,scaledValueOfFirstFixedSurface=85000 t.grib p850.grib
grib_set -s typeOfFirstFixedSurface=100,scaleFactorOfFirstFixedSurface=0,scaledValueOfFirstFixedSurface=50000 t.grib p500.grib
cat p850.grib p500.grib > p.grib
grib_set -s $ENS,typeOfProcessedData=4,typeOfEnsembleForecast=3,perturbationNumber=1 -d 1 p.grib pressure_member1.grib
grib_set -s $ENS,typeOfProcessedData=3,typeOfEnsembleForecast=1,perturbationNumber=0 -d 4 p500.grib pressure_control.grib
rm -f t.grib cf.grib t6.grib cf6.grib p850.grib p500.grib p.grib
