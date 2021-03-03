//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include <cassert>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "models.h"
//#include "intervention_primitives.h"
//#include "interventions.h"
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

void modify_kappa_case_isolate_node(agent &node, const std::vector<Interaction_Space> &i_spaces, int day)
{
  node.quarantined = true;
  for (auto &ispace : node.interaction_strength[day])
  {
      switch (i_spaces[ispace.first].interaction_type)
      {
      case InteractionType ::classroom:
        node.kappa[ispace.first] = 0.0;
        break;

      case InteractionType::hostel:
        node.kappa[ispace.first] = 0.2;
        break;

      case InteractionType::mess:
        node.kappa[ispace.first] = 0.1;
        break;

      }
  }
}

void modify_kappa_class_isolate_node(agent &node, const std::vector<Interaction_Space> &i_spaces, std::vector<agent> &nodes, int day)
{
  node.quarantined = true;
  // for (auto &elem : node.interaction_strength[day])
  // {
  //     if (i_spaces[elem.first].interaction_type == InteractionType::classroom)
  //     {
  //       for (auto & individual : i_spaces[elem.first].individuals)
  //       {
  //           for (auto & person : individual){
  //             modify_kappa_case_isolate_node(nodes[person], i_spaces, day);
  //           }
  //       }
  //     }
  // }
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

void set_kappa_base_value(agent &node, const std::vector<Interaction_Space> &i_spaces, int day)
{
  for (auto &ispace : node.interaction_strength[day])
  {
      node.kappa[ispace.first] = 1;
  }
}

void set_kappa_lockdown_node(agent &node, const int cur_time, const intervention_params intv_params, const std::vector<Interaction_Space> &i_spaces, int day)
{
  node.quarantined = true; //lockdown implies quarantined
  for (auto &elem : node.interaction_strength[day])
  {
      if (i_spaces[elem.first].interaction_type == InteractionType ::hostel)
      {
        node.kappa[elem.first] = 0.2;
      }
      else
      {
        node.kappa[elem.first] = 0.0;
      }
  }
}

bool should_be_isolated_node(const agent &node, const int cur_time, const int quarantine_days)
{
  double time_since_symptoms = cur_time - (node.time_of_infection + node.incubation_period + node.asymptomatic_period);
  //std::cout<<time_since_symptoms<<"\t"<<node.entered_symptomatic_state<<"\t"<<cur_time<<"\n";
  //std::cout<<std::boolalpha<<(node.entered_symptomatic_state && (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) && (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + quarantine_days)*GLOBAL.SIM_STEPS_PER_DAY))<<"\t"<<std::boolalpha<<node.infective<<"\t";
  //printf("%boolalpha", "%boolalpha" ,(node.entered_symptomatic_state && (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) && (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + quarantine_days)*GLOBAL.SIM_STEPS_PER_DAY)), &node.infective);
  return (node.entered_symptomatic_state &&
          (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS * GLOBAL.SIM_STEPS_PER_DAY) &&
          (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + quarantine_days) * GLOBAL.SIM_STEPS_PER_DAY));
}

