sourceVivadoHls="/mnt/ccnas2/bdp/opt/Xilinx/Vivado/2020.1/settings64.sh"
sourceVivado="/mnt/ccnas2/bdp/opt/Xilinx/Vivado/2020.1/settings64.sh"
baseDir=/mnt/ccnas2/tdp/tr720/final-year-project-workspace
configGenScript=${baseDir}/final-year-project/scripts/add_exploration/generate_config.py
hlsReportDir=${baseDir}/design_exploration_hls_prj_2/solution1/syn/report
hlsScript=${baseDir}/final-year-project/scripts/add_exploration/run_hls.tcl
vivadoScript=${baseDir}/final-year-project/scripts/add_exploration/run_vivado.tcl
reportLogDir=${baseDir}/final-year-project/design_exploration/pointwise_add/reports
vivadoReportDir=${baseDir}

run_hls_synthesis() {
  cd /mnt/ccnas2/tdp/tr720/final-year-project-workspace
  eval "source ${sourceVivadoHls} && vivado_hls -f ${hlsScript}"
  cp ${hlsReportDir}/calculate_value_csynth.rpt ${reportLogDir}/calculate_value/hls/${1}.rpt
  cp ${hlsReportDir}/calculate_value_csynth.xml ${reportLogDir}/calculate_value/hls/${1}.xml
}

run_vivado_synthesis() {
  cd /mnt/ccnas2/tdp/tr720/final-year-project-workspace
  eval "source ${sourceVivado} && vivado -mode batch -source $vivadoScript"
  mv ${vivadoReportDir}/vivado_synth_power.rpt ${reportLogDir}/calculate_value/vivado/power/${1}.rpt
  mv ${vivadoReportDir}/vivado_synth_power.xml ${reportLogDir}/calculate_value/vivado/power/${1}.xml
  echo "Copied power reports"
  mv ${vivadoReportDir}/vivado_synth_timing.rpt ${reportLogDir}/calculate_value/vivado/timing/${1}.rpt
  echo "Copied timing reports"
  mv ${vivadoReportDir}/vivado_synth_util.rpt ${reportLogDir}/calculate_value/vivado/util/${1}.rpt
  mv ${vivadoReportDir}/vivado_synth_util.xml ${reportLogDir}/calculate_value/vivado/util/${1}.xml
  echo "Copied util reports"
}

