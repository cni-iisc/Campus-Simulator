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
  dict["NOE"] = parseInt(document.getElementById("NOE").value); // Nature of Establishment
  
  // Employee Information
  dict["nM"] = parseInt(document.getElementById("nM").value); // Number of male employees
  dict["nF"] = parseInt(document.getElementById("nF").value); // Number of female employees
  dict["nOth"] = parseInt(document.getElementById("nOth").value); // Number of other employees
  dict["rAddrKn"] = parseInt(document.getElementById("rAddrKn").value); // Number of employees with address records
  dict["pCS"] = parseInt(document.getElementById("pCS").value); // Percentage of casual labour and security
  dict["nShifts"] = parseInt(document.getElementById("nShifts").value); // Number of shifts
  dict["tGapShift"] = parseFloat(document.getElementById("tGapShift").value); // Time gap between shifts in hours
  dict["informCZEmp"] = parseInt(document.querySelector('input[name="informCZEmp"]:checked').value); // Inform containment zone employee not to come
  dict["informWFH"] = parseInt(document.querySelector('input[name="informWFH"]:checked').value); // Encourage work from home 
  dict["n19"] = parseInt(document.getElementById("n19").value); // Number of employees with age betwwen 15 and 19
  dict["n29"] = parseInt(document.getElementById("n29").value); // Number of employees with age betwwen 20 and 29
  dict["n39"] = parseInt(document.getElementById("n39").value); // Number of employees with age betwwen 30 and 39
  dict["n49"] = parseInt(document.getElementById("n49").value); // Number of employees with age betwwen 40 and 49
  dict["n59"] = parseInt(document.getElementById("n59").value); // Number of employees with age betwwen 50 and 59
  dict["n60plus"] = parseInt(document.getElementById("n60plus").value); // Number of employees with age more than 60
  dict["nASapp"] = parseInt(document.getElementById("nASapp").value); // Number of employees with Aarogya Setu app
  dict["nGCGapp"] = parseInt(document.getElementById("nGCGapp").value); // Fraction of employees with Go Corona Go app

  // Office Infrastructure Information
  dict["tArea"] = parseInt(document.getElementById("tArea").value); // Total office area in sq.ft
  dict["nFloors"] = parseInt(document.getElementById("nFloors").value); // Number of floors in office
  dict["opnCubArea"] = parseInt(document.getElementById("opnCubArea").value); // Total open cubicle area in sq.ft
  dict["prtnArea"] = parseInt(document.getElementById("prtnArea").value); // Remaining working area in sq. ft
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
  dict["tempAC"] = parseInt(document.getElementById("tempAC").value); // Temperature setting
  dict["humAC"] = parseInt(document.getElementById("humAC").value); // Relative humidity setting

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
  dict["nHsS"] = parseInt(document.getElementById("nHsS").value); // Number of hand-sanitiser stations
  dict["nDinf"] = parseInt(document.getElementById("nDinf").value); // Number of disinfection activity per day
  dict["smkZS"] = parseInt(document.querySelector('input[name="smkZS"]:checked').value); // Smoking zone sealed
  dict["nPGT"] = parseInt(document.getElementById("nPGT").value); // Number of employees consuming Pan masala, gutkha, tobacco 
  dict["nWsB"] = parseInt(document.getElementById("nWsB").value); // Number of warning sign boards

  // Employee Interactions
  dict["nVstrs"] = parseInt(document.getElementById("nVstrs").value); // Number of visitors
  dict["nEmpCstmr"] = parseInt(document.getElementById("nEmpCstmr").value); // Number of employees that meet with customer
  // dict["avgExt"] = parseInt(document.getElementById("avgExt").value); // Average number of external contacts with employees
  dict["nDlvrHndlng"] = parseInt(document.getElementById("nDlvrHndlng").value); // Number of employees handling deliveries 
  dict["msk"] = parseInt(document.querySelector('input[name="msk"]:checked').value); // Employees wear mask
  dict["glvs"] = parseInt(document.querySelector('input[name="glvs"]:checked').value); // Employess wear mask and gloves

  // Mobility Related
  dict["nHM"] = parseInt(document.getElementById("nHM").value); // Number of employees that are more than 2 hours aways
  dict["nMM"] = parseInt(document.getElementById("nMM").value); // Number of employees that are more than 1 hour aways 
  dict["nMPD"] = parseFloat(document.getElementById("nMPD").value); // Meetings per day
  dict["avgMS"] = parseFloat(document.getElementById("avgMS").value); // Average number of members in the meeting
  
  // Cafeteria/Pantry
  dict["cntn"] = parseInt(document.querySelector('input[name="cntn"]:checked').value); // Canteen/pantry
  //dict["cntnAC"] = parseInt(document.querySelector('input[name="cntnAC"]:checked').value); // Canteen/pantry air condition
  dict["cntnACOp"] = parseInt(document.querySelector('input[name="cntnACOp"]:checked').value); // Canteen/pantry air condition operational
  dict["nBrkfst"] = parseInt(document.getElementById("nBrkfst").value); // Number of employees having breakfast in canteen
  dict["nLnch"] = parseInt(document.getElementById("nLnch").value); // Number of employees having lunch in canteen
  dict["nSnck"] = parseInt(document.getElementById("nSnck").value); // Number of employees having snacks/coffee in canteen
  dict["nEmpHL"] = parseInt(document.getElementById("nEmpHL").value); // Number of employees who bring lunch from home
  dict["brkfst"] = parseInt(document.querySelector('input[name="brkfst"]:checked').value); // Breakfast served
  dict["lnch"] = parseInt(document.querySelector('input[name="lnch"]:checked').value); // Lunch served
  dict["cff"] = parseInt(document.querySelector('input[name="cff"]:checked').value); // Coffee/snacks served
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

  // Company Provided Transport
  dict["cmpnTrnsprtUsrs"] = parseInt(document.getElementById("cmpnTrnsprtUsrs").value); // Company transport users
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
  dict["crwdnss"] = 0;// document.getElementById("crwdnss").value); // Crowdness extent
  dict["mskMndt"] = parseInt(document.querySelector('input[name="mskMndt"]:checked').value); // Mask mandate
  dict["hsVhcl"] = parseInt(document.querySelector('input[name="hsVhcl"]:checked').value); // Hand sanitiser in vehicle
  dict["noACVhcl"] = parseInt(document.querySelector('input[name="noACVhcl"]:checked').value); // No AC use in vehicle
  dict["nTrnsptSnt"] = parseInt(document.getElementById("nTrnsptSnt").value); // Number of times transport bay is sanitised
  dict["drvSrnd"] = parseInt(document.querySelector('input[name="drvSrnd"]:checked').value); // Drivers screened
  dict["vhclSnt"] = parseInt(document.querySelector('input[name="vhclSnt"]:checked').value); // Vehicles sanitised
  dict["trvlr5K"] = parseInt(document.getElementById("trvlr5K").value); // Numbers travelling 0-5 km
  dict["trvlr10K"] = parseInt(document.getElementById("trvlr10K").value); // Numbers travelling 5-10 km
  dict["trvlr15K"] = parseInt(document.getElementById("trvlr15K").value); // Numbers travelling 10-15 km
  dict["trvlr15Kplus"] = parseInt(document.getElementById("trvlr15Kplus").value); // Numbers travelling >15 km

  // Self-owned vehicle transport
  dict["slfTrnsprtUsrs"] = parseInt(document.getElementById("slfTrnsprtUsrs").value); // Self-owned transport users
  dict["noPlnR"] = parseInt(document.querySelector('input[name="noPlnR"]:checked').value); // No pillion riders
  dict["no2plusTrvl"] = parseInt(document.querySelector('input[name="no2plusTrvl"]:checked').value); // No 2 plus travel in car
  dict["hsLbb"] = parseInt(document.querySelector('input[name="hsLbb"]:checked').value); // Hand sanitiser in the lobby
  dict["mskCar"] = parseInt(document.querySelector('input[name="mskCar"]:checked').value); // Mask in car
  dict["trvlr5Kslf"] = parseInt(document.getElementById("trvlr5Kslf").value); // Numbers travelling 0-5 km private vehicle
  dict["trvlr10Kslf"] = parseInt(document.getElementById("trvlr10Kslf").value); // Numbers travelling 5-10 km private vehicle
  dict["trvlr15Kslf"] = parseInt(document.getElementById("trvlr15Kslf").value); // Numbers travelling 10-15 km private vehicle
  dict["trvlr15Kplusslf"] = parseInt(document.getElementById("trvlr15Kplusslf").value); // Numbers travelling >15 km private vehicle

  // Walking 
  dict["nWlk"] = parseInt(document.getElementById("nWlk").value); // Number of employees who walk to work
  dict["mskWlk"] = parseInt(document.querySelector('input[name="mskWlk"]:checked').value); // Mask while walking

  // Public Transport
  dict["nPubTrvl"] = parseInt(document.getElementById("nPubTrvl").value); // Number of employees travelling in public transport
  dict["mskPub"] = parseInt(document.querySelector('input[name="mskPub"]:checked').value); // Mask while in public transport
  dict["trvlr5Kpub"] = parseInt(document.getElementById("trvlr5Kpub").value); // Numbers travelling 0-5 km private vehicle
  dict["trvlr10Kpub"] = parseInt(document.getElementById("trvlr10Kpub").value); // Numbers travelling 5-10 km private vehicle
  dict["trvlr15Kpub"] = parseInt(document.getElementById("trvlr15Kpub").value); // Numbers travelling 10-15 km private vehicle
  dict["trvlr15Kpluspub"] = parseInt(document.getElementById("trvlr15Kpluspub").value); // Numbers travelling >15 km private vehicle

  // Time to reach office
  dict["n60Min"] = parseInt(document.getElementById("n60Min").value); // Number of employees taking 30-60 minutes
  dict["n60plusMin"] = parseInt(document.getElementById("n60plusMin").value); // Number of employees taking >60 minutes

  // Isolation room
  dict["hl"] = parseInt(document.querySelector('input[name="hl"]:checked').value); // Company helpline
  dict["iQS"] = parseInt(document.querySelector('input[name="iQS"]:checked').value); // Immediate quarantine space
  dict["amblnc"] = parseInt(document.querySelector('input[name="amblnc"]:checked').value); // Ambulance facility
  dict["lHsptl"] = parseInt(document.querySelector('input[name="lHsptl"]:checked').value); // List of nearby hospitals etc.
  dict["emrgncResp"] = parseInt(document.querySelector('input[name="emrgncResp"]:checked').value); // Emergency Response
  dict["alrg"] = parseInt(document.querySelector('input[name="alrg"]:checked').value); // Employee allergy list
  dict["imdtFM"] = parseInt(document.querySelector('input[name="imdtFM"]:checked').value); // Immediate family members list
  dict["lstUpdtTime"] = parseInt(document.getElementById("lstUpdtTime").value); // Last information update time

  // Advertisement and Outreach
  dict["covidPage"] = parseInt(document.querySelector('input[name="covidPage"]:checked').value); // Covid Awareness Page
  dict["faq"] = parseInt(document.querySelector('input[name="faq"]:checked').value); // FAQ Page
  dict["sPers"] = parseInt(document.querySelector('input[name="sPers"]:checked').value); // Cleaniness and safety person
  dict["advSclDis"] = parseInt(document.querySelector('input[name="advSclDis"]:checked').value); // Advised social distancing

  return dict;
}

