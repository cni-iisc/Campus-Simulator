//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include <cassert>

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

void modify_kappa_ward_containment(agent& node){
  node.quarantined = true;
  node.kappa_H = min(0.75, node.kappa_H);
  node.kappa_W = min(0.25, node.kappa_W);
  node.kappa_C = min(0.25, node.kappa_C);
  node.kappa_H_incoming = min(0.75, node.kappa_H_incoming);
  node.kappa_W_incoming = min(0.25, node.kappa_W_incoming);
  node.kappa_C_incoming = min(0.25, node.kappa_C_incoming);
}

bool should_be_isolated_node(const agent& node, const int cur_time, const int quarantine_days){
  double time_since_symptoms = cur_time
                              - (node.time_of_infection
                              + node.incubation_period
                              + node.asymptomatic_period);
  return (node.entered_symptomatic_state &&
   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + quarantine_days)*GLOBAL.SIM_STEPS_PER_DAY));
}

void mark_homes_for_quarantine(const vector<agent>& nodes, vector<house>& homes, const int cur_time){
  //mark all homes for quarantine
  for (count_type count = 0; count < nodes.size(); ++count){
    if(nodes[count].compliant && should_be_isolated_node(nodes[count],cur_time, HOME_QUARANTINE_DAYS)){
       homes[nodes[count].home].quarantined = true;
     }
  }
}

