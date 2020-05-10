//Copyright [2020] [Indian Institute of Science, Bangalore]
//SPDX-License-Identifier: Apache-2.0
//
// Centre for Networked Intelligence, EECS-RBCCPS, Indian Institute of Science Bangalore
// Calculator to gauge the readiness of workplace to re-open with precautions for
// avoiding spread of COVID-19
//
// Prevent forms from submitting.
function preventFormSubmit() {
  var forms = document.querySelectorAll('form');
  for (var i = 0; i < forms.length; i++) {
    forms[i].addEventListener('submit', function(event) {
    event.preventDefault();
    });
  }
}
window.addEventListener('load', preventFormSubmit);    

function onSuccess (scoreMsg) {
    document.getElementById("output").innerHTML=scoreMsg;
    document.getElementById("output").style.display = "inline";
}

function getValues(){
  var dict = new Object();
  
  // Nature of Establishment
  dict["cmpName"] = document.getElementById("cmpName").value; // Company Name
  dict["emailAddr"] = document.getElementById("emailAddr").value; // Company Address
  dict["NOE"] = parseInt(document.getElementById("NOE").value); // Nature of Establishment
  
  // Employee Information
  dict["nShifts"] = parseInt(document.getElementById("nShifts").value); // Number of shifts
  for (var i=0; i<dict["nShifts"]; ++i){
    dict["nM_"+(i+1).toString()] = parseInt(document.getElementById("nM_"+(i+1).toString()).value); // Number of male employees in shift i
    dict["nF_"+(i+1).toString()] = parseInt(document.getElementById("nF_"+(i+1).toString()).value); // Number of female employees in shift i
    dict["nOth_"+(i+1).toString()] = parseInt(document.getElementById("nOth_"+(i+1).toString()).value); // Number of other employees in shift i
    dict["pCS_"+(i+1).toString()] = parseInt(document.getElementById("pCS_"+(i+1).toString()).value); // Percentage of casual labour and security in shift i
  }

  dict["tGapShift"] = parseFloat(document.getElementById("tGapShift").value); // Time gap between shifts in hours
  dict["informCZEmp"] = parseInt(document.querySelector('input[name="informCZEmp"]:checked').value); // Inform containment zone employee not to come
  dict["informWFH"] = parseInt(document.querySelector('input[name="informWFH"]:checked').value); // Encourage work from home 
  dict["n29"] = parseInt(document.getElementById("n29").value); // Number of employees with age betwwen 15 and 29
  dict["n49"] = parseInt(document.getElementById("n49").value); // Number of employees with age betwwen 30 and 49
  dict["n64"] = parseInt(document.getElementById("n64").value); // Number of employees with age betwwen 50 and 64
  dict["n65plus"] = parseInt(document.getElementById("n65plus").value); // Number of employees with age more than 65

  // Office Infrastructure Information
  dict["nBldng"] = parseInt(document.getElementById("nBldng").value); // Number of buildings 
  dict["tArea"] = parseInt(document.getElementById("tArea").value); // Total office area in sq.ft
  dict["nFloors"] = parseInt(document.getElementById("nFloors").value); // Number of floors in office
  dict["opnCubArea"] = parseInt(document.getElementById("opnCubArea").value); // Total open cubicle area in sq.ft
  dict["mntrCCTV"] = parseInt(document.querySelector('input[name="mntrCCTV"]:checked').value); // CCTV monitoring
  dict["acsCntrl"] = parseInt(document.querySelector('input[name="acsCntrl"]:checked').value); // Access controlled
  dict["baDoor"] = parseInt(document.getElementById("baDoor").value); // Number of biometric based access doors
  dict["rfidDoor"] = parseInt(document.getElementById("rfidDoor").value); // Number of RFID based access doors
  dict["sntBio"] = parseInt(document.querySelector('input[name="sntBio"]:checked').value); // Sanitiser at Biometric access doors
  dict["nCW"] = parseInt(document.getElementById("nCW").value); // Number of companies in Cowork space

  // Office Composition
  dict["nSOfcRm"] = parseInt(document.getElementById("nSOfcRm").value); // Number of single office rooms
  dict["n2pOfcRm"] = parseInt(document.getElementById("n2pOfcRm").value); // Number of office rooms with 2 people
  dict["n2pPlusOfcRm"] = parseInt(document.getElementById("n2pPlusOfcRm").value); // Number of office rooms with more than 2 people
  dict["nCub"] = parseInt(document.getElementById("nCub").value); // Number of office seats with cubicle separation
  dict["nRem"] = parseInt(document.getElementById("nRem").value); // Number of remaining office seats
  dict["percAc"] = parseInt(document.getElementById("percAC").value); // Percentage of air conditioned premise
  dict["centralAC"] = parseInt(document.querySelector('input[name="centralAC"]:checked').value); // centralised A/C
  dict["tempAC"] = parseInt(document.getElementById("tempAC").value); // Temperature setting
  dict["hepaFltr"] = parseInt(document.querySelector('input[name="hepaFltr"]:checked').value); // HEPA filter

  // Common Usage Area
  dict["nEle"] = parseInt(document.getElementById("nEle").value); // Number of Elevetors
  dict["eleCpct"] = parseInt(document.getElementById("eleCpct").value); // Average capacity of Elevetors
  dict["nEleDinf"] = parseInt(document.getElementById("nEleDinf").value); // Frequency of elevetor disinfection process
  dict["nStrCln"] = parseInt(document.getElementById("nStrCln").value); // Frequency of stairway cleaning
  dict["nStrHrDinf"] = parseInt(document.getElementById("nStrHrDinf").value); // Frequency of stairway handrails disinfection process
  dict["oMtngSpts"] = parseInt(document.getElementById("oMtngSpts").value); // Other meeting spaces
  dict["freqCln"] = parseInt(document.getElementById("freqCln").value); // Frequency of cleaning
  dict["nHskpngStff"] = parseInt(document.getElementById("nHskpngStff").value); // Number of housekeeping staff

  // Epidemic related precautions
  dict["tempScreening"] = parseInt(document.querySelector('input[name="tempScreening"]:checked').value); // Temperature screening of employee
  dict["faceCover"] = parseInt(document.querySelector('input[name="faceCover"]:checked').value); // Face is covered with mask
  dict["adqFaceCover"] = parseInt(document.querySelector('input[name="adqFaceCover"]:checked').value); // Adequate Facecover quantity
  dict["newShfts"] = parseInt(document.querySelector('input[name="newShfts"]:checked').value); // New shifts/staggered work timings
  dict["advAvdLPM"] = parseInt(document.querySelector('input[name="advAvdLPM"]:checked').value); // Avoid large physical meetings
  dict["nHsS"] = parseInt(document.getElementById("nHsS").value); // Number of hand-sanitiser stations
  dict["tchFree"] = parseInt(document.querySelector('input[name="tchFree"]:checked').value); // Hand sanitisers touch free
  dict["nDinf"] = parseInt(document.getElementById("nDinf").value); // Number of disinfection activity per day
  dict["smkZS"] = parseInt(document.querySelector('input[name="smkZS"]:checked').value); // Smoking zone sealed
  dict["nPGT"] = parseInt(document.getElementById("nPGT").value); // Number of employees consuming Pan masala, gutkha, tobacco 
  dict["nWsB"] = parseInt(document.getElementById("nWsB").value); // Number of warning sign boards
  dict["nASapp"] = parseInt(document.getElementById("nASapp").value); // Number of employees with Aarogya Setu app

  // Isolation room
  dict["hl"] = parseInt(document.querySelector('input[name="hl"]:checked').value); // Company helpline
  dict["iQS"] = parseInt(document.querySelector('input[name="iQS"]:checked').value); // Immediate quarantine space
  dict["amblnc"] = parseInt(document.querySelector('input[name="amblnc"]:checked').value); // Ambulance facility
  dict["lHsptl"] = parseInt(document.querySelector('input[name="lHsptl"]:checked').value); // List of nearby hospitals etc.
  dict["emrgncResp"] = parseInt(document.querySelector('input[name="emrgncResp"]:checked').value); // Emergency Response
  dict["alrg"] = parseInt(document.querySelector('input[name="alrg"]:checked').value); // Employee allergy list
  dict["imdtFM"] = parseInt(document.querySelector('input[name="imdtFM"]:checked').value); // Immediate family members list
  dict["lstUpdtTime"] = parseInt(document.getElementById("lstUpdtTime").value); // Last information update time
  dict["medInsurance"] = parseInt(document.querySelector('input[name="medInsurance"]:checked').value); // Do all employees have medical insurance

  // Advertisement and Outreach
  dict["covidPage"] = parseInt(document.querySelector('input[name="covidPage"]:checked').value); // Covid Awareness Page
  dict["faq"] = parseInt(document.querySelector('input[name="faq"]:checked').value); // FAQ Page
  dict["sPers"] = parseInt(document.querySelector('input[name="sPers"]:checked').value); // Cleaniness and safety person
  dict["advSclDis"] = parseInt(document.querySelector('input[name="advSclDis"]:checked').value); // Advised social distancing
  dict["advWFHVul"] = parseInt(document.querySelector('input[name="advWFHVul"]:checked').value); // Advised work from home to elderly and women
  dict["snzCvr"] = parseInt(document.querySelector('input[name="snzCvr"]:checked').value); // Advised to cover sneeze
  dict["hkTrn"] = parseInt(document.querySelector('input[name="hkTrn"]:checked').value); // House-keeping staff training
  dict["sPrgm"] = parseInt(document.querySelector('input[name="sPrgm"]:checked').value); // Special program on COVID-19
  dict["pstrs"] = parseInt(document.querySelector('input[name="pstrs"]:checked').value); // Prosters on hygiene and social distancing

  // Employee Interactions
  dict["nVstrs"] = parseInt(document.getElementById("nVstrs").value); // Number of visitors
  dict["nEmpCstmr"] = parseInt(document.getElementById("nEmpCstmr").value); // Number of employees that meet with customer
  dict["nDlvrHndlng"] = parseInt(document.getElementById("nDlvrHndlng").value); // Number of employees handling deliveries 
  dict["msk"] = parseInt(document.querySelector('input[name="msk"]:checked').value); // Employees wear mask
  dict["glvs"] = parseInt(document.querySelector('input[name="glvs"]:checked').value); // Employess wear mask and gloves

  // Mobility Related
  dict["nHM"] = parseInt(document.getElementById("nHM").value); // Number of employees that are more than 2 hours aways
  dict["nMM"] = parseInt(document.getElementById("nMM").value); // Number of employees that are more than 1 hour aways 
  dict["nMPD"] = parseFloat(document.getElementById("nMPD").value); // Meetings per day
  dict["avgMS"] = parseFloat(document.getElementById("avgMS").value); // Average number of members in the meeting
  
  // Cafeteria/Pantry
  dict["extFSP"] = parseInt(document.querySelector('input[name="extFSP"]:checked').value); // External food vendor
  dict["cntn"] = parseInt(document.querySelector('input[name="cntn"]:checked').value); // Canteen/pantry
  dict["cntnACOp"] = parseInt(document.querySelector('input[name="cntnACOp"]:checked').value); // Canteen/pantry air condition operational
  dict["nBrkfst"] = parseInt(document.getElementById("nBrkfst").value); // Number of employees having breakfast in canteen
  dict["nLnch"] = parseInt(document.getElementById("nLnch").value); // Number of employees having lunch in canteen
  dict["nSnck"] = parseInt(document.getElementById("nSnck").value); // Number of employees having snacks/coffee in canteen
  dict["dBrkfst"] = parseInt(document.getElementById("dBrkfst").value); // Duration for which breakfast is served
  dict["dLnch"] = parseInt(document.getElementById("dLnch").value); // Duration for which lunch is served
  dict["dSnck"] = parseInt(document.getElementById("dSnck").value); // Duration for which coffee/snacks is served
  dict["nEmpHL"] = parseInt(document.getElementById("nEmpHL").value); // Number of employees who bring lunch from home
  dict["mlStggrd"] = parseInt(document.querySelector('input[name="mlStggrd"]:checked').value); // Meals staggered
  dict["utnslShrd"] = parseInt(document.querySelector('input[name="utnslShrd"]:checked').value); // Utensils shared
  dict["cntnArea"] = parseInt(document.getElementById("cntnArea").value); // Canteen area in sq.ft
  dict["mxCntnPpl"] = parseInt(document.getElementById("mxCntnPpl").value); // Maximum number of employees allowed in canteen at a time
  dict["nWS"] = parseInt(document.getElementById("nWS").value); // Number of water stations
  
  // Washroom Information
  dict["nGntsT"] = parseInt(document.getElementById("nGntsT").value); // Number of gents toilet
  dict["nLdsT"] = parseInt(document.getElementById("nLdsT").value); // Number of ladies toilet
  dict["tClnFreq"] = parseInt(document.getElementById("tClnFreq").value); // Frequency of toilet cleaning
  dict["spPrsnt"] = parseInt(document.querySelector('input[name="spPrsnt"]:checked').value); // Soap dispensed in toilet
  dict["typeSanitation"] = parseInt(document.querySelector('input[name="typeSanitation"]:checked').value); //  Type of sanitation

  // Company Provided Transport
  dict["bsCpctAct"] = parseInt(document.getElementById("bsCpctAct").value); // Actual Bus capacity
  dict["bsCpctCur"] = parseInt(document.getElementById("bsCpctCur").value); // Current Bus capacity
  dict["mnBsCpctAct"] = parseInt(document.getElementById("mnBsCpctAct").value); // Actual Mini bus capacity 
  dict["mnBsCpctCur"] = parseInt(document.getElementById("mnBsCpctCur").value); // Current Mini bus capacity 
  dict["vnCpctAct"] = parseInt(document.getElementById("vnCpctAct").value); // Actual Van capacity 
  dict["vnCpctCur"] = parseInt(document.getElementById("vnCpctCur").value); // Current Van capacity 
  dict["svCpctAct"] = parseInt(document.getElementById("svCpctAct").value); // Actual SUV capacity 
  dict["svCpctCur"] = parseInt(document.getElementById("svCpctCur").value); // Current SUV capacity 
  dict["crCpctAct"] = parseInt(document.getElementById("crCpctAct").value); // Actual Car capacity
  dict["crCpctCur"] = parseInt(document.getElementById("crCpctCur").value); // Current Car capacity
  dict["mskMndt"] = parseInt(document.querySelector('input[name="mskMndt"]:checked').value); // Mask mandate
  dict["hsVhcl"] = parseInt(document.querySelector('input[name="hsVhcl"]:checked').value); // Hand sanitiser in vehicle
  dict["noACVhcl"] = parseInt(document.querySelector('input[name="noACVhcl"]:checked').value); // No AC use in vehicle
  dict["nTrnsptSnt"] = parseInt(document.getElementById("nTrnsptSnt").value); // Number of times transport bay is sanitised
  dict["drvSrnd"] = parseInt(document.querySelector('input[name="drvSrnd"]:checked').value); // Drivers screened
  dict["vhclSnt"] = parseInt(document.querySelector('input[name="vhclSnt"]:checked').value); // Vehicles sanitised
  dict["trvlr5K"] = parseInt(document.getElementById("trvlr5K").value); // Numbers travelling 0-5 km
  dict["trvlr10K"] = parseInt(document.getElementById("trvlr10K").value); // Numbers travelling 5-10 km
  dict["trvlr10Kplus"] = parseInt(document.getElementById("trvlr10Kplus").value); // Numbers travelling >10 km

  // Self-owned vehicle transport
  dict["noPlnR"] = parseInt(document.querySelector('input[name="noPlnR"]:checked').value); // No pillion riders
  dict["no2plusTrvl"] = parseInt(document.querySelector('input[name="no2plusTrvl"]:checked').value); // No 2 plus travel in car
  dict["hsLbb"] = parseInt(document.querySelector('input[name="hsLbb"]:checked').value); // Hand sanitiser in the lobby
  dict["mskCar"] = parseInt(document.querySelector('input[name="mskCar"]:checked').value); // Mask in car
  dict["trvlr5Kslf"] = parseInt(document.getElementById("trvlr5Kslf").value); // Numbers travelling 0-5 km private vehicle
  dict["trvlr10Kslf"] = parseInt(document.getElementById("trvlr10Kslf").value); // Numbers travelling 5-10 km private vehicle
  dict["trvlr10Kplusslf"] = parseInt(document.getElementById("trvlr10Kplusslf").value); // Numbers travelling >10 km private vehicle

  // Walking 
  dict["nWlk"] = parseInt(document.getElementById("nWlk").value); // Number of employees who walk to work
  dict["mskWlk"] = parseInt(document.querySelector('input[name="mskWlk"]:checked').value); // Mask while walking

  // Public Transport
  dict["mskPub"] = parseInt(document.querySelector('input[name="mskPub"]:checked').value); // Mask while in public transport
  dict["trvlr5Kpub"] = parseInt(document.getElementById("trvlr5Kpub").value); // Numbers travelling 0-5 km private vehicle
  dict["trvlr10Kpub"] = parseInt(document.getElementById("trvlr10Kpub").value); // Numbers travelling 5-10 km private vehicle
  dict["trvlr10Kpluspub"] = parseInt(document.getElementById("trvlr10Kpluspub").value); // Numbers travelling >10 km private vehicle

  // Time to reach office
  dict["n30Min"] = parseInt(document.getElementById("n30Min").value); // Number of employees taking 0-30 minutes
  dict["n60Min"] = parseInt(document.getElementById("n60Min").value); // Number of employees taking 30-60 minutes

  return dict;
}

