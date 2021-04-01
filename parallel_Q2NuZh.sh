#!/bin/bash

#PROGRAM TO SEND PARALLEL JOBS

#VARIABLES
binning_location=/user/e/emolina/Pt_broadening/binning/
simul_location=/eos/user/e/emolina/kt_analysis/
target_location=/user/e/emolina/Pt_broadening/acceptance/single_files/

N_NU=0
NSIMUFILES=1
NU_BINS_ARRAY=(3 3 3)

echo Input D C Fe Pb
read target

echo Input N_Q2
read N_Q2

echo Input N_Zh
read N_ZH

echo Input N_PT2
read N_PT2

echo Input N_PHI
read N_PHI

binning_file="$binning_location"binning_"$target"_"$N_Q2$N_NU$N_ZH$N_PT2$N_PHI".root

if [ -e "$binning_file" ] && [ -d "$binning_location" ] && [ -d "$simul_location" ] && [ -d "$target_location" ]
then
    for(( i = 0 ; i < $N_Q2 ; i++ ))
    do
	NU_BINS=${NU_BINS_ARRAY[$i]}
	for(( j = 0 ; j < $NU_BINS ; j++))
	do
	    for(( k = 0 ; k < $N_ZH ; k++ ))
	    do
		echo $i $j $k
		echo Sending jobs for "$N_Q2 $N_NU $N_ZH $N_PT2 $N_PHI $i $j $k $target $NSIMUFILES $simul_location $target_location $binning_location"
		sbatch JOB_MACRO_accfactors_Q2NuZhparallel.sh $N_Q2 $N_NU $N_ZH $N_PT2 $N_PHI $i $j $k $target $NSIMUFILES $simul_location $target_location $binning_location
	    done
	done
    done
elif [ ! -e "$binning_file" ] || [ ! -d "$binning_location" ] || [ ! -d "$simul_location" ] || [ ! -d "$target_location" ]
then
    echo One of the adresses does not exist PLEASE CHECK
fi





