cd /mnt/ccnas2/tdp/tr720/final-year-project-workspace
open_project design_exploration_hls_prj -reset
set_top calculate_value

add_files /mnt/ccnas2/tdp/tr720/final-year-project-workspace/final-year-project/design_exploration/pointwise_mul/main.cpp
add_files /mnt/ccnas2/tdp/tr720/final-year-project-workspace/final-year-project/design_exploration/pointwise_mul/config.h
add_files -tb /mnt/ccnas2/tdp/tr720/final-year-project-workspace/final-year-project/design_exploration/pointwise_mul/main_test.cpp
open_solution solution1
set_part xc7z020clg400-1
create_clock -period 10 -name default

csim_design

puts "Simulated"

csynth_design

puts "Synthesised" 

export_design -rtl verilog

puts "Exported"

puts "Closing project"

close_project
exit