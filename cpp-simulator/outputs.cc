#include "models.h"
#include "outputs.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

const string CSV_TERM = "\n";
const char CSV_SEP = ',';
void output_timed_csv(const vector<string>& field_row, const string& output_file, const matrix<count_type>& mat){
  ofstream fout(output_file, ios::out);
  fout << "Time" << CSV_SEP;
  auto end = field_row.end();
  auto penultimate = end - 1;
  for(auto it = field_row.begin(); it != end; ++it){
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


void output_global_params(const string& output_dir){
  ofstream fout(output_dir + "/global_params.txt", ios::out);

  fout << "COMPLIANCE_PROBABILITY: " << GLOBAL.COMPLIANCE_PROBABILITY << ";" << endl; 
  
  fout << "num_homes: " << GLOBAL.num_homes << ";" << endl; 
  fout << "num_workplaces: " << GLOBAL.num_workplaces << ";" << endl; 
  fout << "num_schools: " << GLOBAL.num_schools << ";" << endl; 
  fout << "num_communities: " << GLOBAL.num_communities << ";" << endl; 
  
  fout << "num_people: " << GLOBAL.num_people << ";" << endl; 
  
  fout << "NUM_DAYS: " << GLOBAL.NUM_DAYS << ";" << endl;  //Number of days. Simulation duration
  fout << "SIM_STEPS_PER_DAY: " << GLOBAL.SIM_STEPS_PER_DAY << ";" << endl;  //Number of simulation steps per day.
  fout << "NUM_TIMESTEPS: " << GLOBAL.NUM_TIMESTEPS << ";" << endl;  //
  fout << "INIT_FRAC_INFECTED: " << GLOBAL.INIT_FRAC_INFECTED << ";" << endl;  // Initial number of people infected
  
  fout << "INCUBATION_PERIOD: " << GLOBAL.INCUBATION_PERIOD << ";" << endl; 
  fout << "MEAN_ASYMPTOMATIC_PERIOD: " << GLOBAL.MEAN_ASYMPTOMATIC_PERIOD << ";" << endl; 
  fout << "MEAN_SYMPTOMATIC_PERIOD: " << GLOBAL.MEAN_SYMPTOMATIC_PERIOD << ";" << endl; 
  fout << "MEAN_HOSPITAL_REGULAR_PERIOD: " << GLOBAL.MEAN_HOSPITAL_REGULAR_PERIOD << ";" << endl; 
  fout << "MEAN_HOSPITAL_CRITICAL_PERIOD: " << GLOBAL.MEAN_HOSPITAL_CRITICAL_PERIOD << ";" << endl; 
  
  fout << "INCUBATION_PERIOD_SHAPE: " << GLOBAL.INCUBATION_PERIOD_SHAPE << ";" << endl; 
  fout << "INCUBATION_PERIOD_SCALE: " << GLOBAL.INCUBATION_PERIOD_SCALE << ";" << endl; // 2.29 days
  
  fout << "INFECTIOUSNESS_SHAPE: " << GLOBAL.INFECTIOUSNESS_SHAPE << ";" << endl; 
  fout << "INFECTIOUSNESS_SCALE: " << GLOBAL.INFECTIOUSNESS_SCALE << ";" << endl; 
  
  fout << "SEVERITY_RATE: " << GLOBAL.SEVERITY_RATE << ";" << endl;  //value used in sim.js

  fout << "ASYMPTOMATIC_PERIOD: " << GLOBAL.ASYMPTOMATIC_PERIOD << ";" << endl; 
  // half a day
  fout << "SYMPTOMATIC_PERIOD: " << GLOBAL.SYMPTOMATIC_PERIOD << ";" << endl; 
  // 5 days
  fout << "HOSPITAL_REGULAR_PERIOD: " << GLOBAL.HOSPITAL_REGULAR_PERIOD << ";" << endl; 
  fout << "HOSPITAL_CRITICAL_PERIOD: " << GLOBAL.HOSPITAL_CRITICAL_PERIOD << ";" << endl; 
  fout << "SYMPTOMATIC_FRACTION: " << GLOBAL.SYMPTOMATIC_FRACTION << ";" << endl; 

  fout << "INTERVENTION: " << static_cast<count_type>(GLOBAL.INTERVENTION) << ";" << endl; 

  // Beta values
  fout << "BETA_H: " << GLOBAL.BETA_H << ";" << endl;  //Thailand data
  fout << "BETA_W: " << GLOBAL.BETA_W << ";" << endl;  //Thailand data
  fout << "BETA_S: " << GLOBAL.BETA_S << ";" << endl;  //Thailand data
  fout << "BETA_C: " << GLOBAL.BETA_C << ";" << endl;  // Thailand data. Product = 0.47

  fout << "ALPHA: " << GLOBAL.ALPHA << ";" << endl; 

  fout << "USE_SAME_INFECTION_PROB_FOR_ALL_WARDS: " << GLOBAL.USE_SAME_INFECTION_PROB_FOR_ALL_WARDS << ";" << endl;

  fout.close();
}

gnuplot::gnuplot(const std::string& output_directory){
  fout.open(output_directory + "/gnuplot_script.gnuplot");
  html_out.open(output_directory  + "/plots.html");
  fout << "set datafile separator ','" << std::endl;
  fout << "set key autotitle columnhead" << std::endl;
  fout << "set term png" << std::endl;
  fout << "set termoption noenhanced" << std::endl;
  html_out << "<html>\n<head><title>Plots</title></head>\n<body>\n";
}

void gnuplot::plot_data(const string& name){
  auto image_name = name + ".png";
  fout << "set output \"" << image_name << "\"" << std::endl;
  fout << "set title \"" <<  name << "\"" << std::endl;
  fout << "plot \"" << name << ".csv\" using 1:2 with lines" << std::endl;
  html_out << "<p><img src=\"" << image_name  << "\">\n";
}

gnuplot::~gnuplot(){
  fout.close();
  html_out << "\n</body>\n</html>\n";
  html_out.close();
}
