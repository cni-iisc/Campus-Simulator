#include "simulator.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const string CSV_TERM = "\r\n";
const char CSV_SEP = ',';
void output_timed_csv(const vector<string>& field_row, const string& output_file, const matrix<count_type>& mat){
  ofstream fout(output_file, ios::out);
  fout << "Time" << CSV_SEP;
  auto end = field_row.end();
  auto penultimate = end - 1;
  for(auto it = field_row.begin() + 1; it != end; ++it){
	fout << *it;
	if(it != penultimate){
	  fout<< CSV_SEP;
	}
  }
  fout << CSV_TERM;
  for(const auto& row: mat){
	auto end = row.end();
	auto penultimate = end - 1;
	fout << double(row[0])/GLOBAL.SIM_STEPS_PER_DAY << CSV_SEP;
	for(auto it = row.begin() + 1; it != end; ++it){
	  fout << *it;
	  if(it != penultimate){
		fout<< CSV_SEP;
	  }
	}
	fout << CSV_TERM;
  }
  fout.close();
}

const int TOTAL_INPUT_ARGS = 15;
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
  // 14: INTERVENTION
  // 15: output directory
  
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

  GLOBAL.INTERVENTION = static_cast<Intervention>(stoi(argv[14]));

  string output_dir(argv[15]);
  
  auto plot_data = run_simulation();

  for(const auto& elem: plot_data){
	string csvfile_name = output_dir + "/" + elem.first + ".csv";
	if(elem.first == "csvContent"){
	  //This file contains everything!
	  output_timed_csv({"community",
						"infected",
						"affected",
						"hospitalised",
						"critical",
						"dead"},
		csvfile_name, elem.second);
	} else {
	  output_timed_csv({elem.first}, csvfile_name, elem.second);
	}
  }
  return 0;
}
