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



