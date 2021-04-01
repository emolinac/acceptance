#!/bin/bash

#SBATCH --time=3:00:00

source /opt/software/root/default/bin/thisroot.sh

N_Q2=$1
N_NU=$2
N_ZH=$3
N_PT2=$4
N_PHI=$5

Q2_BIN_SELECTED=$6
NU_BIN_SELECTED=$7
ZH_BIN_SELECTED=$8

material=$9
nsimufiles=${10}

fsimul_loc=${11}
ftarget_loc=${12}
fbin_loc=${13}

cd $SLURM_SUBMIT_DIR
cp MACRO_accfactors_parallel_Q2NuZh.cpp $TMPDIR
cd $TMPDIR

root -q -b -l "MACRO_accfactors_parallel_Q2NuZh.cpp(${N_Q2},${N_NU},${N_ZH},${N_PT2},${N_PHI},${Q2_BIN_SELECTED},${NU_BIN_SELECTED},${ZH_BIN_SELECTED},\"${material}\",${nsimufiles},\"${fsimul_loc}\",\"${ftarget_loc}\",\"${fbin_loc}\")"
mv accfactors_"${material}"_"${Q2_BIN_SELECTED}${NU_BIN_SELECTED}${ZH_BIN_SELECTED}"_"${N_Q2}${N_NU}${N_ZH}${N_PT2}${N_PHI}".root $ftarget_loc
