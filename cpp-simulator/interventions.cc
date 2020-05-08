//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "interventions.h"
const int UPPER_AGE = 65;
using std::vector;
using std::min;

double kappa_T(const agent& node, double cur_time){
  double val = 0;
  if(!node.infective){
	val = 0;
  }
  else {
	double time_since_infection = cur_time - node.time_of_infection;

	if(time_since_infection < node.incubation_period
	   || time_since_infection> (node.incubation_period
								 + node.asymptomatic_period
								 + node.symptomatic_period)) {
	  // Individual is not yet symptomatic or has been recovered, or has moved to the hospital
	  val = 0;
	} else if(time_since_infection < node.incubation_period + node.asymptomatic_period) {
	  val = 1;
	} else {
	  val = 1.5;
	}
  }
  return val;
}

void set_kappa_base_node(agent& node, double community_factor, const int cur_time){
  //set the basic kappa values for this node according to current time
  node.kappa_T = kappa_T(node, cur_time);
  node.kappa_H = 1.0;
  node.kappa_H_incoming = 1.0;
  node.kappa_W = 1.0;
  node.kappa_W_incoming = 1.0;
  if(node.compliant){
    node.kappa_C = community_factor;
    node.kappa_C_incoming = community_factor;
  }else{
    node.kappa_C = 1.0;
    node.kappa_C_incoming = 1.0;
  }
}

void set_kappa_lockdown_node(agent& node, const int cur_time){
  node.kappa_T = kappa_T(node, cur_time);
  if(node.workplace_type==WorkplaceType::office){
    node.kappa_W = 0.25;
    node.kappa_W_incoming = 0.25;
  }else{
	//Schools and colleges are assumed closed in all lockdowns
    node.kappa_W = 0.0;
    node.kappa_W_incoming = 0.0;
  }

  if(node.compliant){
    node.kappa_H = 2.0;
    node.kappa_H_incoming = 1.0;
    node.kappa_C = 0.25;
    node.kappa_C_incoming = 0.25;
  }else{
    node.kappa_H = 1.25;
    node.kappa_H_incoming = 1.0;
    node.kappa_C = 1.0;
    node.kappa_C_incoming = 1.0;
  }
}

void modify_kappa_SDE_node(agent& node){
  if(node.age>= UPPER_AGE && node.compliant){
    node.kappa_W_incoming = min(0.25, node.kappa_W_incoming);
    node.kappa_C_incoming = min(0.25, node.kappa_C_incoming);
  }
}

void modify_kappa_SC_node(agent& node, double SC_factor){
  if (node.workplace_type==WorkplaceType::school){
    //school and colleges are closed
    node.kappa_W = min(SC_factor, node.kappa_W);
    node.kappa_W_incoming = min(SC_factor, node.kappa_W_incoming);
  }
}

void modify_kappa_OE_node(agent& node){
  if(node.workplace_type==WorkplaceType::office){
    //odd-even rule for workplaces. 50% interactions for workplaces.
    node.kappa_W = min(0.5, node.kappa_W);
    node.kappa_W_incoming = min(0.5,node.kappa_W_incoming);
  }
}

void reset_home_quarantines(vector<house>& homes){
#pragma omp parallel for default(none) shared(homes)
  for(count_type count = 0; count<homes.size(); ++count){
    homes[count].quarantined = false;
  }
}

void modify_kappa_case_isolate_node(agent& node){
  node.quarantined = true;
  node.kappa_H = min(0.75, node.kappa_H);
  node.kappa_W = min(0.0, node.kappa_W);
  node.kappa_C = min(0.1, node.kappa_C);
  node.kappa_H_incoming = min(0.75, node.kappa_H_incoming);
  node.kappa_W_incoming = min(0.0, node.kappa_W_incoming);
  node.kappa_C_incoming = min(0.1, node.kappa_C_incoming);
}

bool should_be_isolated_node(const agent& node, const int cur_time){
  double time_since_symptoms = cur_time
                              - (node.time_of_infection
                              + node.incubation_period
                              + node.asymptomatic_period);
  return ((node.compliant) &&
   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY));
}

