#ifndef MODELS_H_
#define MODELS_H_
#include <vector>
#include <random>


// Global parameters functions
std::default_random_engine GENERATOR;

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
  
} GLOBAL;

// return a random compliance based on GLOBAL.compliance_probability
inline bool compliance(){
  return std::bernoulli_distribution(GLOBAL.compliance_probability)(GENERATOR);
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
  agentState state;
  const agentProps props;
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

enum class WorkplaceType{
   office = 1,
   school = 2
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

