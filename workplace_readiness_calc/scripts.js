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
  dict["NOE"] = document.getElementById("NOE").value; // Nature of Establishment
  
  // Employee Information
  dict["nM"] = parseInt(document.getElementById("nM").value); // Number of male employees
  dict["nF"] = parseInt(document.getElementById("nF").value); // Number of female employees
  dict["nOth"] = parseInt(document.getElementById("nOth").value); // Number of other employees
  dict["rAddrKn"] = parseInt(document.getElementById("rAddrKn").value); // Number of employees with address records
  dict["pCS"] = parseInt(document.getElementById("pCS").value); // Percentage of casual labour and security
  dict["nShifts"] = parseInt(document.getElementById("nShifts").value); // Number of shifts
  dict["tGapShift"] = parseFloat(document.getElementById("tGapShift").value); // Time gap between shifts in hours
  dict["informCZEmp"] = parseInt(document.querySelector('input[name="informCZEmp"]:checked').value); // Inform containment zone employee not to come
  dict["informWFH"] = parseInt(document.querySelector('input[name="informWFH"]:checked').value); // Encourage WFH  
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
  //dict["avgFA"] = parseInt(document.getElementById("avgFA").value); // Average area of floors in sq.ft
  //dict["avgFEmp"] = parseInt(document.getElementById("avgFEmp").value); // Average number of employees per floor
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
  dict["nEleDinf"] = parseInt(document.getElementById("nEleDinf").value); // Frequency of elevetor disinfection process
  dict["nStrCln"] = parseInt(document.getElementById("nStrCln").value); // Frequency of stairway cleaning
  dict["nStrHrDinf"] = parseInt(document.getElementById("nStrHrDinf").value); // Frequency of stairway handrails disinfection process
  
  // Epidemic related precautions
  dict["tempScreening"] = parseInt(document.querySelector('input[name="tempScreening"]:checked').value); // Temperature screening of employee
  dict["faceCover"] = parseInt(document.querySelector('input[name="faceCover"]:checked').value); // Face is covered with mask
  dict["nHsS"] = parseInt(document.getElementById("nHsS").value); // Number of hand-sanitiser stations
  dict["smkZS"] = parseInt(document.querySelector('input[name="smkZS"]:checked').value); // Smoking zone sealed
  dict["nPGT"] = parseInt(document.getElementById("nPGT").value); // Number of employees consuming Pan masala, gutkha, tobacco 
  dict["nWsB"] = parseInt(document.getElementById("nWsB").value); // Number of warning sign boards

  // Employee Interactions
  //DUPLICATE dict["safetyPerson"] = parseInt(document.querySelector('input[name="safetyPerson"]:checked').value); // Designated safety person
  dict["nVisitors"] = parseInt(document.getElementById("nVisitors").value); // Number of visitors
  dict["nEmpCstmr"] = parseInt(document.getElementById("nEmpCstmr").value); // Number of employees that meet with customer
  // dict["avgExt"] = parseInt(document.getElementById("avgExt").value); // Average number of external contacts with employees
  dict["nDlvrHndlng"] = parseInt(document.getElementById("nDlvrHndlng").value); // Number of employees handling deliveries 
  dict["gloves"] = parseInt(document.querySelector('input[name="gloves"]:checked').value); // Employess wear mask and gloves

  // Mobility Related
  dict["nHM"] = parseInt(document.getElementById("nHM").value); // Number of employees that are more than 2 hours aways
  dict["nMM"] = parseInt(document.getElementById("nMM").value); // Number of employees that are more than 1 hour aways 
  //DERIVED dict["nLM"] = parseInt(document.getElementById("nLM").value); // Remaining employees
  dict["nMPD"] = parseFloat(document.getElementById("nMPD").value); // Meetings per day
  dict["avgMS"] = parseFloat(document.getElementById("avgMS").value); // Average number of members in the meeting
  
  // Interaction Spaces
  dict["cntn"] = parseInt(document.querySelector('input[name="cntn"]:checked').value); // Canteen/pantry
  dict["cntnAC"] = parseInt(document.querySelector('input[name="cntnAC"]:checked').value); // Canteen/pantry air condition
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
  dict["oMtngSpts"] = parseInt(document.getElementById("oMtngSpts").value); // Other meeting spaces
  dict["freqCln"] = parseInt(document.getElementById("freqCln").value); // Frequency of cleaning
  dict["nHskpngStff"] = parseInt(document.getElementById("nHskpngStff").value); // Number of housekeeping staff

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
  dict["trvlr5Kpub"] = parseInt(document.getElementById("trvlr5Kpub").value); // Numbers travelling 0-5 km private vehicle
  dict["trvlr10Kpub"] = parseInt(document.getElementById("trvlr10Kpub").value); // Numbers travelling 5-10 km private vehicle
  dict["trvlr15Kpub"] = parseInt(document.getElementById("trvlr15Kpub").value); // Numbers travelling 10-15 km private vehicle
  dict["trvlr15Kpluspub"] = parseInt(document.getElementById("trvlr15Kpluspub").value); // Numbers travelling >15 km private vehicle

  // Time to reach office
  dict["n60Min"] = parseInt(document.getElementById("n60Min").value); // Number of employees taking 30-60 minutes
  dict["n60plusMin"] = parseInt(document.getElementById("n60plusMin").value); // Number of employees taking >60 minutes
  dict["mskAll"] = parseInt(document.querySelector('input[name="mskAll"]:checked').value); // Mask and gloves for all

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
  
  return dict;
}

