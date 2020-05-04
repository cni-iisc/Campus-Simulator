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
  dict["est_type"] = document.getElementById("NOE").value; // Nature of Establishment
  
  // Employee Information
  dict["nM"] = document.getElementById("nM").value; // Number of male employees
  dict["nF"] = document.getElementById("nF").value; // Number of female employees
  dict["nOth"] = document.getElementById("nOth").value; // Number of other employees
  dict["rAddrKn"] = document.getElementById("rAddrKn").value; // Number of employees with address records
  dict["pCS"] = document.getElementById("pCS").value; // Percentage of casual labour and security
  dict["nShifts"] = document.getElementById("nShifts").value; // Number of shifts
  dict["tGapShift"] = document.getElementById("tGapShift").value; // Time gap between shifts in hours
  dict["informCZEmp"] = parseInt(document.querySelector('input[name="informCZEmp"]:checked').value); // Inform containment zone employee not to come
  dict["n19"] = document.getElementById("n19").value; // Number of employees with age betwwen 15 and 19
  dict["n29"] = document.getElementById("n29").value; // Number of employees with age betwwen 20 and 29
  dict["n39"] = document.getElementById("n39").value; // Number of employees with age betwwen 30 and 39
  dict["n49"] = document.getElementById("n49").value; // Number of employees with age betwwen 40 and 49
  dict["n59"] = document.getElementById("n59").value; // Number of employees with age betwwen 50 and 59
  dict["n60plus"] = document.getElementById("n60plus").value; // Number of employees with age more than 60
  dict["nASapp"] = document.getElementById("nASapp").value; // Number of employees with Aarogya Setu app
  dict["fracGCGapp"] = document.getElementById("fracGCGapp").value; // Fraction of employees with Go Corona Go app

  //Office Infrastructure Information
  dict["tArea"] = document.getElementById("tArea").value; // Total office area in sq.ft
  dict["nFloors"] = document.getElementById("nFloors").value; // Number of floors in office
  dict["avgFA"] = document.getElementById("avgFA").value; // Average area of floors in sq.ft
  dict["avgFEmp"] = document.getElementById("avgFEmp").value; // Average number of employees per floor
  dict["mntrCCTV"] = parseInt(document.querySelector('input[name="mntrCCTV"]:checked').value); // CCTV monitoring
  dict["acsCntrl"] = parseInt(document.querySelector('input[name="acsCntrl"]:checked').value); // Access controlled
  dict["baDoor"] = document.getElementById("baDoor").value; // Number of biometric based access doors
  dict["rfidDoor"] = document.getElementById("rfidDoor").value; // Number of RFID based access doors
  dict["sntBio"] = parseInt(document.querySelector('input[name="sntBio"]:checked').value); // Sanitiser at Biometric access doors

  dict["nCW"] = document.getElementById("nCW").value; // Number of companies in Cowork space

  //Office Composition
  dict["nSOfcRm"] = document.getElementById("nSOfcRm").value; // Number of single office rooms
  dict["n2pOfcRm"] = document.getElementById("n2pOfcRm").value; // Number of office rooms with 2 people
  dict["n2pPlusOfcRm"] = document.getElementById("n2pPlusOfcRm").value; // Number of office rooms with more than 2 people
  dict["nCub"] = document.getElementById("nCub").value; // Number of office seats with cubicle separation
  dict["nRem"] = document.getElementById("nRem").value; // Number of remaining office seats
  dict["percAc"] = document.getElementById("percAC").value; // Percentage of air conditioned premise
  dict["tempAC"] = document.getElementById("tempAC").value; // Temperature setting
  dict["humAC"] = document.getElementById("humAC").value; // Relative humidity setting

  //Common Usage Area
  dict["nEle"] = document.getElementById("nEle").value; // Number of Elevetors
  dict["nEleDinf"] = document.getElementById("nEleDinf").value; // Frequency of elevetor disinfection process
  dict["nStrCln"] = document.getElementById("nStrCln").value; // Frequency of stairway cleaning
  dict["nStrHrDinf"] = document.getElementById("nStrHrDinf").value; // Frequency of stairway handrails disinfection process
    //console.log("Bool: " + dict["informCZEmp"] )
    //var xyz = 1.0 * dict["informCZEmp"];
    //console.log("Val: " + xyz )
  return dict;
}