function clipAndRound_bounds (score) {
  var score_out = Math.round(score/10);
  if (score_out < 0 ) {
    score_out = 0;
  } else if (score_out > 100 ) {
    score_out = 100;
  }
  return score_out;
}

function sg_update(msg_count, current_sg, msg){
  msg_count += 1;
  current_sg = (current_sg=="") ? "" : current_sg + "<br>";
  current_sg += msg;
  return ([msg_count, current_sg])
}

function calcScore () {
  inputs = getValues(); //Read values from html page...

  var number_of_suggestions = 3;
  var suggestion = "";

  // Office Infrastructure
  var nEmp = inputs["nM_1"] + inputs["nF_1"] + inputs["nOth_1"];
  var nMeets = 4;
  var cFactor = 1;
  if(inputs["opnCubArea"]>0){
    cFactor = Math.max( ((inputs["nCub"] + inputs["nRem"])*40 / inputs["opnCubArea"]), 1 ); 
  }
  var crowding = 2*inputs["n2pOfcRm"] * nMeets + 3*inputs["n2pPlusOfcRm"] * nMeets * 1.2 +
      (inputs["nCub"] + inputs["nRem"] * 1.2) * nMeets * cFactor;
  var stairsElev = 0.5 * inputs["nFloors"] * (1 + inputs["eleCpct"]/2 * (1-0.1*inputs["advSclDis"]))/2 * 4 * Math.max( (1 - 0.1*Math.min( inputs["nStrCln"], inputs["nEleDinf"] )), 0.5);
  var bmFlag = ((inputs["baDoor"] >= 1) ? 1 : 0);
  var accessContacts = 4 * ( 1 - 0.1*((bmFlag * inputs["sntBio"]) + inputs["mntrCCTV"]) ) * bmFlag;
  var shiftDelta = ( (inputs["tGapShift"] > 8) ? 0 : (8 - inputs["tGapShift"]));
  var premisesContacts = 0;
  if (nEmp>0){
    premisesContacts = (accessContacts + stairsElev + crowding / nEmp ) * (1 + (shiftDelta/8.0)) * (1-0.4*inputs["msk"]);
  }

  var nominal_office_infra_raw_score = 31;
  var nominal_office_infra_scaled_score = 800;

  // Other meeting spaces
  var score_other_spaces = 0.5 * inputs["oMtngSpts"] * Math.max(1-0.1*(inputs["freqCln"]*Math.min(1, inputs["nHskpngStff"])), 0.5) * (1-0.4*inputs["msk"]);
  var score_office_infra = 0;
  if (premisesContacts+score_other_spaces>0 && !isNaN(premisesContacts+score_other_spaces)){
    score_office_infra = nominal_office_infra_raw_score*nominal_office_infra_scaled_score/(premisesContacts + score_other_spaces);
  }
  score_office_infra = clipAndRound_bounds(score_office_infra);

  var sg_office_infra = "Well done!"
  if (nEmp==0){
    sg_office_infra = "You have entered the total number of employees in a shift to be zero. This is invalid. Please go back and re-enter the correct number of employees for each shift.";
  } else if (score_office_infra<70){
    sg_office_infra = "Consider encouraging more employees to work from home or shifts";
  } else if (inputs["nEleDinf"]<2) {
    sg_office_infra = "Consider cleaning the lifts more often";
  }

  // Toilet scores
  var nGntsTlt = inputs["nM_1"] + inputs["nOth_1"]/2.0; 
  var nLdsTlt = inputs["nF_1"] + inputs["nOth_1"]/2.0; 
  var avgTltVstsPrDy = 5; 
  var avgTltDrtn = 4;
  var tltCnctrtnHrs = 4; 

  var nominal_raw_aggrToiletSc = 800;
  var nominal_scaled_aggrToiletSc = 1;

  var aggrToiletSc = 1000;
  if (inputs["nGntsT"] || inputs["nLdsT"]){
  var cRateGentsToilet = nGntsTlt * avgTltVstsPrDy * avgTltDrtn * (Math.max(0.5, (1.0 - 0.1*inputs["tClnFreq"]))) * (1.0 - 0.1*inputs["spPrsnt"]) / (tltCnctrtnHrs*60*inputs["nGntsT"]);
  var cRateLadiesToilet = nLdsTlt * avgTltVstsPrDy * avgTltDrtn * (Math.max(0.5, (1.0 - 0.1*inputs["tClnFreq"]))) * (1.0 - 0.1*inputs["spPrsnt"]) / (tltCnctrtnHrs*60*inputs["nLdsT"]);
  aggrToiletSc = nominal_raw_aggrToiletSc*nominal_scaled_aggrToiletSc/(Math.max(cRateLadiesToilet, cRateGentsToilet)+0.001);
  }
  var score_sanitation = clipAndRound_bounds(aggrToiletSc);

  var sg_sanitation = "Well done!";
  if (!inputs["nGntsT"] && !inputs["nLdsT"]){
    sg_sanitation = "High score. But do you really have no toilets in your workplace?";
  } else if (score_sanitation < 70) {
      sg_sanitation = "Disinfect toilets more often or consider reducing the employees per shift";
  }

  // Sick Rooms/Isolation Ward
  var score_sickRoom = 100*(inputs["iQS"]*2 + inputs["amblnc"] + inputs["lHsptl"]*2 + inputs["emrgncResp"] + inputs["hl"] + 
                            inputs["imdtFM"] + inputs["alrg"] * Math.max( 0, (1.0 - inputs["lstUpdtTime"]/60))*2);
  var score_isolation = clipAndRound_bounds(score_sickRoom);
  var count = 0;
  var sg_isolation = "";
  if (!inputs["iQS"]){
    count += 1;
    sg_isolation = (sg_isolation=="") ? "" : sg_isolation + "<br>";
    sg_isolation += "Designate a well-ventilated place for immediate quarantine.";
  }
  if (!inputs["lHsptl"] && count<number_of_suggestions){
    count += 1;
    sg_isolation = (sg_isolation=="") ? "" : sg_isolation + "<br>";
    sg_isolation += "Prepare a list of COVID-19 ready hospitals.";
  } 
  if (!inputs["amblnc"] && count<number_of_suggestions){
    count += 1;
    sg_isolation = (sg_isolation=="") ? "" : sg_isolation + "<br>";
    sg_isolation += "Keep an ambulance on stand-by.";
  } 
  if (!inputs["emrgncResp"] && count<number_of_suggestions) {
    count += 1;
    sg_isolation = (sg_isolation=="") ? "" : sg_isolation + "<br>";
    sg_isolation += "Give instructions to transport department on COVID-19 emergency.";
  }
  if (!inputs["hl"] && count<number_of_suggestions) {
    count += 1;
    sg_isolation = (sg_isolation=="") ? "" : sg_isolation + "<br>";
    sg_isolation += "Consider having a dedicated COVID-19 helpline.";
  }
  if (Math.max( 0, (1.0 - inputs["lstUpdtTime"]/60)) < 0.5 && count <number_of_suggestions) {
      count += 1;
      sg_isolation = (sg_isolation=="") ? "" : sg_isolation + "<br>";
      sg_isolation += "Update medical history of employees.";
  }
  if (!inputs["imdtFM"] && count<number_of_suggestions){
    count += 1;
    sg_isolation = (sg_isolation=="") ? "" : sg_isolation + "<br>";
    sg_isolation += "Consider preparing a list of immediate family members and caretakers of employees.";
  }
  if (!inputs["alrg"] && count<number_of_suggestions){
    sg_isolation = (sg_isolation=="") ? "" : sg_isolation + "<br>";
    sg_isolation += "Consider preparing a list of employees' medical history.";
  }
  if (sg_isolation==""){
    sg_isolation = "Well done!";
  }

  // Cafeteria/Pantry
  var time_brkfst = 15; // In minutes
  var time_lnch = 30;
  var time_snck = 15;
  var time_wtr = 2;
  var num_wtr_sought = 5;
  var prsnl_area = 28; // In sq. feet: Approx. 3 feet radius
  var pvtl_hrs = 4;
  var nOutside = 0;

  var score_cafeteria_scaled = 1000;
  var nominal_raw_cafeteria_score = 2;
  var nominal_scaled_cafeteria_score = 700;

  var mtng_brkfst = 0;
  var mtng_lnch = 0;
  var mntg_snck = 0;
  var mntg_wtr = 0;

  if (inputs["cntnArea"]>0 && inputs["cntn"]){
    nOutside = nEmp - inputs["nLnch"] - inputs["nEmpHL"];
    mtng_brkfst = (inputs["nBrkfst"] * time_brkfst * prsnl_area) / (inputs["cntnArea"] * (inputs["dBrkfst"] ? inputs["dBrkfst"] : 60));
    mtng_lnch = (inputs["nLnch"] * time_lnch * prsnl_area) / (inputs["cntnArea"] * (inputs["dLnch"] ? inputs["dLnch"] : 90));
    mntg_snck = (inputs["nSnck"] * time_snck * prsnl_area) / (inputs["cntnArea"] * (inputs["dSnck"] ? inputs["dSnck"] : 60));
    if (inputs["nWS"]>0){
      mntg_wtr = (nEmp * time_wtr * num_wtr_sought * prsnl_area) / (inputs["nWS"] * inputs["cntnArea"] * pvtl_hrs *  60);
    }
  } else {
    nOutside = nEmp - inputs["nEmpHL"];
    if (inputs["nWS"]>0){
      mntg_wtr = (nEmp * time_wtr * num_wtr_sought * prsnl_area) / (inputs["nWS"] * 144 * pvtl_hrs *  60);
    }
  }

  var score_cafeteria_scaled = 0;
  if (nEmp>0){
    var score_cafeteria = (mtng_brkfst*(inputs["nBrkfst"]/nEmp) + mtng_lnch*(inputs["nLnch"]/nEmp) + mntg_snck*(inputs["nSnck"]/nEmp) + mntg_wtr + (inputs["nEmpHL"]*0.25/nEmp) + (nOutside*2/nEmp) + (nOutside*0.5*(inputs["extFSP"])/nEmp));
    score_cafeteria = score_cafeteria * Math.max((1 - 0.1*( inputs["mlStggrd"] + inputs["freqCln"] - inputs["utnslShrd"])), 1/2);
    score_cafeteria_scaled = nominal_scaled_cafeteria_score*nominal_raw_cafeteria_score/score_cafeteria;
    // We prefer the score_cafeteria to be less than 2
  }

  var score_cafeteria_scaled = clipAndRound_bounds(score_cafeteria_scaled);
  var sg_cafeteria = "";
  if ((!inputs["cntnArea"]>0 || !inputs["cntn"]) && nEmp>0){
    sg_cafeteria = "No cafeteria/pantry/kitchen area are not functional. <br>Score is based on interaction during the following activities: <br>Using water dispenser, having lunch from outside and lunch brought from home.";
  } else if (nEmp==0){
    sg_cafeteria = "You have entered the total number of employees in a shift to be zero. This is invalid.";
  } else if (score_cafeteria_scaled<70){
    sg_cafeteria = "You have overcrowding in your cafeteria/pantry. <br>Consider staggered cafeteria/pantry timings. <br>Encourage employees to work from home.";
  } else if (nOutside/nEmp > 0.5 && score_cafeteria_scaled<60){
    sg_cafeteria = "You have too many outside contacts during lunch time. Encourage employees to bring lunch from home or provide lunch on premises."
  } else {
    sg_cafeteria = "Well done!";
  }

  // Mobility
  var nLM = nEmp - inputs["nHM"] - inputs["nMM"]
  var score_mobility = ((0.25*nLM + 0.5*inputs["nMM"] + inputs["nHM"])/(nLM+inputs["nMM"]+inputs["nHM"])) * (1-0.4*inputs["msk"]);
  var nominal_raw_score_mobility = 0.85;
  var nominal_scaled_score_mobility = 1000;
  score_mobility = (1-score_mobility)*nominal_scaled_score_mobility/nominal_raw_score_mobility;
  score_mobility = clipAndRound_bounds(score_mobility);

  var sg_mobility = "Well done!";
  if (!inputs["msk"]){
    sg_mobility = "Consider mandating masks while interacting with other employees";
  } else if (score_mobility<70){
    sg_mobility = "Consider allowing a few high mobility users to work from home";
  }

  var score_meetings = 1000;
  // Meetings
  if (inputs["nMPD"] && inputs["avgMS"]){
    score_meetings = 1000*nEmp/(inputs["nMPD"] * Math.pow(inputs["avgMS"], 1.2) * (1-0.4*inputs["msk"]));
  }
  score_meetings = clipAndRound_bounds(score_meetings);

  var sg_meetings = "Well done!";
  if (!inputs["msk"]){
    sg_meetings = "Consider making mask mandatory in all the meetings";
  } else if (score_meetings<70){
    sg_meetings = "Consider shifting to online meetings";
  } else if (inputs["avgMS"]>5) {
    sg_meetings = "Consider reducing number of employees per meeting";
  }

  // Outside contacts
  var score_outside = 1000;
  var sg_outside = "";
  if (inputs["nVstrs"] && inputs["nEmpCstmr"]){
    score_outside =  100*nEmp/(inputs["nVstrs"] * Math.pow(inputs["nEmpCstmr"], 0.1) * (1-0.4*inputs["msk"]) * (1-0.1*inputs["glvs"]));
    if (score_outside >= 900) {
     sg_outside = "Well done!";
    }
    else if (!inputs["msk"]){
      sg_outside = "Consider using masks while meeting visitors";
    } else if (!inputs["glvs"]){
      sg_outside = "Consider wearing gloves while meeting visitors";
    } else if (score_outside<700) {
      sg_outside = "Consider reducing the number of employees that meet outsiders";
    }
  }
  else{
    sg_outside = "No outside interactions";
  }
  score_outside = clipAndRound_bounds(score_outside);

  // Epidemic related precautions
  var meets_shift_requirement = 1;
  var total_emp = inputs["n29"]+inputs["n49"]+inputs["n64"]+inputs["n65plus"];
  // Uncomment this in the production code.
  if (0<inputs["NOE"] && inputs["NOE"]<=4){
    meets_shift_requirement = (total_emp*0.33>=nEmp) ? 1:0; 
  }

  var score_epidemic = 0;
  if (meets_shift_requirement){
  score_epidemic = 100*(inputs["tempScreening"] + inputs["faceCover"] + inputs["adqFaceCover"] + inputs["newShfts"] +
                        (inputs["tchFree"]? 1:0.5)*((inputs["nHsS"] > (inputs["tArea"]/1000)) ? 1 : 0) + 
                        Math.min(inputs["nDinf"], 2)/2 + Math.min(1, inputs["smkZS"]*((inputs["nPGT"]>0) ? 0 : 1))/2 + meets_shift_requirement + 
                        ((inputs["nWsB"] > (inputs["nFloors"]*2) ? 1 : 0))/2 + inputs["nASapp"]/total_emp + inputs["advAvdLPM"]);                              
  }
  score_epidemic = clipAndRound_bounds(score_epidemic);
                                
  var sg_epidemic = "";
  count = 0;
  if (!meets_shift_requirement){
    count += number_of_suggestions;
    sg_epidemic = "For your establishment, there is a maximum 33% attendance requirement which is not met. Hence the score is 0. You must comply with this requirement to get a positive score.";
  }
  if (!inputs["tempScreening"] && count<number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider temperature screening of all employees on entry and exit.";
  }
  if (!inputs["faceCover"] && count<number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider mandating face cover inside the office premises.";
  }
  if (!inputs["adqFaceCover"] && count<number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider procuring adequate number of facecovers for employees.";
  }
  if (!inputs["newShfts"] && count<number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider dividing the work timings into more shifts.";
  }
  if (!((inputs["nHsS"] > (inputs["tArea"]/1000)) ? 1 : 0)  && count<number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider increasing hand sanitiser dispensing stations.";
  }
  if (inputs["nDinf"]<2 && count<number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider disinfecting the office at least 2 times a day.";
  }
  if (!inputs["smkZS"] && count<number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider sealing smoking zones.";
  }
  if (!((inputs["nPGT"]>0) ? 0 : 1) && count <number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider banning pan-masala, gutkha and tobacco on the premises.";
  }
  if (!(inputs["nWsB"] > (inputs["nFloors"]*2) ? 1 : 0) && count <number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider having more warning sign boards for \'no spitting\'.";
  }
  if (!inputs["advAvdLPM"] && count <number_of_suggestions){
    count += 1;
    sg_epidemic = (sg_epidemic=="") ? "" : sg_epidemic + "<br>";
    sg_epidemic += "Consider avoiding large physical meetings.";
  }
  if (sg_epidemic==""){
    sg_epidemic = "Well done!";
  }

  // Advertisement and outreach
  var score_adv_outrch = 100*(inputs["covidPage"] + inputs["faq"] + inputs["sPers"] + inputs["hkTrn"] + inputs["advSclDis"] + inputs["advWFHVul"] + inputs["snzCvr"] + inputs["sPrgm"] + inputs["pstrs"] + (inputs["nWsB"] > (inputs["nFloors"]*2) ? 1 : 0));
  score_adv_outrch = clipAndRound_bounds(score_adv_outrch);

  var sg_adv_outrch = "";
  count = 0;
  if (!inputs["covidPage"]){
    count += 1;
    sg_adv_outrch += "Prepare a COVID-19 awareness page.";
  }
  if (!inputs["faq"] && count<number_of_suggestions){    
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Provide access to COVID-19 FAQ to the employees.";
  }
  if (!inputs["sPers"] && count<number_of_suggestions){
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Designate a cleanliness, awareness and safety champion.";
  }
  if (!inputs["hkTrn"] && count<number_of_suggestions){
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Conduct intensive training of house-keeping staff for COVID-19 related precautions.";
  }
  if (!inputs["advSclDis"] && count<number_of_suggestions){
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Advise employees to maintain social distance.";
  }
  if (!inputs["advWFHVul"] && count<number_of_suggestions){
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Advise vulnerable employees to work from home.";
  }
  if (!inputs["snzCvr"] && count<number_of_suggestions){
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Advise employees to cover their sneezes and coughs.";
  }
  if (!inputs["sPrgm"] && count<number_of_suggestions){
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Organise special programmes to sensitise employees on COVID-19 epidemic.";
  }
  if (!inputs["pstrs"] && count<number_of_suggestions){
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Put up posters on hygiene in common places.";
  }
  if (!(inputs["nWsB"] > (inputs["nFloors"]*2) ? 1 : 0) && count<number_of_suggestions){
    count += 1;
    sg_adv_outrch = (sg_adv_outrch=="") ? "" : sg_adv_outrch + "<br>";
    sg_adv_outrch += "Have more warning sign boards for \'no spitting\'.";
  }
  if (sg_adv_outrch==""){
    sg_adv_outrch = "Well done!";
  }

  // Company Transport
  var cmpnTrnsprtUsrs = inputs["trvlr5K"]+inputs["trvlr10K"]+inputs["trvlr10Kplus"];
  var F = Math.max( (inputs["bsCpctAct"]) ? inputs["bsCpctCur"]/inputs["bsCpctAct"] : 0, (inputs["mnBsCpctAct"]) ? inputs["mnBsCpctCur"]/inputs["mnBsCpctAct"] : 0, 
                    (inputs["vnCpctAct"]) ? inputs["vnCpctCur"]/inputs["vnCpctAct"] : 0, (inputs["svCpctAct"]) ? inputs["svCpctCur"]/inputs["svCpctAct"] : 0,
                    (inputs["crCpctAct"]) ? inputs["crCpctCur"]/inputs["crCpctAct"] : 0);
  var score_company_transport = F * (1-0.1*(inputs["nTrnsptSnt"]+inputs["drvSrnd"]+inputs["hsVhcl"]+inputs["vhclSnt"]+inputs["noACVhcl"])) * (1-0.4*inputs["mskMndt"])
                                * (inputs["trvlr5K"]*5 + inputs["trvlr10K"]*(10/1.25) + inputs["trvlr10Kplus"]*(15/1.5));

  // Self-owned Vehicles
  var slfTrnsprtUsrs = inputs["trvlr5Kslf"]+inputs["trvlr10Kslf"]+inputs["trvlr10Kplusslf"];
  var F_slf = 1
  if (inputs["noPlnR"] && inputs["no2plusTrvl"]){
    F_slf = 1/2;
  }
  
  var score_self_transport = F_slf * (1-0.1*inputs["hsLbb"]) * (1-0.4*inputs["mskCar"])
                             * (inputs["trvlr5Kslf"]*5 + inputs["trvlr10Kslf"]*(10/1.25) + inputs["trvlr10Kplusslf"]*(15/1.5));

  // Walking
  var score_walk = inputs["nWlk"] * ((1-inputs["hsLbb"])*0.2 + inputs["hsLbb"]*0.1)*(1-0.4*inputs["mskWlk"]);

  // Public Transport
  var pubTrnsprtUsrs = inputs["trvlr5Kpub"]+inputs["trvlr10Kpub"]+inputs["trvlr10Kpluspub"];
  
  var score_public_transport = (1-0.1*inputs["hsLbb"]) * (1-0.4*inputs["mskPub"]) *
                                 (inputs["trvlr5Kpub"]*5 + inputs["trvlr10Kpub"]*(10/1.25) + inputs["trvlr10Kpluspub"]*(15/1.5));
  
  var score_total_transport = 1;
  var score_total_transport_scaled = 1000;
  var nominal_scaled_score_total_transport = 800;
  var nominal_raw_score_total_transport = 3.5;

  if (cmpnTrnsprtUsrs + slfTrnsprtUsrs + inputs["nWlk"] + pubTrnsprtUsrs > 0){
    score_total_transport = (score_company_transport + score_self_transport + score_walk + score_public_transport)/
                            (cmpnTrnsprtUsrs + slfTrnsprtUsrs + inputs["nWlk"] + pubTrnsprtUsrs);

    score_total_transport_scaled = nominal_scaled_score_total_transport*nominal_raw_score_total_transport/score_total_transport;
  }
  
  score_total_transport_scaled = clipAndRound_bounds(score_total_transport_scaled);

  var sg_transport = "Well done!";
  if (cmpnTrnsprtUsrs + slfTrnsprtUsrs + inputs["nWlk"] + pubTrnsprtUsrs == 0){
    sg_transport = "High score. But do you really have no one traveling to your workplace?";
  } else if ((inputs["mskMndt"]+inputs["mskCar"]+inputs["mskWlk"]+inputs["mskPub"])<3){
    sg_transport = "Consider using mask while travelling";
  } else if (F>0.5){
    sg_transport = "Consider reducing numbers per journey";
  } else if ((inputs["nTrnsptSnt"]+inputs["drvSrnd"]+inputs["hsVhcl"]+inputs["vhclSnt"]+inputs["noACVhcl"])<3 && inputs["cmpnTrnsprtUsrs"]>0){
      sg_transport = "Consider more use of hand sanitiser etc.";
  }

  onSuccess("Successfully generated workplace readiness score!");
  var sg_total = "For general suggestions, see below"
  var score_total = score_office_infra + score_epidemic +  score_isolation + 
      score_adv_outrch + score_mobility + score_meetings +  score_outside + score_cafeteria_scaled + score_sanitation + score_total_transport_scaled
  
  var greeting = "<b>Company name:  " + inputs['cmpName'] + "</b><br><br>"; 
  var overall_report = "<div class='overall_report p-3'><b>Your overall COVID-19 readiness score is ";
  overall_report += score_total  
  overall_report += "<br>Your percentile score among your type of establishment is "
  overall_report +=  score_total/10;
  overall_report += "</div><br><br>"


	var resTable = "";
	resTable += "<table class='table table-bordered'><thead class='bg-dark'>";
	resTable += "<th>Readiness category</th><th>Score</th>";
  resTable += "<th>Specific suggestions for each readiness category</th></thead>";
  resTable += "<tr><td>Infrastructure</td><td>" + score_office_infra + "</td><td>" + sg_office_infra + "</td></tr>"
  resTable += "<tr><td>Epidemic related: Precautions</td><td>" + score_epidemic + "</td><td>" + sg_epidemic + "</td></tr>"
  resTable += "<tr><td>Epidemic related: Awareness and readiness</td><td>" + score_isolation + "</td><td>" + sg_isolation + "</td></tr>"
  resTable += "<tr><td>Epidemic related: Advertisement and outreach</td><td>" + score_adv_outrch + "</td><td>" + sg_adv_outrch + "</td></tr>"
  resTable += "<tr><td>Employee interactions: Mobility</td><td>" + score_mobility + "</td><td>"+ sg_mobility +"</td></tr>"
  resTable += "<tr><td>Employee interactions: Meetings</td><td>" + score_meetings + "</td><td>" + sg_meetings + "</td></tr>"
  resTable += "<tr><td>Employee interactions: Outside contacts</td><td>" + score_outside + "</td><td>" + sg_outside + "</td></tr>"
  resTable += "<tr><td>Cafeteria/pantry</td><td>" + score_cafeteria_scaled + "</td><td>" + sg_cafeteria + "</td></tr>"
	resTable += "<tr><td>Hygiene and sanitation</td><td>" + score_sanitation + "</td><td>" + sg_sanitation + "</td></tr>"
  resTable += "<tr><td>Transportation</td><td>" + score_total_transport_scaled + "</td><td>" + sg_transport + "</td></tr>"
  resTable += "<tr><td>Total</td><td>" + score_total + "</td><td>" + sg_total + "</td></tr>"
  resTable += "</table>";
  document.getElementById("scoreTable").innerHTML = greeting + overall_report + resTable;
  
  var outputs = new Object();
  outputs["Infrastructure"] = score_office_infra;
  outputs["Epidemic related: Precautions"] = score_epidemic;
  outputs["Epidemic related: Awareness and readiness"] = score_isolation;
  outputs["Epidemic related: Advertisement and outreach"] = score_adv_outrch;
  outputs["Employee interactions: Mobility"] = score_mobility;
  outputs["Employee interactions: Meetings"] = score_meetings;
  outputs["Employee interactions: Outside contacts"] = score_outside;
  outputs["Cafeteria/pantry"] = score_cafeteria_scaled;
  outputs["Hygiene and sanitation"] = score_sanitation;
  outputs["Transportation"] = score_total_transport_scaled;
  outputs["Total"] = score_total;
  
  log_json = JSON.stringify({'inputs': inputs, 'outputs': outputs});
  post_function(log_json);
}