#for N in {1,2,3,4,8,16,32,64,128,256}
for N in {1,2}
do
  echo "ITERATION N: $N"
  for IN_WIDTH in {1,2,3,4,6,8,10,12,14,16}
  do
    echo "ITERATION IN WIDTH: $IN_WIDTH"

    #DEFAULT
    echo "NEXT"
    echo "DEFAULT"
    if [ ! -f "${reportLogDir}/calculate_value/hls/n${N}_in${IN_WIDTH}_default.rpt" ]
    then
      python $configGenScript --N $N --in_width $IN_WIDTH --default_adds $N --fabric_adds 0 --dsp_adds 0 --ram_adds 0 --ram_partition_factor 1 --dual_port false --lut_ram false
      run_hls_synthesis n${N}_in${IN_WIDTH}_default
      echo "Completed hls synthesis"
      run_vivado_synthesis n${N}_in${IN_WIDTH}_default
      echo "Completed vivado synthesis"
    else 
      echo "SKIPPED"
    fi
    
    #FABRIC
    echo "NEXT"
    echo "FABRIC"
    if [ ! -f "${reportLogDir}/calculate_value/hls/n${N}_in${IN_WIDTH}_fabric.rpt" ]
    then
      python $configGenScript --N $N --in_width $IN_WIDTH --default_adds 0 --fabric_adds $N --dsp_adds 0 --ram_adds 0 --ram_partition_factor 1 --dual_port false --lut_ram false
      run_hls_synthesis n${N}_in${IN_WIDTH}_fabric
      echo "Completed hls synthesis"
      run_vivado_synthesis n${N}_in${IN_WIDTH}_fabric
      echo "Completed vivado synthesis"
    else  
      echo "SKIPPED"
    fi

    #DSP
    if [ $IN_WIDTH -le 48 ]
    then
      echo "NEXT"
      echo "DSP"
      if [ ! -f "${reportLogDir}/calculate_value/hls/n${N}_in${IN_WIDTH}_dsp.rpt" ]
      then
        python $configGenScript --N $N --in_width $IN_WIDTH --default_adds 0 --fabric_adds 0 --dsp_adds $N --ram_adds 0 --ram_partition_factor 1 --dual_port false --lut_ram false
        run_hls_synthesis n${N}_in${IN_WIDTH}_dsp
        echo "Completed hls synthesis"
        run_vivado_synthesis n${N}_in${IN_WIDTH}_dsp
        echo "Completed vivado synthesis"
      else
        echo "SKIPPED"
      fi
    fi

    #BRAM
    if [ $IN_WIDTH -lt 10 ]
    then 
      echo "NEXT"
      echo "BRAM"
      echo "SINGLE PORT"
      if [ ! -f "${reportLogDir}/calculate_value/hls/n${N}_in${IN_WIDTH}_1pbram.rpt" ]
      then
        python $configGenScript --N $N --in_width $IN_WIDTH --default_adds 0 --fabric_adds 0 --dsp_adds 0 --ram_adds $N --ram_partition_factor $N --dual_port false --lut_ram false
        run_hls_synthesis n${N}_in${IN_WIDTH}_1pbram
        echo "Completed hls synthesis"
        run_vivado_synthesis n${N}_in${IN_WIDTH}_1pbram
        echo "Completed vivado synthesis"
      else  
        echo "SKIPPED"
      fi

      echo "NEXT"
      echo "DUAL PORT"
      if [ ! -f "${reportLogDir}/calculate_value/hls/n${N}_in${IN_WIDTH}_2pbram.rpt" ]
      then
        python $configGenScript --N $N --in_width $IN_WIDTH --default_adds 0 --fabric_adds 0 --dsp_adds 0 --ram_adds $N --ram_partition_factor $N --dual_port true --lut_ram false
        run_hls_synthesis n${N}_in${IN_WIDTH}_2pbram
        echo "Completed hls synthesis"
        run_vivado_synthesis n${N}_in${IN_WIDTH}_2pbram
        echo "Completed vivado synthesis"
      else 
        echo "SKIPPED"
      fi

    fi

    #LUTRAM 
    if [ $IN_WIDTH -lt 10 ]
    then
      echo "NEXT"
      echo "LUTRAM"
      echo "SINGLE PORT"
      if [ ! -f "${reportLogDir}/calculate_value/hls/n${N}_in${IN_WIDTH}_1plutram.rpt" ]
      then
        python $configGenScript --N $N --in_width $IN_WIDTH --default_adds 0 --fabric_adds 0 --dsp_adds 0 --ram_adds $N --ram_partition_factor 1 --dual_port false --lut_ram true
        run_hls_synthesis n${N}_in${IN_WIDTH}_1plutram
        echo "Completed hls synthesis"
        run_vivado_synthesis n${N}_in${IN_WIDTH}_1plutram
        echo "Completed vivado synthesis"
      else 
        echo "SKIPPED"
      fi

      echo "NEXT"
      echo "DUAL PORT"
      if [ ! -f "${reportLogDir}/calculate_value/hls/n${N}_in${IN_WIDTH}_2plutram.rpt" ]
      then
        python $configGenScript --N $N --in_width $IN_WIDTH --default_adds 0 --fabric_adds 0 --dsp_adds 0 --ram_adds $N --ram_partition_factor 1 --dual_port true --lut_ram true
        run_hls_synthesis n${N}_in${IN_WIDTH}_2plutram
        echo "Completed hls synthesis"
        run_vivado_synthesis n${N}_in${IN_WIDTH}_2plutram
        echo "Completed vivado synthesis"
      else 
        echo "SKIPPED"
      fi
    fi

  done
done