function calcScore () {
    form_inputs = getValues();
    console.log(form_inputs);

	var est_type = document.getElementById("NOE").value;
	var nM = parseInt(document.getElementById("nM").value);
	var nF = parseInt(document.getElementById("nF").value);
	var nOth = parseInt(document.getElementById("nOth").value);
	var rAddrKn = parseInt(document.getElementById("rAddrKn").value);
	var pCS = parseInt(document.getElementById("pCS").value);
	//var num_ASApp = parseInt(document.getElementById("num_ASApp").value);
	var nShifts = parseInt(document.getElementById("nShifts").value);
	var tGapShift = parseFloat(document.getElementById("tGapShift").value);
	
	//Toilets
	var nGentsT = parseInt(document.getElementById("nGentsT").value);
	var nLadiesT = parseInt(document.getElementById("nLadiesT").value);
	var tCleanFreq = parseInt(document.getElementById("tCleanFreq").value);
	var soapDisp_flag = parseInt(document.querySelector('input[name="soap_present"]:checked').value);
    var nUsersG = nM + nOth/2.0;
    var nUsersL = nF + nOth/2.0;
    var nAVT = 5;
    var durAVT = 4;
    var tConcHr = 4;
    //var scoreGentsToilet = NgentsToilet * AveToiletVisitsPerDay * AveToiletDuration / NgentsToilet / (ToiletConcHrs * 60) * max(0.5 - NgentsCleaning*0.1) * (1 - 0.1*soapDispensed)
    var cRateGentsToilet = nUsersG * nAVT * durAVT * (Math.max(0.5, (1.0 - 0.1*tCleanFreq) )) * (1.0 - 0.1*soapDisp_flag) / (tConcHr*60*nGentsT);
    var score_GentsToilet = cRateGentsToilet;
    var cRateLadiesToilet = nUsersL * nAVT * durAVT * (Math.max(0.5, (1.0 - 0.1*tCleanFreq) )) * (1.0 - 0.1*soapDisp_flag) / (tConcHr*60*nLadiesT);
    var score_sanitation = 1000;
    console.log(cRateLadiesToilet, cRateGentsToilet);
    if (cRateGentsToilet + cRateLadiesToilet == 0) {
        score_sanitation = 1000;
    } else {
        score_sanitation = Math.round(Math.min( 100, 70.0/(cRateGentsToilet + cRateLadiesToilet) )) * 10; // change to round
    }


    // Sick Room
    var HL_flag = parseInt(document.querySelector('input[name="HL"]:checked').value);
    var IQS_flag = parseInt(document.querySelector('input[name="IQS"]:checked').value);
    var Amblnc_flag = parseInt(document.querySelector('input[name="Amblnc"]:checked').value);
    var LHsptl_flag = parseInt(document.querySelector('input[name="LHsptl"]:checked').value);
    var EmrgncResp_flag = parseInt(document.querySelector('input[name="EmrgncResp"]:checked').value);
    var ImdtFM_flag = parseInt(document.querySelector('input[name="ImdtFM"]:checked').value);
	var lstUpdtTime = parseInt(document.getElementById("lstUpdtTime").value);
    var score_sickRoom = 1.0 - 0.1*(IQS_flag*2 + Amblnc_flag*2 + LHsptl_flag*2 + EmrgncResp_flag + HL_flag + 
             ImdtFM_flag * Math.max( 0, (1.0 - lstUpdtTime/30)) );
    var score_isolation = Math.round((1.0 - score_sickRoom) * 100) * 10;

	var nEmp = nM + nF + nOth;
	
	onSuccess("Total Employees: " + nEmp);

	var resTable = "";
	resTable += "<table><tr><td>Category</td>";
	resTable += "<td>Score</td>";
	resTable += "<td>Suggestions for improvements</td></tr>";
	resTable += "<tr><td>Seating arrangements and work timings</td><td>" + "1000" + "</td><td></td></tr>"
	resTable += "<tr><td>Transportation</td><td>" + "1000" + "</td><td></td></tr>"
	resTable += "<tr><td>Office infrastructure</td><td>" + "1000" + "</td><td></td></tr>"
	resTable += "<tr><td>Hygiene and sanitation</td><td>" + score_sanitation + "</td><td>Disinfect the toilets more often</td></tr>"
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



