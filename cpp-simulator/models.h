#ifndef MODELS_H_
#define MODELS_H_
#include <vector>
#include <random>
#include <tuple>
#include <cmath>

template<typename T>
using matrix = std::vector< std::vector<T> >;

// Global parameters functions
std::default_random_engine GENERATOR;

inline double gamma(double shape, double scale){
  return std::gamma_distribution<double>(shape, scale)(GENERATOR);
}

inline bool bernoulli(double p){
  return std::bernoulli_distribution(p)(GENERATOR);
}

inline double uniform_real(double left, double right){
  return std::uniform_real_distribution<double>(left, right)(GENERATOR);
}

//These are parameters associated with the disease progression
const double NUM_DAYS_TO_RECOG_SYMPTOMS = 1;
const bool SEED_INFECTION_FROM_FILE = false;
// Global parameters
//
// The default values are as in the js simulator.  These are changed
// when the input files are read.
struct global_params{
  double compliance_probability = 1;

  int num_homes = 25000;
  int num_workplaces = 5000;
  int num_schools = 0;
  int num_communities = 198;

  int num_people = 100000;

  int NUM_DAYS = 120; //Number of days. Simulation duration
  int SIM_STEPS_PER_DAY = 4; //Number of simulation steps per day.
  int NUM_TIMESTEPS = NUM_DAYS*SIM_STEPS_PER_DAY; //
  double INIT_FRAC_INFECTED = 0.0001; // Initial number of people infected

  double INCUBATION_PERIOD = 2.25;
  double MEAN_ASYMPTOMATIC_PERIOD = 0.5;
  double MEAN_SYMPTOMATIC_PERIOD = 5;
  double MEAN_HOSPITAL_REGULAR_PERIOD = 8;
  double MEAN_HOSPITAL_CRITICAL_PERIOD = 8;

  double INCUBATION_PERIOD_SHAPE = 2;
  double INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD*SIM_STEPS_PER_DAY;// 2.29 days

  double INFECTIOUSNES_SHAPE = 0.25;
  double INFECTIOUSNES_SCALE = 4;

  double SEVERITY_RATE = 0.5; //value used in sim.js

  double ASYMPTOMATIC_PERIOD = MEAN_ASYMPTOMATIC_PERIOD*SIM_STEPS_PER_DAY;
  // half a day
  double SYMPTOMATIC_PERIOD = MEAN_SYMPTOMATIC_PERIOD*SIM_STEPS_PER_DAY;
  // 5 days
  double HOSPITAL_REGULAR_PERIOD = MEAN_HOSPITAL_REGULAR_PERIOD*SIM_STEPS_PER_DAY;
  double HOSPITAL_CRITICAL_PERIOD = MEAN_HOSPITAL_CRITICAL_PERIOD*SIM_STEPS_PER_DAY;
  double SYMPTOMATIC_FRACTION = 0.67;

} GLOBAL;

// return a random compliance based on GLOBAL.compliance_probability
inline bool compliance(){
  return bernoulli(GLOBAL.compliance_probability);
}

//Age groups (5-years)

const int NUM_AGE_GROUPS = 16;
int get_age_group(int age){
  int age_group = age/5;
  return std::min(age_group, NUM_AGE_GROUPS - 1);
}


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


// End of global parameters

struct location{
  double lat, lon; //latitude and longitude
};

enum class Progression {
   susceptible,
   exposed,
   infective,
   symptomatic,
   recovered,
   hospitalised,
   critical,
   dead
};


enum class Intervention {
   no_intervention,
   Case_isolation,
   home_quarantine,
   lockdown,
   case_isolation_and_home_quarantine,
   case_isolation_and_home_quarantine_SD_70_PLUS,
   lockdown_21_ci_hq_sd_70_plus_21_ci,
   lockdown_21
};


enum class WorkplaceType{
   office = 1,
   school = 2,
   home = 0
};

//Default workplace value for homebound individuals.
const int WORKPLACE_HOME = -1;

// Time varying properties of agents.
//
// Eg: whether the agent is infected, whether it is
// hospitalized etc.
struct agentState {
};

// Fixed properties of agents.
// Eg: age
struct  agentProps {
};


struct agent{
  location loc;
  int age;
  int age_group;
  double zeta_a = 1;
  double infectiousnes = gamma(GLOBAL.INFECTIOUSNES_SHAPE, GLOBAL.INFECTIOUSNES_SCALE);
  //a.k.a rho
  double severity = bernoulli(GLOBAL.SEVERITY_RATE)?1:0;
  //a.k.a S_k, is 0 or
  int home; //index of household
  int workplace;

  int community;
  double time_of_infection;
  Progression infection_status;
  bool infective = false;

  double lambda_h = 0;
  //individuals contribution to his home cluster
  double lambda_w = 0;
  //individuals contribution to his workplace cluster
  double lambda_c = 0;
  //individuals contribution to his community
  double lambda = 0;

  double kappa_T = 1;
  double psi_T = -0;
  double funct_d_ck;

  WorkplaceType workplace_type;
  //one of school, office, or home
  std::tuple<double, double, double> lambda_incoming;
  //infectiousness from home, workplace, community as seen by
  //individual


  bool compliant = true;
  double kappa_H = 1;
  double kappa_W = 1;
  double kappa_C = 1;

  double incubation_period = gamma(GLOBAL.INCUBATION_PERIOD_SHAPE, GLOBAL.INCUBATION_PERIOD_SCALE);
  double asymptomatic_period = gamma(1, GLOBAL.ASYMPTOMATIC_PERIOD);
  double symptomatic_period = gamma(1, GLOBAL.SYMPTOMATIC_PERIOD);

  double hospital_regular_period = GLOBAL.HOSPITAL_REGULAR_PERIOD;
  double hospital_critical_period = GLOBAL.HOSPITAL_CRITICAL_PERIOD;

  double kappa_H_incoming = 1;
  double kappa_W_incoming = 1;
  double kappa_C_incoming = 1;
  bool quarantined = false;

  agent(){}

};


struct house{
  location loc;
  double lambda_home = 0;
  std::vector<agent> individuals;
  double Q_h = 1;
  double scale = 1;
  bool compliant;
  bool quarantined = false;
  //age_dependent_mixing not added yet, since it is unused
  house(){}
  house(double latitude, double longitude, bool compliance):
	loc{latitude, longitude}, compliant(compliance) {}

  void set(double latitude, double longitude, bool compliance){
	this->loc = {latitude, longitude};
	this->compliant = compliance;
  }
};


struct workplace {
  location loc;
  double lambda_workplace = 0;
  std::vector<agent> individuals;
  double Q_w = 1;
  double scale = 0;
  WorkplaceType workplace_type;
  bool quarantined = false;
  //age_dependent_mixing not added yet, since it is unused

  workplace(){}
  workplace(double latitude, double longitude, WorkplaceType t):
	  loc{latitude, longitude}, workplace_type(t) {}

  void set(double latitude, double longitude, WorkplaceType t){
	this->loc = {latitude, longitude};
	this->workplace_type = t;
  }

};

struct community {
  location loc;
  double lambda_community = 0;
  double lambda_community_global = 0;
  std::vector<agent> individuals;
  double Q_c = 1;
  double scale = 0;
  bool quarantined = false;

  community(){}
  community(double latitude, double longitude):
	loc{latitude, longitude}{}
  void set(double latitude, double longitude){
	this->loc = {latitude, longitude};
  }
};

#endif