function calcScore () {
  inputs = getValues(); //Read values from html page...
  console.log(inputs);

  //Preetam changes; insert near Office Infra inputs
  var nTravels = 4;
  var nMeets = 4;
  var aOpen = inputs["tArea"] - inputs["nSOfcRm"]*150 - inputs["n2pOfcRm"]*200 - inputs["n2pPlusOfcRm"]*240;
  var cFactor = Math.max( ((inputs["nCub"] + inputs["nRem"])*40 / aOpen), 1 ); 
  var crowding = 2*inputs["n2pOfcRm"] * nMeets + 3*inputs["n2pPlusOfcRm"] * nMeets * 1.2 +
      (inputs["nCub"] + inputs["nRem"] * 1.2)* nMeets * cFactor;
  var stairsElev = 0.5 * inputs["nFloors"] * 4 * Math.max( (1 - 0.1*Math.min( inputs["nStrCln"], inputs["nEleDinf"] )), 0.5) / (2* inputs["nEle"])
  var bmFlag = ((inputs["baDoor"] >= 1) ? 1 : 0);
  var accessContacts = 4 * ( 1 - 0.1*((bmFlag * inputs["sntBio"]) + inputs["mntrCCTV"]) ) * bmFlag;
  var shiftDelta = ( (inputs["tGapShift"] > 8) ? 0 : (8 - inputs["tGapShift"]));
  var premContacts = (accessContacts + stairsElev + crowding/ nEmp ) * (1 + (shiftDelta/8.0));

  var nGntsTlt = inputs["nM"] + inputs["nOth"]/2.0; 
  var nLdsTlt = inputs["nF"] + inputs["nOth"]/2.0; 
  var avgTltVstsPrDy = 5; 
  var avgTltDrtn = 4;
  var tltCnctrtnHrs = 4; 

  // Toilet scores
  var cRateGentsToilet = nGntsTlt * avgTltVstsPrDy * avgTltDrtn * (Math.max(0.5, (1.0 - 0.1*inputs["tClnFreq"]) )) * (1.0 - 0.1*inputs["spPrsnt"]) / (tltCnctrtnHrs*60*inputs["nGntsT"]);
  var cRateLadiesToilet = nLdsTlt * avgTltVstsPrDy * avgTltDrtn * (Math.max(0.5, (1.0 - 0.1*inputs["tClnFreq"]) )) * (1.0 - 0.1*inputs["spPrsnt"]) / (tltCnctrtnHrs*60*inputs["nLdsT"]);
  var score_sanitation = 1000;
    var sg_sanitation = "";
  console.log(cRateLadiesToilet, cRateGentsToilet);

  if (cRateGentsToilet + cRateLadiesToilet == 0) {
      score_sanitation = 1000;
      sg_sanitation = "Well done!"
  } else if (isNaN(cRateLadiesToilet) || isNaN(cRateGentsToilet)) {
        score_sanitation = "";
        sg_sanitation = "Provide inputs and recalculate";
  } else {
      score_sanitation = Math.round(Math.min( 100, 70.0/(cRateGentsToilet + cRateLadiesToilet) )) * 10;
        if (score_sanitation >= 900) { 
            sg_sanitation = "Well done!"
        } else{
            sg_sanitation = "Disinfect toilets more often";
        }
  }

  // Sick Roomz
  var score_sickRoom = 1.0 - 0.1*(inputs["iQS"]*2 + inputs["amblnc"]*2 + inputs["lHsptl"]*2 + inputs["emrgncResp"] + inputs["hl"] + 
                                  inputs["imdtFM"] * Math.max( 0, (1.0 - inputs["lstUpdtTime"]/30)) );
  var score_isolation = Math.round((1.0 - score_sickRoom) * 100) * 10;
  
  // Meeting places
  var time_brkfst = 15; // In minutes
  var time_lnch = 30;
  var time_snck = 15;
  var time_wtr = 2;
  var num_wtr_sought = 5;
  var prsnl_area = 10; // In feet
  var pvtl_hrs = 4;
  var nEmp = inputs["nM"] + inputs["nF"] + inputs["nOth"];

  var mtng_brkfst = (inputs["nBrkfst"] * time_brkfst * prsnl_area) / (inputs["cntnArea"] * 60);
  var mtng_lnch = (inputs["nLnch"] * time_lnch * prsnl_area) / (inputs["cntnArea"] * 90);
  var mntg_snck = (inputs["nSnck"] * time_snck * prsnl_area) / (inputs["cntnArea"] * 60);
  var mntg_wtr = (nEmp * time_wtr * num_wtr_sought * prsnl_area) / (inputs["nWS"] * inputs["cntnArea"] * pvtl_hrs *  60);
  
  var nEmp = inputs["nM"] + inputs["nF"] + inputs["nOth"];
  onSuccess("Total Employees: " + nEmp);

	var resTable = "";
	resTable += "<table><tr><td>Category</td>";
	resTable += "<td>Score</td>";
	resTable += "<td>Suggestions for improvements</td></tr>";
	resTable += "<tr><td>Seating arrangements and work timings</td><td>" + "1000" + "</td><td></td></tr>"
	resTable += "<tr><td>Transportation</td><td>" + "1000" + "</td><td></td></tr>"
	resTable += "<tr><td>Office infrastructure</td><td>" + "1000" + "</td><td></td></tr>"
	resTable += "<tr><td>Hygiene and sanitation</td><td>" + score_sanitation + "</td><td>" + sg_sanitation + "</td></tr>"
	resTable += "<tr><td>Awareness and readiness</td><td>" + score_isolation + "</td><td>Update employee contact lists more frequently</td></tr>"
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

