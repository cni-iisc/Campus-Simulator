# Campussim: An agent-based epidemic simulator for campuses

CampusSim is an agent-based simulator that models the disease's spread via various interaction spaces on the campus. It generates an interaction graph between agents and interaction spaces. Agents are assigned different interactions spaces such as classrooms, hostels, messes, and cafeterias. CampusSim is a parsimonious model that enables a fast and concise simulation of the epidemic spread. The simulator models intervention strategies (case isolation, hostel quarantine, etc.) and users can also define their own, custom interventions to simulate. The simulator is a derivative based out of the [IISc-TIFR City-Scale simulator](https://github.com/cni-iisc/epidemic-simulator). 

This project is carried out by the Centre for Networked Intelligence at the Indian Institute of Science, Bangalore and is funded as part of the Campus Rakshak project by the Department of Science and Technology, Govt. of India.

## Quick start

For ease-of-use there is a single shell script which can be used to test the campus-simulator, called `run_sims.sh`.
The script instatiates an example sample campus, compiles the simulator's executable, runs the simulator and makes the plots from the different statistics

```shell
bash run_sims.sh or ./run_sims.sh
```

## Documentation
Detailed instructions to instantiate campuses is available at [staticInst/README.md](#)
Detailed instructions to run the simulator is available at [cpp-simulator/README.md](#)
Detailed instructions to calibrate the simulator is available at [calibrate_betas/README.md](#)

`visualization/` has a script to generate the plots



## License
The source code provided in this repository is available for public-use under the [Apache2 License terms](License.md).
