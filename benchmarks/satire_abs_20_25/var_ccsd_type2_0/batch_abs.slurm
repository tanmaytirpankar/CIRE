#!/bin/bash
#SBATCH --account=soc-kp
#SBATCH --partition=soc-kp
#SBATCH --reservation=DAT-u1260704
#SBATCH --job-name=20_ccsd_type2_var_0_cire_abs
#SBATCH -o 20_ccsd_type2_var_0_cire_abs.log
#SBATCH -e 20_ccsd_type2_var_0_cire_abs.log
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --time=72:00:00
#SBATCH --export=ALL
#SBATCH --qos=soc-kp

# Abs window : (15,25)
../../../build-debug/bin/CIRE \
    ccsd_type2_var_0.txt \
    --abstraction \
    --min-depth 15 \
    --max-depth 25 \
    --debug-level 1 \
    --log-level 2 \
    --output 20_ccsd_type2_var_0_cire_15_25.csv \
    --log-output 20_ccsd_type2_var_0_cire_15_25.clog \
    --csv-friendly