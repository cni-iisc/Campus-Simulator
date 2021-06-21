## Campus Simulator

This repository contains the source code for the epidemic spread modelling simulation framework for campuses. The simulator is an adaptation of the [IISc-TIFR City-Scale simulator](https://github.com/cni-iisc/epidemic-simulator). 

For ease-of-use there is a single shell script which can be used to test the campus-simulator, called `run_sims.sh`

```bash
run_sims.sh instantiate #instantiate a sample synthetic campus
run_sims.sh simulate #running a simulation on the sample synthetic campus for the basic interventions for 100 days
run_sims.sh visualize #generating plots for the simulation runs in 
run_sims.sh all #runs the complete workflow of instantiating the campus, running simulations, and visualizing simulation results
run_sims.sh init # sets the virtual env and installs required packages
```

Detailed instructions to instantiate campuses is available at [staticInst/README.md](#)
Detailed instructions to run the simulator is available at [cpp-simulator/README.md](#)
Detailed instructions to calibrate the simulator is available at [calibrate_betas/README.md](#)

`visualization/` has a script to generate the plots
