//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "testing.h"
using std::vector;


void set_test_request_household(vector<agent>& nodes, vector<house>& homes, count_type current_time){
  for(count_type i=0; i<nodes.size(); ++i){
	double time_since_hospitalised = current_time
                - (nodes[i].time_of_infection
                        + nodes[i].incubation_period
                        + nodes[i].asymptomatic_period
                        + nodes[i].symptomatic_period);
  	if(nodes[i].infection_status == Progression::hospitalised && time_since_hospitalised >0 && time_since_hospitalised<=1){
		for(count_type j=0; j<homes[nodes[i].home].individuals.size(); j++){
			nodes[homes[nodes[i].home].individuals[j]].test_status.test_requested = true;
		}
	}
  }
}

