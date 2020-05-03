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
  dict["informCZEmp"] = Boolean (document.getElementById("informCZEmp").value); // Inform containment zone employee not to come
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
  dict["mntrCCTV"] = Boolean (document.getElementById("mntrCCTV").value); // CCTV monitoring
  dict["acsCntrl"] = Boolean (document.getElementById("acsCntrl").value); // Access controlled
  dict["baDoor"] = document.getElementById("baDoor").value; // Number of biometric based access doors
  dict["rfidDoor"] = document.getElementById("rfidDoor").value; // Number of RFID based access doors
  dict["sntBio"] = Boolean (document.getElementById("sntBio").value); // Sanitiser at Biometric access doors
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
}

function calcScore () {
	var est_type = document.getElementById("NOE").value;
	var nM = parseInt(document.getElementById("nM").value);
	var nF = parseInt(document.getElementById("nF").value);
	var nOth = parseInt(document.getElementById("nOth").value);
	var rAddrKn = parseInt(document.getElementById("rAddrKn").value);
	var pCS = parseInt(document.getElementById("pCS").value);
	//var num_ASApp = parseInt(document.getElementById("num_ASApp").value);
	var nShifts = parseInt(document.getElementById("nShifts").value);
	var tGapShift = parseFloat(document.getElementById("tGapShift").value);
	var nGentsT = parseInt(document.getElementById("nGentsT").value);
	var nLadiesT = parseInt(document.getElementById("nLadiesT").value);
	var tCleanFreq = parseInt(document.getElementById("tCleanFreq").value);
	
    //Read all booleans here:
	var soapDisp_flag = parseInt(document.querySelector('input[name="soapDisp_flag"]:checked').value);
	console.log("SoapDisp: " + soapDisp_flag)

    var nUsersG = nM + nOth/2.0;
    var nUsersL = nF + nOth/2.0;
    var nAVT = 5;
    var durAVT = 4;
    var tConcHr = 4;
    //var scoreGentsToilet = NgentsToilet * AveToiletVisitsPerDay * AveToiletDuration / NgentsToilet / (ToiletConcHrs * 60) * max(0.5 - NgentsCleaning*0.1) * (1 - 0.1*soapDispensed)
    var cRateGentsToilet = nUsersG * nAVT * durAVT * (Math.max(0.5, (1.0 - 0.1*tCleanFreq) )) * (1.0 - 0.1*soapDisp_flag) / (tConcHr*60*nGentsT);
    var score_GentsToilet = cRateGentsToilet;
    var cRateLadiesToilet = nUsersL * nAVT * durAVT * (Math.max(0.5, (1.0 - 0.1*tCleanFreq) )) * (1.0 - 0.1*soapDisp_flag) / (tConcHr*60*nLadiesT);
    var score_LadiesToilet = cRateLadiesToilet;
    //var score_GentsToilet = 1000 - cRateGentsToilet*1000;

	var nEmp = nM + nF + nOth;
	
	onSuccess("Total Employees: " + nEmp);

	var resTable = "<br><br><H3> Covid Readiness Score</H3>";
	resTable += "<table><tr><td>Category</td>";
	resTable += "<td>Score</td>";
	resTable += "<td>Possible improvement</td></tr>";
	resTable += "<tr><td>Gents toilets</td><td>" + score_GentsToilet + "</td><td></td></tr>"
	resTable += "<tr><td>Ladies toilets</td><td>" + score_LadiesToilet + "</td><td></td></tr>"
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
	openPage('Scores', document.getElementById("ScoresTab"), '#669977')
}

function reEnter() {
	openPage('Qn', document.getElementById("QnTab"), '#886655')
}