void mark_and_isolate_quarantined_homes(vector<agent>& nodes, vector<house>& homes, const int cur_time){
  //mark all homes for quarantine
  for (count_type count = 0; count < nodes.size(); ++count){
    if(should_be_isolated_node(nodes[count],cur_time)){
       homes[nodes[count].home].quarantined = true;
     }
  }

  //isolate all members in quarantined homes
  for (count_type count = 0; count < homes.size(); ++count){
    if(homes[count].quarantined){
      for(count_type resident = 0; resident < homes[count].individuals.size(); ++resident){
        modify_kappa_case_isolate_node(nodes[resident]);
      }
    }
  }
}

void get_kappa_no_intervention(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
#pragma omp parallel for default(none) shared(nodes)
  for (count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;
  }
}

void get_kappa_case_isolation(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  const auto SIM_STEPS_PER_DAY = GLOBAL.SIM_STEPS_PER_DAY;
#pragma omp parallel for default(none) shared(nodes)
  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS * SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * SIM_STEPS_PER_DAY)){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void get_kappa_SC(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  const auto SIM_STEPS_PER_DAY = GLOBAL.SIM_STEPS_PER_DAY;
#pragma omp parallel for default(none) shared(nodes)
  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS * SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * SIM_STEPS_PER_DAY)){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
	if(nodes[count].workplace_type==WorkplaceType::school){
			nodes[count].kappa_W = 0;
			nodes[count].kappa_W_incoming = 0;
	}
  }
}

void get_kappa_home_quarantine(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count < homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void get_kappa_lockdown(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
#pragma omp parallel for default(none) shared(nodes)
  for(count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	if(nodes[count].compliant){
	  nodes[count].kappa_H = 2;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	  if(nodes[count].workplace_type==WorkplaceType::office){
		nodes[count].kappa_W = 0.25;
	  }
	  else{
		nodes[count].kappa_W = 0;
	  }
	}
	else{ //non-compliant
	  nodes[count].kappa_H = 1.25;
	  nodes[count].kappa_C = 1;
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_C_incoming = 1;
	  if(nodes[count].workplace_type==WorkplaceType::office){
		nodes[count].kappa_W = 0.25;
		nodes[count].kappa_W_incoming = 0.25;
	  }
	  else{
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	  }
	}
  }
}

void get_kappa_CI_HQ(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be
	//updated depending on the household individuals.  Same as HQ.
	homes[count].quarantined = false;
  }

  for(count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY)
	   && (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + HOME_QUARANTINE_DAYS) *GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for(count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void get_kappa_CI_HQ_65P(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void get_kappa_LOCKDOWN_fper_CI_HQ_SD_65_PLUS_sper_CI(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P(nodes, homes, workplaces, communities, cur_time);
	}else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_LOCKDOWN_fper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time, double FIRST_PERIOD){
  if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
  }else{
	get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
  }
}

void get_kappa_CI_HQ_65P_SC(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;

	}
	if(nodes[count].workplace_type==WorkplaceType::school){
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	}
  }
}

void get_kappa_CI_HQ_65P_SC_OE(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
	if(nodes[count].workplace_type==WorkplaceType::office){
		//odd-even rule for workplaces. 50% interactions for workplaces.
		nodes[count].kappa_W = 0.5;
		nodes[count].kappa_W_incoming = 0.5;
	} else {
		//school and colleges are closed
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	}
  }
}

struct intervention_decription {
  bool case_isolation = false;
  bool home_quarantine = false;
  bool lockdown = false;
  bool social_dist_elderly = false;
  bool school_closed = false;
  bool workplace_odd_even = false;
  double SC_factor = 0;
  double community_factor = 1;

  intervention_decription& set_case_isolation(bool c){
	this->case_isolation = c;
	return *this;
  }
  intervention_decription& set_home_quarantine(bool c){
	this->home_quarantine = c;
	return *this;
  }
  intervention_decription& set_lockdown(bool c){
	this->lockdown = c;
	return *this;
  }
  intervention_decription& set_social_dist_elderly(bool c){
	this->social_dist_elderly = c;
	return *this;
  }
  intervention_decription& set_school_closed(bool c){
	this->school_closed = c;
	return *this;
  }
  intervention_decription& set_workplace_odd_even(bool c){
	this->workplace_odd_even = c;
	return *this;
  }
  intervention_decription& set_SC_factor(double c){
	this->SC_factor = c;
	return *this;
  }
  intervention_decription& set_community_factor(double c){
	this->community_factor = c;
	return *this;
  }
};



