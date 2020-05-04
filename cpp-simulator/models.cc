//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include <cmath>
#include <random>

#ifdef MERSENNE_TWISTER
std::mt19937_64 GENERATOR;
#else
std::default_random_engine GENERATOR;
#endif


void SEED_RNG(){
#ifdef MERSENNE_TWISTER
  //TODO: Use better seeding.  This just seeds with a 32 bit integer.
  std::random_device rd;
  GENERATOR.seed(rd());
#endif
}

void SEED_RNG_PROVIDED_SEED(count_type seed){
#ifdef MERSENNE_TWISTER
  //TODO: Use better seeding.  This just seeds with a 32 bit integer.
  GENERATOR.seed(seed);
#endif
}


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
  double a = GLOBAL.F_KERNEL_A;
  double b = GLOBAL.F_KERNEL_B;
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

const double HALF_PI = atan2(1.0, 0.0);
const double DEGREE = HALF_PI/90.0; //1 degree in radians
const double EARTH_RADIUS = 6371.0; //in KM
double earth_distance(location a, location b){
  double delta_lon = DEGREE*(a.lon - b.lon);
  double delta_lat = DEGREE*(a.lat - b.lat);

  // The Haversine formula
  double angle
	= 2*asin(sqrt(pow(sin(delta_lat/2), 2)
				  + (cos(DEGREE*a.lat)
					 * cos(DEGREE*b.lat)
					 * pow(sin(delta_lon/2), 2))));
  return angle*EARTH_RADIUS;
}


office_attendance ATTENDANCE;

//attendance probability at given time
double get_attendance_probability(WorkplaceType workplace_type, OfficeType office_type, count_type time){
  if (workplace_type != WorkplaceType::office || GLOBAL.IGNORE_ATTENDANCE_FILE){
    return 1;
    //Let the other features handle these workplaces
  } else {
    auto entry = time/GLOBAL.SIM_STEPS_PER_DAY;
    if (entry >= ATTENDANCE.number_of_entries){
      entry = ATTENDANCE.number_of_entries - 1;
      //Just use the last entry
    }
    return ATTENDANCE.probabilities[entry][static_cast<count_type>(office_type)];
  }
}

//interpolation with a threshold
double interpolate(double start, double end, double current, double threshold){
  if (current >= threshold){
    return end;
  }
  else {
    return start + (end - start)*current/threshold;
  }
}

void set_compliance(std::vector<agent> & nodes, std::vector<house> & homes,  double compliance_probability){
  //set the compliant flag for a household and it's individuals based on compliance_probability
  for(count_type count = 0; count < homes.size(); ++count){
    homes[count].compliant=(homes[count].non_compliance_metric <= compliance_probability);
    for(count_type individual_count = 0; individual_count<homes[count].individuals.size();++individual_count){
      nodes[homes[count].individuals[individual_count]].compliant = homes[count].compliant;
    }
  }
}

void set_nbr_cell(house &home){
  if(!GLOBAL.ENABLE_CONTAINMENT) {
    return;
  }

  location loc_temp;

  loc_temp.lon = home.loc.lon;
  loc_temp.lat = GLOBAL.city_SW.lat;  
  home.neighbourhood.cell_x
	= count_type(floor(earth_distance(loc_temp,GLOBAL.city_SW)/GLOBAL.NBR_CELL_SIZE));

  loc_temp.lat = home.loc.lat;
  loc_temp.lon = GLOBAL.city_SW.lon;
  home.neighbourhood.cell_y
	= count_type(floor(earth_distance(loc_temp,GLOBAL.city_SW)/GLOBAL.NBR_CELL_SIZE));
}
