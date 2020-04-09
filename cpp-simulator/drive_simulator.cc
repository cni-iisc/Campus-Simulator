#include "simulator.h"
#include "outputs.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int TOTAL_INPUT_ARGS = 17;
  // 1: NUM_DAYS
  // 2: INIT_FRAC_INFECTED
  // 3: INCUBATION_PERIOD
  // 4: MEAN_ASYMPTOMATIC_PERIOD
  // 5: MEAN_SYMPTOMATIC_PERIOD
  // 6: SYMPTOMATIC_FRACTION
  // 7: MEAN_HOSPITAL_REGULAR_PERIOD
  // 8: MEAN_HOSPITAL_CRITICAL_PERIOD
  // 9: COMPLIANCE_PROBABILITY
  // 10: BETA_H
  // 11: BETA_W
  // 12: BETA_C
  // 13: BETA_S
  // 14: BETA_TRAVEL
  // 15: INTERVENTION
  // 16: output directory
  // 17: input directory
  
int main(int argc, char** argv){
  cout << "Number of arguments provided: " << argc << endl;
  assert(argc == TOTAL_INPUT_ARGS + 1);
  GLOBAL.NUM_DAYS = stoct(argv[1]);
  GLOBAL.NUM_TIMESTEPS = GLOBAL.NUM_DAYS*GLOBAL.SIM_STEPS_PER_DAY;
  GLOBAL.INIT_FRAC_INFECTED = stod(argv[2]);
  GLOBAL.INCUBATION_PERIOD = stod(argv[3]);
  GLOBAL.INCUBATION_PERIOD_SCALE = GLOBAL.INCUBATION_PERIOD*GLOBAL.SIM_STEPS_PER_DAY;
  GLOBAL.MEAN_ASYMPTOMATIC_PERIOD = stod(argv[4]);
  GLOBAL.MEAN_SYMPTOMATIC_PERIOD = stod(argv[5]);
  GLOBAL.SYMPTOMATIC_FRACTION = stod(argv[6]);
  GLOBAL.MEAN_HOSPITAL_REGULAR_PERIOD = stod(argv[7]);
  GLOBAL.MEAN_HOSPITAL_CRITICAL_PERIOD = stod(argv[8]);
  GLOBAL.COMPLIANCE_PROBABILITY = stod(argv[9]);

  GLOBAL.ASYMPTOMATIC_PERIOD = GLOBAL.MEAN_ASYMPTOMATIC_PERIOD*GLOBAL.SIM_STEPS_PER_DAY;
  GLOBAL.SYMPTOMATIC_PERIOD = GLOBAL.MEAN_SYMPTOMATIC_PERIOD*GLOBAL.SIM_STEPS_PER_DAY;
  GLOBAL.HOSPITAL_REGULAR_PERIOD = GLOBAL.MEAN_HOSPITAL_REGULAR_PERIOD*GLOBAL.SIM_STEPS_PER_DAY;
  GLOBAL.HOSPITAL_CRITICAL_PERIOD = GLOBAL.MEAN_HOSPITAL_CRITICAL_PERIOD*GLOBAL.SIM_STEPS_PER_DAY;

  GLOBAL.BETA_H = stod(argv[10]);
  GLOBAL.BETA_W = stod(argv[11]);
  GLOBAL.BETA_C = stod(argv[12]);
  GLOBAL.BETA_S = stod(argv[13]);
  GLOBAL.BETA_TRAVEL = stod(argv[14]);
  
  GLOBAL.INTERVENTION = static_cast<Intervention>(stoi(argv[15]));

  string output_dir(argv[16]);

  GLOBAL.input_base = argv[17];
  if(GLOBAL.input_base != ""
	 && GLOBAL.input_base[GLOBAL.input_base.size() - 1] != '/'){ 
	GLOBAL.input_base += '/';
	//Make sure the path of the input_base
	//directory is terminated by a "/"
  }
  
  auto plot_data = run_simulation();

  output_global_params(output_dir);

  gnuplot gnuplot(output_dir);
  for(const auto& elem: plot_data){
	string csvfile_name = elem.first + ".csv";
	string csvfile_path = output_dir + "/" + csvfile_name;
	if(elem.first == "csvContent"){
	  //This file contains everything!
	  output_timed_csv({"community",
						"infected",
						"affected",
						"hospitalised",
						"critical",
						"dead"},
		csvfile_path, elem.second);
	} else {
	  output_timed_csv({elem.first},
					   csvfile_path,
					   elem.second);
	  gnuplot.plot_data(elem.first);
	}
  }
  return 0;
}