void get_kappa_custom_modular(vector<agent>& nodes, vector<house>& homes, const int cur_time,
							  const intervention_decription& intv){

  if(intv.home_quarantine){
    reset_home_quarantines(homes);
    mark_and_isolate_quarantined_homes(nodes, homes, cur_time);
  }
#pragma omp parallel for default(none) shared(nodes, intv)
  for (count_type count = 0; count < nodes.size(); ++count){
    //choose base kappas
    if(intv.lockdown){
      set_kappa_lockdown_node(nodes[count], cur_time);
    }else{
      set_kappa_base_node(nodes[count], intv.community_factor, cur_time);
    }

    //modifiers begin
    if(intv.social_dist_elderly){
      modify_kappa_SDE_node(nodes[count]);
    }
    if(intv.workplace_odd_even){
	  //This is only for the old attendance implementation.  Now odd even should
	  //be implemented in the attendance file.
      modify_kappa_OE_node(nodes[count]);
    }
    if(intv.school_closed){
      modify_kappa_SC_node(nodes[count], intv.SC_factor);
    }
    if(intv.case_isolation){
      if(should_be_isolated_node(nodes[count],cur_time)){
        modify_kappa_case_isolate_node(nodes[count]);
      }
    }
  }
}

void get_kappa_custom(vector<agent>& nodes, vector<house>& homes,
					  const vector<workplace>& workplaces,
					  const vector<community>& communities, const int cur_time,
					  const bool case_isolation = false,
					  const bool home_quarantine = false,
					  const bool lockdown = false,
					  const bool social_dist_elderly = false,
					  const bool school_closed = false,
					  const bool workplace_odd_even = false,
					  const double SC_factor = 0,
					  const double community_factor = 1){

  if(home_quarantine){
	for(count_type count = 0; count<homes.size(); ++count){
	  //reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	  homes[count].quarantined = false;
  	}
	for (count_type count = 0; count < nodes.size(); ++count){
	  double time_since_symptoms = cur_time
		- (nodes[count].time_of_infection
		   + nodes[count].incubation_period
		   + nodes[count].asymptomatic_period);
	  if((nodes[count].compliant) &&
		 (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
		 (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
		homes[nodes[count].home].quarantined = true;
	  }
	}
  }

  const auto SIM_STEPS_PER_DAY = GLOBAL.SIM_STEPS_PER_DAY;

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period); // needed to determine case isolation status

	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(nodes[count].compliant){
		nodes[count].kappa_C = community_factor;
		nodes[count].kappa_C_incoming = community_factor;
	}

	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant && social_dist_elderly){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}

	if(nodes[count].workplace_type==WorkplaceType::office && workplace_odd_even){
		//odd-even rule for workplaces. 50% interactions for workplaces.
		nodes[count].kappa_W = 0.5;
		nodes[count].kappa_W_incoming = 0.5;
	}

	if (nodes[count].workplace_type==WorkplaceType::school && school_closed){
		//school and colleges are closed
		nodes[count].kappa_W = SC_factor;
		nodes[count].kappa_W_incoming = SC_factor;
	}

	if(lockdown){
		if(nodes[count].compliant){
			nodes[count].kappa_H = 2;
			nodes[count].kappa_C = 0.25; //community value set above ignored.
			nodes[count].quarantined = true;
			nodes[count].kappa_H_incoming = 1;
			nodes[count].kappa_C_incoming = 0.25; //community value set above ignored.
		}
		else{ //non-compliant
			nodes[count].kappa_H = 1.25;
			nodes[count].kappa_C = 1;
			nodes[count].quarantined = true;
			nodes[count].kappa_H_incoming = 1;
			nodes[count].kappa_C_incoming = 1;
		}
		if(nodes[count].workplace_type==WorkplaceType::office){
				nodes[count].kappa_W = 0.25;
				nodes[count].kappa_W_incoming = 0.25;
			}
		else{
			nodes[count].kappa_W = 0;
			nodes[count].kappa_W_incoming = 0;
		}
  	}

	if((nodes[count].compliant) && ((case_isolation && !home_quarantine) || (case_isolation && lockdown)) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS * SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * SIM_STEPS_PER_DAY)){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;	//community value set in the beginning overwritten.
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;	//community value set in the beginning overwritten
	}

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined && home_quarantine){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}