void get_kappa_custom_modular(std::vector<agent> &nodes,
                              const std::vector<Interaction_Space> &i_spaces,
                              const int cur_time, const intervention_params intv_params, int day)
{
  /*if(intv_params.trains_active){
    GLOBAL.TRAINS_RUNNING = true;
    GLOBAL.FRACTION_FORCED_TO_TAKE_TRAIN = intv_params.fraction_forced_to_take_train;
  }*/

  /*if(intv_params.home_quarantine || intv_params.neighbourhood_containment){
    reset_home_quarantines(homes);
  }*/

  /*if(intv_params.home_quarantine ){
    mark_homes_for_quarantine(nodes, homes, cur_time);
    //Don't isolate them yet; have to assign base kappas first.
    //These members will be isolated at the end.
  }*/

  /*if(intv_params.neighbourhood_containment){
	mark_neighbourhood_homes_for_quarantine(nodes, homes, nbr_cells, cur_time);
  }*/

  /*if(intv_params.ward_containment){
	reset_community_containment(communities);
	mark_communities_for_containment(nodes, communities,cur_time);
  }*/

#pragma omp parallel for default(none) shared(nodes, homes, communities)
  for (count_type count = 0; count < nodes.size(); ++count)
  {
    //choose base kappas
    if (intv_params.lockdown)
    {
      set_kappa_lockdown_node(nodes[count], cur_time, intv_params, i_spaces, day);
      //for(auto& ispace: nodes[count].interaction_strength[0]){
      //    std::cout<<"\n"<<ispace.first<<"\t"<<cur_time<<"\t"<<count<<"\t"<<nodes[count].kappa[ispace.first]<<"\t";
      //  }
    }
    else
    {
      set_kappa_base_value(nodes[count], i_spaces, day);
    }

    //modifiers begin
    /*if(intv_params.social_dist_elderly){
      modify_kappa_SDE_node(nodes[count]);
    }*/
    /*if(intv_params.workplace_odd_even){
	  //This is only for the old attendance implementation.  Now odd even should
	  //be implemented in the attendance file.
      modify_kappa_OE_node(nodes[count]);
    }*/
    /*if(intv_params.school_closed){
      modify_kappa_SC_node(nodes[count], intv_params.SC_factor);
    }*/
    /*std::cout<<"Outside case isolation code block"<<"\n";
    for(auto& node: nodes){
      for(auto& ispace: node.interaction_strength[0]){
        std::cout<<node.kappa[ispace.first]<<" ";
        }
    }*/
    if (intv_params.case_isolation)
    {
      //std::cout<<"Inside CI if statement\n";
      if (nodes[count].compliant && should_be_isolated_node(nodes[count], cur_time, SELF_ISOLATION_DAYS))
      {
        modify_kappa_case_isolate_node(nodes[count], i_spaces, day);
        //std::cout<<"Inside case isolation code block"<<"\n";
        /*for(auto& node: nodes){
          for(auto& ispace: node.interaction_strength[0]){
            std::cout<<node.kappa[ispace.first]<<" ";
          }
        }*/
      }
    }
    if (intv_params.class_isolation)
    {
      //std::cout<<"Inside class isolation 1st loop\n";
      if (nodes[count].compliant && should_be_isolated_node(nodes[count], cur_time, SELF_ISOLATION_DAYS))
      {
        //std::cout<<"Inside class isolation 2nd loop\n";
        modify_kappa_class_isolate_node(nodes[count], i_spaces, nodes, day);
        //for(auto& ispace: nodes[count].interaction_strength[0]){
        //  std::cout<<"\n"<<ispace.first<<"\t"<<cur_time<<"\t"<<count<<"\t"<<nodes[count].kappa[ispace.first]<<"\t";
        //}
      }
    }
    /*if(homes[nodes[count].home].quarantined
	   && (intv_params.home_quarantine || (intv_params.neighbourhood_containment))){
	    modify_kappa_case_isolate_node(nodes[count]);
	}*/
    /*if(nodes[count].compliant
	   && communities[nodes[count].community].quarantined
	   && intv_params.ward_containment){
	  modify_kappa_ward_containment(nodes[count]);
	}*/
  }
  /*
	if(intv_params.home_quarantine){
	isolate_quarantined_residents(nodes, homes, cur_time);
	}
  */
}

void get_kappa_file_read(std::vector<agent> &nodes,
                         const std::vector<Interaction_Space> &i_spaces,
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

  //set_compliance(nodes, homes, intv_params_vector[intv_index].compliance,
  //			 intv_params_vector[intv_index].compliance_hd);

  get_kappa_custom_modular(nodes, i_spaces, cur_time, intv_params_vector[intv_index], day);
}

