create_project -force /mnt/ccnas2/tdp/tr720/final-year-project-workspace/design_exploration_vivado_prj_2 -part xc7z020clg400-1

add_files /mnt/ccnas2/tdp/tr720/final-year-project-workspace/design_exploration_hls_prj_2/solution1/syn/verilog
synth_design -top calculate_value -part xc7z020clg400-1

set clk_ports [get_ports ap_clk]
if {[llength $clk_ports] > 0} {
    puts "DEFINING ap_clk"
    create_clock -period 10 -name default $clk_ports
} else {
    puts "CREATING VIRTUAL CLOCK"
    create_clock -name virtual_clk -period 10
}
opt_design -retarget -propconst -sweep -bram_power_opt -shift_register_opt
report_utilization -file /mnt/ccnas2/tdp/tr720/final-year-project-workspace/vivado_synth_util.rpt
report_utilization -format xml -file /mnt/ccnas2/tdp/tr720/final-year-project-workspace/vivado_synth_util.xml
report_power -file /mnt/ccnas2/tdp/tr720/final-year-project-workspace/vivado_synth_power.rpt 
report_power -format xml -file /mnt/ccnas2/tdp/tr720/final-year-project-workspace/vivado_synth_power.xml
report_timing -file /mnt/ccnas2/tdp/tr720/final-year-project-workspace/vivado_synth_timing.rpt

close_project