void get_kappa_LD_fper_CI_HQ_SD65_SC_sper_SC_tper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_SC(nodes, homes, workplaces, communities, cur_time);
	}else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_sper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	} else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_OE_sper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time, double FIRST_PERIOD, double OE_SECOND_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+OE_SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P_SC_OE(nodes, homes, workplaces, communities, cur_time);
	} else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_intv_fper_intv_sper_intv_tper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ(nodes, homes, workplaces, communities, cur_time);
	}else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_NYC(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
	const double FIRST_PERIOD = 3;
	const double SECOND_PERIOD = 1;
	const double THIRD_PERIOD = 3;
	const double FOURTH_PERIOD = 5;

	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_custom(nodes, homes, workplaces, communities, cur_time, true, false, false, false, true, false, 0.75, 1);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_custom(nodes, homes, workplaces, communities, cur_time, true, false, false, false, true, false, 0.75, 0.75);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD+FOURTH_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_custom(nodes, homes, workplaces, communities, cur_time, true, false, false, false, true, false, 0, 0.75);
	}else{
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_Mumbai(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
    auto LOCKDOWN_PERIOD = FIRST_PERIOD + SECOND_PERIOD;
	double USUAL_COMPLIANCE_PROBABILITY = 0.6;
	double HD_AREA_COMPLIANCE_PROBABILITY = 0.5;
	if(cur_time
	   < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS + LOCKDOWN_PERIOD)
	   *GLOBAL.SIM_STEPS_PER_DAY){
	  set_compliance(nodes, homes,
					 USUAL_COMPLIANCE_PROBABILITY,
					 HD_AREA_COMPLIANCE_PROBABILITY);
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  //Update global travel parameters
	  GLOBAL.TRAINS_RUNNING = false;
	  GLOBAL.KAPPA_TRAVEL = 0.0;
	} else{
	  set_compliance(nodes, homes,
					 USUAL_COMPLIANCE_PROBABILITY,
					 HD_AREA_COMPLIANCE_PROBABILITY);
	  get_kappa_custom(nodes, homes, workplaces, communities, cur_time, true, true, false, true, true, false, 0, 0.75);
	  //Update global travel parameters
	  GLOBAL.TRAINS_RUNNING = true;
	  GLOBAL.KAPPA_TRAVEL = 1.0;
	}
}

void get_kappa_Mumbai_cyclic(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
  auto LOCKDOWN_PERIOD = FIRST_PERIOD + SECOND_PERIOD;
  double USUAL_COMPLIANCE_PROBABILITY = 0.6;
  double HD_AREA_COMPLIANCE_PROBABILITY = 0.5;
  if(cur_time
	 < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS + LOCKDOWN_PERIOD)
	 *GLOBAL.SIM_STEPS_PER_DAY){
	set_compliance(nodes, homes,
				   USUAL_COMPLIANCE_PROBABILITY,
				   HD_AREA_COMPLIANCE_PROBABILITY);
	get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	//Update global travel parameters
	GLOBAL.TRAINS_RUNNING = false;
	GLOBAL.KAPPA_TRAVEL = 0.0;
  } else{
	set_compliance(nodes, homes,
				   USUAL_COMPLIANCE_PROBABILITY,
				   HD_AREA_COMPLIANCE_PROBABILITY);
	//get_kappa_custom(nodes, homes, workplaces, communities, cur_time, true, true, false, true, true, false, 0, 0.75);
	{
	  intervention_decription intv;
	  intv.case_isolation = true;
	  intv.home_quarantine = true;
	  intv.social_dist_elderly = true;
	  intv.school_closed = true;
	  intv.community_factor = 0.75;
	  //All others are default values
	  get_kappa_custom_modular(nodes, homes, cur_time, intv);
	}
	//Update global travel parameters
	GLOBAL.CYCLIC_POLICY_ENABLED = true;
	GLOBAL.NUMBER_OF_CYCLIC_CLASSES = 3;
	GLOBAL.PERIOD_OF_ATTENDANCE_CYCLE = 5;
	GLOBAL.CYCLIC_POLICY_START_DAY
	  = GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS + LOCKDOWN_PERIOD;

	GLOBAL.TRAINS_RUNNING = true;
	GLOBAL.KAPPA_TRAVEL = 1.0;
  }
}