void update_all_kappa(std::vector<agent> &nodes, const std::vector<Interaction_Space> &i_spaces, std::vector<intervention_params> &intv_params, int cur_time, int day)
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
      /*case Intervention::no_intervention:
      //get_kappa_no_intervention(nodes, homes, workplaces, communities, cur_time);
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;*/
      /*case Intervention::case_isolation:
      intv_params_local.case_isolation = true;
      //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
      get_kappa_custom_modular(nodes, homes, workplaces, communities, i_space, nbr_cells, cur_time, intv_params_local);
      break;*/
      /*case Intervention::home_quarantine:
      //get_kappa_home_quarantine(nodes, homes, workplaces, communities, cur_time);
      intv_params_local.home_quarantine = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;*/
      /*case Intervention::lockdown:
      //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
      intv_params_local.lockdown = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;*/
      /*case Intervention::case_isolation_and_home_quarantine:
      //get_kappa_CI_HQ(nodes, homes, workplaces, communities, cur_time);
      intv_params_local.case_isolation = true;
      intv_params_local.home_quarantine = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;*/
      /*case Intervention::case_isolation_and_home_quarantine_sd_65_plus:
      //get_kappa_CI_HQ_65P(nodes, homes, workplaces, communities, cur_time);
      intv_params_local.case_isolation = true;
      intv_params_local.home_quarantine = true;
      intv_params_local.social_dist_elderly = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;*/
      /*case Intervention::lockdown_fper_ci_hq_sd_65_plus_sper_ci:
      get_kappa_LOCKDOWN_fper_CI_HQ_SD_65_PLUS_sper_CI(nodes, homes, workplaces, communities, cur_time,
                                                       GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD);
      break;*/
      /*case Intervention::lockdown_fper:
      get_kappa_LOCKDOWN_fper(nodes, homes, workplaces, communities, cur_time, GLOBAL.FIRST_PERIOD);
      break;*/
      /*case Intervention::ld_fper_ci_hq_sd65_sc_sper_sc_tper:
      get_kappa_LD_fper_CI_HQ_SD65_SC_sper_SC_tper(nodes, homes, workplaces, communities, cur_time,
                                                   GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD, GLOBAL.THIRD_PERIOD);
      break;*/
      /*case Intervention::ld_fper_ci_hq_sd65_sc_sper:
      get_kappa_LD_fper_CI_HQ_SD65_SC_sper(nodes, homes, workplaces, communities, cur_time,
                                           GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD);
      break;*/
      /*case Intervention::ld_fper_ci_hq_sd65_sc_oe_sper:
      get_kappa_LD_fper_CI_HQ_SD65_SC_OE_sper(nodes, homes, workplaces, communities, cur_time,
                                              GLOBAL.FIRST_PERIOD, GLOBAL.OE_SECOND_PERIOD);
	  break;*/
      /*case Intervention::intv_fper_intv_sper_intv_tper:
      get_kappa_intv_fper_intv_sper_intv_tper(nodes, homes, workplaces, communities, cur_time,
                                                   GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD, GLOBAL.THIRD_PERIOD);
      break;*/
      /*case Intervention::intv_NYC:
      get_kappa_NYC(nodes, homes, workplaces, communities, cur_time);
      break;*/
      /*case Intervention::intv_Mum:
	  get_kappa_Mumbai_alternative_version(nodes, homes, workplaces, communities, nbr_cells, cur_time,
                                                   GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD);
      break;*/
      /*case Intervention::intv_Mum_cyclic:
      get_kappa_Mumbai_cyclic(nodes, homes, workplaces, communities, nbr_cells, cur_time,
							  GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD);
	  break;*/
    /*case Intervention::intv_nbr_containment:
      get_kappa_containment(nodes, homes, workplaces, communities, nbr_cells, cur_time, GLOBAL.FIRST_PERIOD, Intervention::intv_nbr_containment);
      break;*/
    /*case Intervention::intv_ward_containment:
      get_kappa_containment(nodes, homes, workplaces, communities, nbr_cells, cur_time, GLOBAL.FIRST_PERIOD, Intervention::intv_ward_containment);
      break;*/
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