function post_function(log_json)
{   
    console.log(log_json);
    /*
    $.ajax({
      type: "POST",
      url: "",
      data: "data="+log_json
  });
  */
}

function openPage(pageName, elmnt, color) {
  // Hide all elements with class="tabcontent" by default */
  var i, tabcontent, tablinks;
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }

  // Remove the background color of all tablinks/buttons
  tablinks = document.getElementsByClassName("tablink");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].style.backgroundColor = "";
  }

  // Show the specific tab content
  document.getElementById(pageName).style.display = "block";

  // Add the specific color to the button used to open the tab content
  elmnt.style.backgroundColor = color;
}

// Get the element with id="defaultOpen" and click on it
//document.getElementById("defaultOpen").click();
document.getElementById("QnTab").click();

function handleFormSubmit(formObject) {
  /* google.script.run.withSuccessHandler(onSuccess).processForm(formObject); */
  /* document.getElementById("myForm").reset(); */
        calcScore();
	openPage('Scores', document.getElementById("ScoresTab"), '#26734d')
}

function reEnter() {
	openPage('Qn', document.getElementById("QnTab"), '#2c4268')
}

function printPage(){
  $("#header").hide()
  $(".tablink").hide()
  $(".sub-btn").hide()
  $(".prnt-btn").hide()
  $("#footer").hide()
  $(".tabcontent").css('color','black');
  $("#Scores").css('background-color','white');
  /* $(".bg-dark").css('background-color','white'); */
  window.print()
  $("#header").show()
  $(".tablink").show()
  $(".sub-btn").show()
  $(".prnt-btn").show()
  $("#footer").show()
  $(".tabcontent").css('color','white');
  $("#Scores").css('background-color','#26734d');
  /* $(".bg-dark").css('background-color','#212529'); */
}

