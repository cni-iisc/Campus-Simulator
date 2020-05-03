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
	/*
	var est_name = document.getElementById("est_name").value;
	var est_type = document.getElementById("est_type").value;
	var num_emp_m = parseInt(document.getElementById("num_emp_m").value);
	var num_emp_f = parseInt(document.getElementById("num_emp_f").value);
	var num_emp_o = parseInt(document.getElementById("num_emp_o").value);
	//var res_add = document.getElementById("res_add").value;
	var num_ASApp = parseInt(document.getElementById("num_ASApp").value);
	var num_shifts = parseInt(document.getElementById("num_shifts").value);
	var gap_shifts_min = parseInt(document.getElementById("gap_shifts_min").value);
	    
	var num_emp = num_emp_m + num_emp_f + num_emp_o; */
	var num_emp = 100;
	onSuccess("Total Employees: " + num_emp);

	var resTable = "<br><br><H3> Covid Readiness Score</H3>";
	resTable += "<table><tr><td style='width: 200px; color: white;'>Category</td>";
	resTable += "<td style='width: 100px; color: white;'>Score</td>";
	resTable += "<td style='width: 200px; color: white;'>Improvement</td></tr>";
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
	openPage('Scores', document.getElementById("ScoresTab"), 'darkgreen')
}

function reEnter() {
	openPage('Qn', document.getElementById("QnTab"), '#777777')
}



