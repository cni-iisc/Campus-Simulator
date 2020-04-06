#include "models.h"

std::default_random_engine GENERATOR;

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

double f_kernel(double dist){
  double a = 4; // for distance in kms
  double b = 3.8;
  //both values are for Thailand, until we get a fit for India
  //Same as in the JS file
  return 1/(1 + pow(dist/a,b));
}
