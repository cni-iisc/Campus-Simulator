#include "models.h"

std::mt19937 GENERATOR(1234);

global_params GLOBAL;

double zeta(int age){
	// This might change based on better age-related interaction data.
  if(age < 5) {
    return 0.1;
  } else if(age < 10) {
    return 0.25;
  } else if(age < 15) {
    return 0.5;
  } else if(age < 20) {
    return 0.75;
  } else if(age < 65) {
    return 1;
  } else if(age < 70) {
    return 0.75;
  } else if(age < 75) {
    return 0.5;
  } else if(age < 85) {
    return 0.25;
  } else {
    return 0.1;
  }
}

int get_age_index(int age){
  // Determine age category of individual.
  if(age < 10) {
    return 0;
  } else if(age < 20) {
    return 1;
  } else if(age < 30) {
    return 2;
  } else if(age < 40) {
    return 3;
  } else if(age < 50) {
    return 4;
  } else if(age < 60) {
    return 5;
  } else if(age < 70) {
    return 6;
  } else if(age < 80) {
    return 7;
  } else {
    return 8;
  }
}


double f_kernel(double dist){
  double a = 4; // for distance in kms
  double b = 3.8;
  //both values are for Thailand, until we get a fit for India
  //Same as in the JS file
  return 1.0/(1.0 + pow(dist/a,b));
}

// Absenteeism parameter. This may depend on the workplace type.
double psi_T(const agent& node, double cur_time){
	if(!node.infective){
	  //check if not infectious
	  return 0;
	}
	double PSI_THRESHOLD = GLOBAL.SIM_STEPS_PER_DAY;
	double time_since_infection = cur_time - node.time_of_infection;
	if(time_since_infection < PSI_THRESHOLD){
	  return 0;
	}
	else{
	  double scale_factor = 0.5;
	  if(node.workplace_type == WorkplaceType::school){
		scale_factor = 0.1;  //school
	  }
	  else if(node.workplace_type == WorkplaceType::office){
		scale_factor = 0.5;  //office
	  }
	  return scale_factor;
	}
}

