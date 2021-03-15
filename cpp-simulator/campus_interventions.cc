//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include <cassert>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "models.h"
#include "campus_interventions.h"

using std::min;
using std::vector;

/*void set_compliance(std::vector<agent> & nodes, std::vector<house> & homes,
					double usual_compliance_probability, double hd_area_compliance_probability){
  //set the compliant flag for a household and it's individuals based on compliance_probability
  for(auto& node: nodes){
	auto home = node.home;
	if(node.hd_area_resident){
	  homes[home].compliant =
		(homes[home].non_compliance_metric
		 <= hd_area_compliance_probability);
	} else {
	  homes[home].compliant =
		(homes[home].non_compliance_metric
		 <= usual_compliance_probability);
	}
	node.compliant = homes[home].compliant;
  }
}*/

void modify_kappa_case_isolate_node(agent &node, std::vector<Interaction_Space> &i_spaces, int day)
{
  node.quarantined = true;
  for (auto &ispace : node.interaction_strength[day])
  {
      switch (i_spaces[ispace.first].interaction_type)
      {
      case InteractionType ::classroom:
        node.kappa[ispace.first] = std::min(GLOBAL.kappa_class_case_isolation, node.kappa[ispace.first]) ;
        break;

      case InteractionType::hostel:
        node.kappa[ispace.first] = std::min(GLOBAL.kappa_hostel_case_isolation,node.kappa[ispace.first]);
        break;

      case InteractionType::mess:
        node.kappa[ispace.first] = std::min(GLOBAL.kappa_mess_case_isolation, node.kappa[ispace.first]);
        break;
      
      case InteractionType :: cafeteria:
        node.kappa[ispace.first] = std::min(GLOBAL.kappa_cafe_case_isolation, node.kappa[ispace.first]); 
        break;
      
      case InteractionType :: smaller_networks:
        node.kappa[ispace.first] = std::min(GLOBAL.kappa_smaller_networks_case_isolation, node.kappa[ispace.first]);
        break;
      }
  }
}

void modify_kappa_class_isolate_node(agent &node, std::vector<Interaction_Space> &i_spaces, std::vector<agent> &nodes, int day)
{
  node.quarantined = true;
  for (auto &space : node.spaces){
  	if(i_spaces[space].interaction_type == InteractionType::classroom){
  		for(auto &individual : i_spaces[space].individuals){
  			for(auto &person : individual){
  				modify_kappa_case_isolate_node(nodes[person], i_spaces, day);
  			}
  		}
  	}
  }
}

void set_kappa_base_value(agent &node, std::vector<Interaction_Space> &i_spaces, int day)
{
  for (auto &ispace : node.interaction_strength[day])
  {
      node.kappa[ispace.first] = GLOBAL.kappa_base_value;
  }
}

void set_kappa_lockdown_node(agent &node, const int cur_time, const intervention_params intv_params, std::vector<Interaction_Space> &i_spaces, int day)
{
  node.quarantined = true; //lockdown implies quarantined
  for (auto &elem : node.interaction_strength[day])
  {
      if (i_spaces[elem.first].interaction_type == InteractionType ::hostel)
      {
        node.kappa[elem.first] = std::min(GLOBAL.kappa_hostel_lockdown, node.kappa[elem.first]);
      }
      else
      {
        node.kappa[elem.first] = std::min(GLOBAL.kappa_default_lockdown, node.kappa[elem.first]);
      }
  }
}

void reset_class_quarantines(std::vector<Interaction_Space>& i_space){
  for(count_type count = 0; count<i_space.size(); ++count){
    i_space[count].quarantined = false;
  }
}

void mark_class_for_quarantine(const std::vector<agent>& nodes, std::vector<Interaction_Space> &i_space, const int cur_time){
  //mark all classes for quarantine
  for (count_type count = 0; count < nodes.size(); ++count){
    if(nodes[count].compliant && should_be_isolated_node(nodes[count],cur_time,SELF_ISOLATION_DAYS)){
       for (auto &space : nodes[count].spaces){
         if (i_space[space].interaction_type == InteractionType::classroom){
           i_space[space].quarantined = true;
         }
       }
     }
  }
}

