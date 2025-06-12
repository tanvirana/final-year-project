# Computing MEng Individual Project

This repo holds design exploration code and scripts used for the project 'Maximising Heterogeneous Resource Utilisation in FPGAs'.

hls4ml extensions and testing are held in their own standalone repositories.

## Repository Structure
- ```/design_exploration``` 

Holds module directories contain HLS code for configurable heterogeneous resource implementations of pointwise add, pointwise multiplication, summation (additive reduction), and 1D convolution (dot product). Each module has it's own subdirectory. ```/report``` subdirectories hold Vivado HLS and Vivado reports from configured designs.

- ```/scripts```

Subdirectories for configuration of modules. These include interactive programs for high-level configuration, code generation logic, and scripts for automated parameter sweeps.

```vivado_setup.tcl``` sets up a Vivado project for AXI4 streaming to an imported IP. 

```/helpers``` contains scripts for file transfer and environment setup.

```/pynq_z2``` holds programs for deploying and testing designs on a PYNQ-Z2 FPGA development board.