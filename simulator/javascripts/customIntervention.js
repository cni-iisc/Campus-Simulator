$("#customIntv").hide();

	$('input[type="radio"]').click(function(){
		if ($(this).is(':checked'))
		{
			var value = $(this).val();
			if (value == 11){$("#customIntv").show();}
			else{$("#customIntv").hide();}
		}
	});

    // Create a new list itemwhen clicking on the "Add"button

    const mapping = {
        "0": "No Intervention (NI)",
        "1": "Case Isolation (CI)",
        "2": "Home Quarantine (HQ)",
        "3": "Lockdown (LD)",
        "4": "CI and HQ of infected household (CIHQI)",
        "5": "CIHQI and social distancing of those above 65 years of age"
    }

    let count = 0

    function newInterv() {
        let li = document.createElement("li");
        li.number = count
        li.className = "interv-li"
        /*
        let intervCode = document.getElementById("interv-choice").value;
        let intervTime = document.getElementById("interv-time").value;
        let intervName = mapping[intervCode];
        */

        // Create li div for everything
        let liDiv = document.createElement("div");
        liDiv.className = "li-interv-div"
        let selectInterv = document.createElement("select")
        const createOption = (value, text) => {
            let option = document.createElement("option");
            option.appendChild(document.createTextNode(text))
            option.value = String(value)
            return option;
        }

        for (let code in mapping) {
            selectInterv.appendChild(createOption(code, mapping[code]))
        }
        selectInterv.className = "li-interv-select"

        let input = document.createElement("input")
        input.className = "li-interv-time"
        input.type = "text"
        input.placeholder = "Duration in days"

        liDiv.appendChild(selectInterv)
        liDiv.appendChild(input)

        li.appendChild(liDiv);

        let close = document.createElement("input");
        close.type = "button"
        close.value = "\u2715"
        liDiv.appendChild(close);
        close.onclick = () => {
            li.parentNode.removeChild(li);
        }


        document.getElementById("interv").appendChild(li);

        count++
    }