bool should_be_isolated_node(const agent &node, const int cur_time, const int quarantine_days)
{
  double time_since_symptoms = cur_time - (node.time_of_infection + node.incubation_period + node.asymptomatic_period);
  return (node.entered_symptomatic_state &&
          (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS * GLOBAL.SIM_STEPS_PER_DAY) &&
          (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + quarantine_days) * GLOBAL.SIM_STEPS_PER_DAY));
}

void get_kappa_custom_modular(std::vector<agent> &nodes,
                              std::vector<Interaction_Space> &i_spaces,
                              const int cur_time, const intervention_params intv_params, int day)
{
  if (intv_params.class_isolation){
    reset_class_quarantines(i_spaces);
    mark_class_for_quarantine(nodes, i_spaces, cur_time);
  }
#pragma omp parallel for default(none) shared(nodes, homes, communities)
  for (count_type count = 0; count < nodes.size(); ++count)
  {
    bool quarantine_flag = false;
    //choose base kappas
     if (intv_params.lockdown)
    {
      set_kappa_lockdown_node(nodes[count], cur_time, intv_params, i_spaces, day);
    }
    else
    {
      set_kappa_base_value(nodes[count], i_spaces, day);
    }
    if (intv_params.case_isolation)
    {

      if (nodes[count].compliant && should_be_isolated_node(nodes[count], cur_time, SELF_ISOLATION_DAYS))
      {
        modify_kappa_case_isolate_node(nodes[count], i_spaces, day);
      }
    }
    if (intv_params.class_isolation)
    {
      for (auto &space : nodes[count].spaces){
         if (i_spaces[space].interaction_type == InteractionType::classroom){
           if (i_spaces[space].quarantined == true){
             quarantine_flag = true;
             break;
           }
         }
       }
      if (quarantine_flag)
      {
        modify_kappa_case_isolate_node(nodes[count], i_spaces, day);
      }
    }
  }
}

void get_kappa_file_read(std::vector<agent> &nodes,
                         std::vector<Interaction_Space> &i_spaces,
                         const std::vector<intervention_params> &intv_params_vector, int cur_time, int day)
{
  count_type time_threshold = GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS;
  count_type cur_day = cur_time / GLOBAL.SIM_STEPS_PER_DAY; //get current day. Division to avoid multiplication inside for loop.
  const auto SIZE = intv_params_vector.size();

  assert(SIZE > 0);
  assert(cur_day >= time_threshold);
  count_type intv_index = 0;

  for (count_type count = 0; count < SIZE - 1; ++count)
  {
    time_threshold += intv_params_vector[count].num_days;
    if (cur_day >= time_threshold)
    {
      ++intv_index;
    }
    else
    {
      break;
    }
  }
  get_kappa_custom_modular(nodes, i_spaces, cur_time, intv_params_vector[intv_index], day);
}

void update_all_kappa(std::vector<agent> &nodes, std::vector<Interaction_Space> &i_spaces, std::vector<intervention_params> &intv_params, int cur_time, int day)
{
  intervention_params intv_params_local;
  if (cur_time < GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS * GLOBAL.SIM_STEPS_PER_DAY)
  {
    //get_kappa_no_intervention(nodes, homes, workplaces, communities,cur_time);
    get_kappa_custom_modular(nodes, i_spaces, cur_time, intv_params_local, day);
  }
  else
  {
    switch (GLOBAL.INTERVENTION)
    {
    case Intervention::intv_file_read:
      get_kappa_file_read(nodes, i_spaces, intv_params, cur_time, day);
      break;
    default:
      //get_kappa_no_intervention(nodes, homes, workplaces, communities, cur_time);
      get_kappa_custom_modular(nodes, i_spaces, cur_time, intv_params_local, day);
      break;
    }
  }
}