function clipAndRound_bounds (score) {
  /*
  if (isNan (score) ) {
    console.log ("Score is NaN, pls check");
    return -1;
  }
  else if (score == "N.A.") {
    return score;
  }
  */

  var score_out = Math.round(score/10);
  if (score_out < 0 ) {
    score_out = 0;
  } else if (score_out > 100 ) {
    score_out = 100;
  }
  return score_out;
}

function calcScore () {
  inputs = getValues(); //Read values from html page...
  console.log(inputs);

  // Office Infrastructure
  var nEmp = inputs["nM"] + inputs["nF"] + inputs["nOth"];
  var nMeets = 4;
  var aOpen = inputs["tArea"] - inputs["nSOfcRm"]*150 - inputs["n2pOfcRm"]*200 - inputs["n2pPlusOfcRm"]*240;
  var cFactor = Math.max( ((inputs["nCub"] + inputs["nRem"])*40 / aOpen), 1 ); 
  var crowding = 2*inputs["n2pOfcRm"] * nMeets + 3*inputs["n2pPlusOfcRm"] * nMeets * 1.2 +
      (inputs["nCub"] + inputs["nRem"] * 1.2)* nMeets * cFactor;
  var stairsElev = 0.5 * inputs["nFloors"] * (1 + inputs["eleCpct"]/2 * (1-0.1*inputs["advSclDis"]))/2 * 4 * Math.max( (1 - 0.1*Math.min( inputs["nStrCln"], inputs["nEleDinf"] )), 0.5);
  var bmFlag = ((inputs["baDoor"] >= 1) ? 1 : 0);
  var accessContacts = 4 * ( 1 - 0.1*((bmFlag * inputs["sntBio"]) + inputs["mntrCCTV"]) ) * bmFlag;
  var shiftDelta = ( (inputs["tGapShift"] > 8) ? 0 : (8 - inputs["tGapShift"]));
  var premisesContacts = (accessContacts + stairsElev + crowding/ nEmp ) * (1 + (shiftDelta/8.0)) * (1-0.4*inputs["msk"]);

  // Other meeting spaces
  var score_other_spaces = 0.5 * inputs["oMtngSpts"] * Math.max(1-0.1*(inputs["freqCln"]*Math.min(1, inputs["nHskpngStff"])), 0.5) * (1-0.4*inputs["msk"]);
  var score_office_infra = 800*31/(premisesContacts + score_other_spaces);
  score_office_infra = clipAndRound_bounds(score_office_infra);

  var sg_office_infra = "Well done!"
  if (score_office_infra<70){
    sg_office_infra = "Consider encouraging more employees to work from home or shifts";
  } else if (inputs["nEleDinf"]<2) {
    sg_office_infra = "Consider cleaning the lifts more often";
  }

  // Toilet scores
  var nGntsTlt = inputs["nM"] + inputs["nOth"]/2.0; 
  var nLdsTlt = inputs["nF"] + inputs["nOth"]/2.0; 
  var avgTltVstsPrDy = 5; 
  var avgTltDrtn = 4;
  var tltCnctrtnHrs = 4; 

  var cRateGentsToilet = nGntsTlt * avgTltVstsPrDy * avgTltDrtn * (Math.max(0.5, (1.0 - 0.1*inputs["tClnFreq"]))) * (1.0 - 0.1*inputs["spPrsnt"]) / (tltCnctrtnHrs*60*inputs["nGntsT"]);
  var cRateLadiesToilet = nLdsTlt * avgTltVstsPrDy * avgTltDrtn * (Math.max(0.5, (1.0 - 0.1*inputs["tClnFreq"]))) * (1.0 - 0.1*inputs["spPrsnt"]) / (tltCnctrtnHrs*60*inputs["nLdsT"]);
  //var aggrToiletSc = 800*nEmp/(cRateLadiesToilet + cRateGentsToilet);
  var aggrToiletSc = 800/(Math.max(cRateLadiesToilet, cRateGentsToilet)+0.001);
  var score_sanitation = clipAndRound_bounds(aggrToiletSc);

  var sg_sanitation = "Well done!";

  //score_sanitation = Math.round(Math.min( 100, 70.0/(cRateGentsToilet + cRateLadiesToilet) )) * 10;
  if (score_sanitation < 70) {
      sg_sanitation = "Disinfect toilets more often or consider reducing the employees per shift";
  }

  // Sick Rooms/Isolation Ward
  var score_sickRoom = 100*(inputs["iQS"]*2 + inputs["amblnc"] + inputs["lHsptl"]*2 + inputs["emrgncResp"] + inputs["hl"] + 
                            inputs["imdtFM"] + inputs["alrg"] * Math.max( 0, (1.0 - inputs["lstUpdtTime"]/60))*2);
  var score_isolation = clipAndRound_bounds(score_sickRoom);
  var sg_isolation = "Well done!";
  if (!inputs["iQS"]){
    sg_isolation = "Designate place for immediate quarantine";
  } else if (!inputs["lHsptl"]){
    sg_isolation = "Prepare a list of COVID-19 ready hospitals";
  } else if (!inputs["amblnc"]){
    sg_isolation = "Keep an ambulance on stand-by";
  } else if (!inputs["emrgncResp"]) {
    sg_isolation = "Give instructions to transport department on COVID-19 emergency";
  } else if (!inputs["hl"]) {
    sg_isolation = "Consider having a dedicated COVID-19 helpline";
  } else if (Math.max( 0, (1.0 - inputs["lstUpdtTime"]/60)) < 0.5 ) {
    sg_isolation = "Update medical history of employees";
  }

  // Interactions spaces
  var time_brkfst = 15; // In minutes
  var time_lnch = 30;
  var time_snck = 15;
  var time_wtr = 2;
  var num_wtr_sought = 5;
  var prsnl_area = 28; // In sq. feet: Approx. 3 feet radius
  var pvtl_hrs = 4;
  var nOutside = nEmp - inputs["nLnch"] - inputs["nEmpHL"];

  var mtng_brkfst = (inputs["nBrkfst"] * time_brkfst * prsnl_area) / (inputs["cntnArea"] * 60);
  var mtng_lnch = (inputs["nLnch"] * time_lnch * prsnl_area) / (inputs["cntnArea"] * 90);
  var mntg_snck = (inputs["nSnck"] * time_snck * prsnl_area) / (inputs["cntnArea"] * 60);
  var mntg_wtr = (nEmp * time_wtr * num_wtr_sought * prsnl_area) / (inputs["nWS"] * inputs["cntnArea"] * pvtl_hrs *  60);
  
  var score_cafeteria = (mtng_brkfst*(inputs["nBrkfst"]/nEmp) + mtng_lnch*(inputs["nLnch"]/nEmp) + mntg_snck*(inputs["nSnck"]/nEmp) + mntg_wtr + (inputs["nEmpHL"]*0.25/nEmp) + (nOutside*2/nEmp));
  score_cafeteria = score_cafeteria * Math.max((1 - 0.1*( inputs["mlStggrd"] + inputs["freqCln"] - inputs["utnslShrd"])), 1/2);

  // We prefer the score_cafeteria to be less than 2
  var score_cafeteria_scaled = 700*2/score_cafeteria;
  var score_cafeteria_scaled = clipAndRound_bounds(score_cafeteria_scaled);
  var sg_cafeteria = "Well done!";
  if (score_cafeteria_scaled<70){
    sg_cafeteria = "Increase the cafeteria area to accomodate more people or encourage work from home";
  } else if (nOutside/nEmp > 0.5 && score_cafeteria_scaled<60){
    sg_cafeteria = "Encourage bringing lunch from home or provide lunch on premise"
  }

  // Mobility
  var nLM = nEmp - inputs["nHM"] - inputs["nMM"]
  var score_mobility = ((0.25*nLM + 0.5*inputs["nMM"] + inputs["nHM"])/(nLM+inputs["nMM"]+inputs["nHM"])) * (1-0.4*inputs["msk"]);
  score_mobility = (1-score_mobility)*1000/0.85;
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
  // TODO: check if NA is to be introduced
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
    }
  }
  else {
    score_outside = "N.A."
  }
  score_outside = clipAndRound_bounds(score_outside);


  // Epidemic related precautions
  var score_epidemic = 100*(inputs["tempScreening"]*2 + inputs["faceCover"]*2 +
                                ((inputs["nHsS"] > (inputs["tArea"]/1000)) ? 1 : 0)*2 +
                                Math.min(inputs["nDinf"], 2) + Math.min(1, inputs["smkZS"]*((inputs["nPGT"]>0) ? 0 : 1)) + 
                                ((inputs["nWsB"] > (inputs["nFloors"]*2) ? 1 : 0)) );                              
  
  score_epidemic = clipAndRound_bounds(score_epidemic);
                                
  var sg_epidemic = "Well done!";
  if (!inputs["tempScreening"]){
    sg_epidemic = "Consider temperature screening of all employees on entry and exit";
  } else if (!inputs["faceCover"]){
    sg_epidemic = "Consider mandating face cover inside the office premise";
  } else if (!(inputs["nHsS"] > (inputs["tArea"]/1000)) ? 1 : 0){
    sg_epidemic = "Consider increasing hand sanitiser stations";
  } else if (inputs["nDinf"]<2){
    sg_epidemic = "Consider disinfecting the office at least 2 times a day";
  } else if (!inputs["smkZS"]){
    sg_epidemic = "Consider sealing smoking zones";
  } else if (!(inputs["nPGT"]>0) ? 0 : 1){
    sg_epidemic = "Consider banning pan-masala, gutkha and tobaccon on premise";
  } else if (!(inputs["nWsB"] > (inputs["nFloors"]*2) ? 1 : 0)){
    sg_epidemic = "Consider having more warning sign boards for \'no spitting\'";
  }

  // Advertisement and outreach
  var score_adv_outrch = (inputs["covidPage"] + inputs["faq"] + inputs["sPers"] + (inputs["nWsB"] > (inputs["nFloors"]*2) ? 1 : 0))*1000/4 ;
  score_adv_outrch = clipAndRound_bounds(score_adv_outrch);

  var sg_adv_outrch = "Well done!";
  if (!inputs["covidPage"]){
    sg_adv_outrch = "Prepare a COVID-19 awareness page";
  } else if (!inputs["faq"]){
    sg_adv_outrch = "Provide access to COVID-19 FAQ to the employees";
  } else if (!inputs["sPers"]){
    sg_adv_outrch = "Designate a cleanliness, awareness and safety person";
  } else if (!(inputs["nWsB"] > (inputs["nFloors"]*2) ? 1 : 0)){
    sg_adv_outrch = "Consider having more warning sign boards for \'no spitting\'";
  }

  // Company Transport
  var F = Math.max(inputs["bsCpctCur"]/inputs["bsCpctAct"], inputs["mnBsCpctCur"]/inputs["mnBsCpctAct"], inputs["vnCpctCur"]/inputs["vnCpctAct"],
                   inputs["svCpctCur"]/inputs["svCpctAct"], inputs["crCpctCur"]/inputs["crCpctAct"]);
  var ttl_cmpn_trnsprt_mtchs = (inputs["cmpnTrnsprtUsrs"]<=inputs["trvlr5K"]+inputs["trvlr10K"]+inputs["trvlr15K"]) ? 1 : 0;
  var score_company_transport = 0;
  if (ttl_cmpn_trnsprt_mtchs){
    score_company_transport = F * (1-0.1*(inputs["nTrnsptSnt"]+inputs["drvSrnd"]+inputs["hsVhcl"]+inputs["vhclSnt"]+inputs["noACVhcl"])) * (1-0.4*inputs["mskMndt"])
                                * (inputs["trvlr5K"]*5 + inputs["trvlr10K"]*(10/1.25) + inputs["trvlr15K"]*(15/1.5) + inputs["trvlr15Kplus"]*(20/1.5));
  }

  // Self-owned Vehicles
  var ttl_slf_trnsprt_mtchs = (inputs["slfTrnsprtUsrs"]<=inputs["trvlr5Kslf"]+inputs["trvlr10Kslf"]+inputs["trvlr15Kslf"]) ? 1 : 0;
  var score_self_transport = 0;
  var F_slf = 1
  if (inputs["noPlnR"] && inputs["no2plusTrvl"]){
    F_slf = 1/2;
  }
  if (ttl_slf_trnsprt_mtchs){
    score_self_transport = F_slf * (1-0.1*inputs["hsLbb"]) * (1-0.4*inputs["mskCar"])
                             * (inputs["trvlr5Kslf"]*5 + inputs["trvlr10Kslf"]*(10/1.25) + inputs["trvlr15Kslf"]*(15/1.5) + inputs["trvlr15Kplusslf"]*(20/1.5));
  }

  // Walking
  var score_walk = inputs["nWlk"] * ((1-inputs["hsLbb"])*0.2 + inputs["hsLbb"]*0.1)*(1-0.4*inputs["mskWlk"]);

  // Public Transport
  var ttl_pblc_trnsprt_mtchs = (inputs["slfTrnsprtUsrs"]<=inputs["trvlr5Kslf"]+inputs["trvlr10Kslf"]+inputs["trvlr15Kslf"]) ? 1 : 0;
  var score_public_transport = 0;
  if (ttl_pblc_trnsprt_mtchs){
    var score_public_transport = (1-0.1*inputs["hsLbb"]) * (1-0.1*inputs["mskPub"]) *
                                 (inputs["trvlr5Kpub"]*5 + inputs["trvlr10Kpub"]*(10/1.25) + (inputs["trvlr15Kpub"]+ inputs["trvlr15Kpluspub"])*(15/1.5));
  }
  var score_total_transport = (score_company_transport + score_self_transport + score_walk + score_public_transport)/
                              (inputs["cmpnTrnsprtUsrs"] + inputs["slfTrnsprtUsrs"] + inputs["nWlk"] + inputs["nPubTrvl"]);
  // Average is more appropriate for large companies.
  var score_total_transport_scaled = 800*3.5/score_total_transport;
  score_total_transport_scaled = clipAndRound_bounds(score_total_transport_scaled);

  var sg_transport = "Well done!";
  if ((inputs["mskMndt"]+inputs["mskCar"]+inputs["mskWlk"]+inputs["mskPub"])<3){
    sg_transport = "Consider using mask while travelling";
  } else if (F>0.5){
    sg_transport = "Consider reducing numbers per journey";
  } else if ((inputs["nTrnsptSnt"]+inputs["drvSrnd"]+inputs["hsVhcl"]+inputs["vhclSnt"]+inputs["noACVhcl"])<3 && inputs["cmpnTrnsprtUsrs"]>0){
      sg_transport = "Consider more use of hand sanitiser etc.";
  }

  onSuccess("Check you workplace ratings!");

	var resTable = "";
	resTable += "<table class='table table-bordered'><thead class='bg-dark'>";
	resTable += "<th>Category</th><th>Score</th>";
	resTable += "<th>Suggestions for improvements</th></thead>";
  resTable += "<tr><td>Mobility in office</td><td>" + score_mobility + "</td><td>"+ sg_mobility +"</td></tr>"
  resTable += "<tr><td>Meeting space in office</td><td>" + score_meetings + "</td><td>" + sg_meetings + "</td></tr>"
  resTable += "<tr><td>Outside contacts in office</td><td>" + score_outside + "</td><td>" + sg_outside + "</td></tr>"
  resTable += "<tr><td>Cafeteria/pantry</td><td>" + score_cafeteria_scaled + "</td><td>" + sg_cafeteria + "</td></tr>"
  resTable += "<tr><td>Epidemic related precautions</td><td>" + score_epidemic + "</td><td>" + sg_epidemic + "</td></tr>"
  resTable += "<tr><td>Transportation</td><td>" + score_total_transport_scaled + "</td><td>" + sg_transport + "</td></tr>"
	resTable += "<tr><td>Office infrastructure</td><td>" + score_office_infra + "</td><td>" + sg_office_infra + "</td></tr>"
	resTable += "<tr><td>Hygiene and sanitation</td><td>" + score_sanitation + "</td><td>" + sg_sanitation + "</td></tr>"
	resTable += "<tr><td>Awareness and readiness</td><td>" + score_isolation + "</td><td>" + sg_isolation + "</td></tr>"
  resTable += "<tr><td>Advertisement and outreach</td><td>" + score_adv_outrch + "</td><td>" + sg_adv_outrch + "</td></tr>"
  resTable += "</table>";
	document.getElementById("scoreTable").innerHTML = resTable;
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