void mark_neighbourhood_homes_for_quarantine(const vector<agent>& nodes, vector<house>& homes,
												const vector<vector<nbr_cell>>& nbr_cells, const int cur_time){
	for (count_type count = 0; count < nodes.size(); ++count){
		double time_since_hospitalised = cur_time
		- (nodes[count].time_of_infection
			+ nodes[count].incubation_period
			+ nodes[count].asymptomatic_period
			+ nodes[count].symptomatic_period);
		if(((nodes[count].entered_hospitalised_state) &&
		(time_since_hospitalised <= (HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)) ){
			homes[nodes[count].home].quarantined = true;		
			//TODO: Need to check if the nbr_cell's quarantined flag needs to be set.
			grid_cell my_nbr_grid_cell = homes[nodes[count].home].neighbourhood;
			nbr_cell my_nbr_cell = 	nbr_cells[my_nbr_grid_cell.cell_x][my_nbr_grid_cell.cell_y];	
			count_type num_homes_in_cell = my_nbr_cell.houses_list.size();
			for(count_type nbr_count = 0; nbr_count < num_homes_in_cell; ++nbr_count){
				count_type neighbour = my_nbr_cell.houses_list[nbr_count];
				homes[neighbour].quarantined = true;
			}		
		}
	}

}
void isolate_quarantined_residents(vector<agent>& nodes, const vector<house>& homes, const int cur_time){
  for (count_type count = 0; count < homes.size(); ++count){
    if(homes[count].quarantined){
      for(count_type resident = 0; resident < homes[count].individuals.size(); ++resident){
        modify_kappa_case_isolate_node(nodes[resident]);
      }
    }
  }
}

void get_kappa_no_intervention(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
#pragma omp parallel for
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

void get_kappa_case_isolation(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
#pragma omp parallel for
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

	if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * GLOBAL.SIM_STEPS_PER_DAY)){
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

void get_kappa_SC(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
#pragma omp parallel for
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

	if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * GLOBAL.SIM_STEPS_PER_DAY)){
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

void get_kappa_home_quarantine(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  for(count_type count = 0; count < homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

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

void get_kappa_lockdown(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
#pragma omp parallel for
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
		nodes[count].kappa_W_incoming = 0.25;
	  }
	  else{
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
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

void get_kappa_CI_HQ(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
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
	if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY)
	   && (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + HOME_QUARANTINE_DAYS) *GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for(count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

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

void get_kappa_CI_HQ_65P(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
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

void get_kappa_LOCKDOWN_fper_CI_HQ_SD_65_PLUS_sper_CI(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	intervention_params intv_params;
	vector<vector<nbr_cell>> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  intv_params.lockdown = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine=true;
	  intv_params.social_dist_elderly=true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation=true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_LOCKDOWN_fper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, int cur_time, double FIRST_PERIOD){
  intervention_params intv_params;
  vector<vector<nbr_cell>> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
  if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	//get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	intv_params.lockdown = true;
	get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
  }else{
	//get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	intv_params.case_isolation = true;
	get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
  }
}

void get_kappa_CI_HQ_65P_SC(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
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

void get_kappa_CI_HQ_65P_SC_OE(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
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
	
	if(nodes[count].workplace_type==WorkplaceType::office){
		//odd-even rule for workplaces. 50% interactions for workplaces.
		nodes[count].kappa_W = 0.5;
		nodes[count].kappa_W_incoming = 0.5;
	} else {
		//school and colleges are closed
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	}

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

void reset_community_containment(vector<community>& communities){
	for(count_type count = 0; count<communities.size(); ++count){
		//reset all wards as non-quarantined. The status will be updated depending on the household individuals.
		communities[count].quarantined = false;
	}
}

void mark_communities_for_containment(const vector<agent>& nodes, vector<community>& communities,const int cur_time){
	vector<count_type> num_ward_hospitalised(communities.size(),0);
	for (count_type count = 0; count < nodes.size(); ++count){
		double time_since_hospitalised = cur_time
		- (nodes[count].time_of_infection
			+ nodes[count].incubation_period
			+ nodes[count].asymptomatic_period
			+ nodes[count].symptomatic_period);
		if(((nodes[count].entered_hospitalised_state) &&
		(time_since_hospitalised <= (HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)) ){
			++num_ward_hospitalised[nodes[count].community];
		}
	}
	for (count_type count = 0; count < communities.size(); ++count){
		if(num_ward_hospitalised[count] > GLOBAL.WARD_CONTAINMENT_THRESHOLD){
			communities[count].quarantined = true;
		}
	}
}

void get_kappa_custom_modular(std::vector<agent>& nodes, std::vector<house>& homes, const std::vector<workplace>& workplaces, std::vector<community>& communities, std::vector<std::vector<nbr_cell>>& nbr_cells, const int cur_time, const intervention_params intv_params){
  if(intv_params.home_quarantine || intv_params.neighbourhood_containment){
    reset_home_quarantines(homes);
  }
  if(intv_params.home_quarantine ){
    mark_homes_for_quarantine(nodes, homes, cur_time);
    //Don't isolate them yet; have to assign base kappas first.
    //These members will be isolated at the end.
  }
  if(intv_params.neighbourhood_containment){
	mark_neighbourhood_homes_for_quarantine(nodes, homes, nbr_cells, cur_time);
  }

  if(intv_params.ward_containment){
	reset_community_containment(communities);
	mark_communities_for_containment(nodes, communities,cur_time);
  }

#pragma omp parallel for default(none) shared(nodes, homes, communities)
  for (count_type count = 0; count < nodes.size(); ++count){
    //choose base kappas
    if(intv_params.lockdown){
      set_kappa_lockdown_node(nodes[count], cur_time);
    }else{
      set_kappa_base_node(nodes[count], intv_params.community_factor, cur_time);
    }

    //modifiers begin
    if(intv_params.social_dist_elderly){
      modify_kappa_SDE_node(nodes[count]);
    }
    if(intv_params.workplace_odd_even){
	  //This is only for the old attendance implementation.  Now odd even should
	  //be implemented in the attendance file.
      modify_kappa_OE_node(nodes[count]);
    }
    if(intv_params.school_closed){
      modify_kappa_SC_node(nodes[count], intv_params.SC_factor);
    }
    if(intv_params.case_isolation){
      if(nodes[count].compliant && should_be_isolated_node(nodes[count], cur_time, SELF_ISOLATION_DAYS)){
        modify_kappa_case_isolate_node(nodes[count]);
      }
    }
	if(homes[nodes[count].home].quarantined
	   && (intv_params.home_quarantine || (intv_params.neighbourhood_containment))){
	    modify_kappa_case_isolate_node(nodes[count]);
	}
	if(nodes[count].compliant
	   && communities[nodes[count].community].quarantined
	   && intv_params.ward_containment){
		   modify_kappa_ward_containment(nodes[count]);
	}
  }
  /*
  if(intv_params.home_quarantine){
    isolate_quarantined_residents(nodes, homes, cur_time);
  }
  */
}

void get_kappa_custom(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, vector<vector<nbr_cell>>& nbr_cells, int cur_time, intervention_params intv_params){
  
  if(intv_params.home_quarantine || intv_params.neighbourhood_containment){
	  for(count_type count = 0; count<homes.size(); ++count){
		//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
		homes[count].quarantined = false;
  	}
  }
  
  if(intv_params.home_quarantine){
	
	for (count_type count = 0; count < nodes.size(); ++count){
		double time_since_symptoms = cur_time
		- (nodes[count].time_of_infection
			+ nodes[count].incubation_period
			+ nodes[count].asymptomatic_period);
		if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state) &&
		(time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
		(time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
			homes[nodes[count].home].quarantined = true;
		}
	}
  }

  if(intv_params.neighbourhood_containment && GLOBAL.ENABLE_CONTAINMENT){	
	for (count_type count = 0; count < nodes.size(); ++count){
		double time_since_hospitalised = cur_time
		- (nodes[count].time_of_infection
			+ nodes[count].incubation_period
			+ nodes[count].asymptomatic_period
			+ nodes[count].symptomatic_period);
		if(((nodes[count].entered_hospitalised_state) &&
		(time_since_hospitalised <= (HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)) ){
			homes[nodes[count].home].quarantined = true;		
			//TODO: Need to check if the nbr_cell's quarantined flag needs to be set.
			grid_cell my_nbr_grid_cell = homes[nodes[count].home].neighbourhood;
			nbr_cell my_nbr_cell = 	nbr_cells[my_nbr_grid_cell.cell_x][my_nbr_grid_cell.cell_y];	
			count_type num_homes_in_cell = my_nbr_cell.houses_list.size();
			for(count_type nbr_count = 0; nbr_count < num_homes_in_cell; ++nbr_count){
				count_type neighbour = my_nbr_cell.houses_list[nbr_count];
				homes[neighbour].quarantined = true;
			}		
		}
	}
  }
  
  if(intv_params.ward_containment && GLOBAL.ENABLE_CONTAINMENT){
	for(count_type count = 0; count<communities.size(); ++count){
		//reset all wards as non-quarantined. The status will be updated depending on the household individuals.
		communities[count].quarantined = false;
	}
	vector<count_type> num_ward_hospitalised(communities.size(),0);
	for (count_type count = 0; count < nodes.size(); ++count){
		double time_since_hospitalised = cur_time
		- (nodes[count].time_of_infection
			+ nodes[count].incubation_period
			+ nodes[count].asymptomatic_period
			+ nodes[count].symptomatic_period);
		if(((nodes[count].entered_hospitalised_state) &&
		(time_since_hospitalised <= (HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)) ){
			++num_ward_hospitalised[nodes[count].community];
		}
	}
	for (count_type count = 0; count < communities.size(); ++count){
		if(num_ward_hospitalised[count] > GLOBAL.WARD_CONTAINMENT_THRESHOLD){
			communities[count].quarantined = true;
		}
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
	//Local variables in parallel loops should be declared inside the loop so
	//that they are not shared across different parallel runs
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
		nodes[count].kappa_C = intv_params.community_factor;
		nodes[count].kappa_C_incoming = intv_params.community_factor;
	}
	//ward containment
	if(nodes[count].compliant
	   && communities[nodes[count].community].quarantined
	   && (intv_params.ward_containment && GLOBAL.ENABLE_CONTAINMENT)){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0.25;
	  nodes[count].kappa_C = 0.25;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}

	//social distancing of elderly
	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant && intv_params.social_dist_elderly){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}

	//workplace odd-even
	if(nodes[count].workplace_type==WorkplaceType::office && intv_params.workplace_odd_even){
		//odd-even rule for workplaces. 50% interactions for workplaces.
		nodes[count].kappa_W = 0.5;
		nodes[count].kappa_W_incoming = 0.5;
	} 

	//school closure
	if (nodes[count].workplace_type==WorkplaceType::school && intv_params.school_closed){
		//school and colleges are closed
		nodes[count].kappa_W = intv_params.SC_factor;
		nodes[count].kappa_W_incoming = intv_params.SC_factor;
	}

	//lockdown
	if(intv_params.lockdown){
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

	//case isolation
	if((nodes[count].compliant) && (nodes[count].entered_symptomatic_state)
	   && ((intv_params.case_isolation && !intv_params.home_quarantine) || (intv_params.case_isolation && intv_params.lockdown)) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * GLOBAL.SIM_STEPS_PER_DAY)){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;	//community value set in the beginning overwritten. 
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;	//community value set in the beginning overwritten
	}

	//home quarantine
	if(homes[nodes[count].home].quarantined
	   && (intv_params.home_quarantine || (intv_params.neighbourhood_containment && GLOBAL.ENABLE_CONTAINMENT))){
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



void get_kappa_LD_fper_CI_HQ_SD65_SC_sper_SC_tper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD){
	intervention_params intv_params;
	vector<vector<nbr_cell>> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_SC(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.school_closed = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_sper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	intervention_params intv_params;
	vector<vector<nbr_cell>> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_OE_sper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, int cur_time, double FIRST_PERIOD, double OE_SECOND_PERIOD){
	intervention_params intv_params;
	vector<vector<nbr_cell>> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+OE_SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P_SC_OE(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  intv_params.workplace_odd_even = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_intv_fper_intv_sper_intv_tper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD){
	intervention_params intv_params;
	vector<vector<nbr_cell>> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_NYC(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities,  int cur_time){
	const double FIRST_PERIOD = 3;
	const double SECOND_PERIOD = 1;
	const double THIRD_PERIOD = 3;
	const double FOURTH_PERIOD = 5;
	intervention_params intv_params;
	vector<vector<nbr_cell>> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
		intervention_params intv_params;
		intv_params.case_isolation = true;
		intv_params.school_closed = true;
		intv_params.SC_factor = 0.75;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
		intervention_params intv_params;
		intv_params.case_isolation = true;
		intv_params.school_closed = true;
		intv_params.SC_factor = 0.75;
		intv_params.community_factor = 0.75;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD+FOURTH_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
		intervention_params intv_params;
		intv_params.case_isolation = true;
		intv_params.school_closed = true;
		intv_params.SC_factor = 0;
		intv_params.community_factor = 0.75;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}else{
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_Mumbai(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, vector<vector<nbr_cell>>& nbr_cells, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	intervention_params intv_params;
	//vector<vector<nbr_cell>> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  set_compliance(nodes,homes,0.8); //compliance hard coded to 0.8 post lockdown.
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else{
	  set_compliance(nodes,homes,0.8); //compliance hard coded to 0.8 post lockdown.
	  intervention_params intv_params;
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  intv_params.SC_factor = 0;
	  intv_params.community_factor = 0.75;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_containment(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, vector<vector<nbr_cell>>& nbr_cells, int cur_time, double FIRST_PERIOD, Intervention intv){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  intervention_params intv_params;
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else{
		if(intv == Intervention::intv_nbr_containment){
			intervention_params intv_params;
	  		intv_params.case_isolation = true;
	  		intv_params.home_quarantine = true;
	  		intv_params.social_dist_elderly = true;
	  		intv_params.school_closed = true;
	  		intv_params.SC_factor = 0;
	  		intv_params.community_factor = 1;
			intv_params.neighbourhood_containment = GLOBAL.ENABLE_CONTAINMENT;
			get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
		} else{
			intervention_params intv_params;
	  		intv_params.case_isolation = true;
	  		intv_params.home_quarantine = true;
	  		intv_params.social_dist_elderly = true;
	  		intv_params.school_closed = true;
	  		intv_params.SC_factor = 0;
	  		intv_params.community_factor = 1;
			intv_params.ward_containment = GLOBAL.ENABLE_CONTAINMENT;
			get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
		}
	}	
}

void get_kappa_file_read(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, vector<vector<nbr_cell>>& nbr_cells, vector<intervention_params>& intv_params_vector, int cur_time){
  count_type time_threshold = GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS;
  count_type cur_day = cur_time/GLOBAL.SIM_STEPS_PER_DAY; //get current day. Division to avoid multiplication inside for loop.
  const auto SIZE = intv_params_vector.size();

  assert(SIZE > 0);
  assert(cur_day > time_threshold);
  count_type intv_index = 0;

  for (count_type count = 0; count < SIZE - 1; ++count){
	time_threshold += intv_params_vector[count].num_days;
	if(cur_day >= time_threshold){
	  ++intv_index;
	} else {
	  break;
	}
  }

  set_compliance(nodes,homes,intv_params_vector[intv_index].compliance);
  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_vector[intv_index]);
}
