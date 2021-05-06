function openNavBar() {
	document.getElementById("navBarMenu").style.width = "250px";
	document.getElementById("main").style.marginLeft = "250px";
	document.getElementById("navBarBtn").onclick = function() {closeNavBar();};
	document.getElementById("navBarBtn").innerHTML = "&times;";
}
/* Set the width of the sidebar to 0 and the left margin of the page content to 0 */
function closeNavBar() {
	document.getElementById("navBarMenu").style.width = "0";
	document.getElementById("main").style.marginLeft = "0";
	document.getElementById("navBarBtn").onclick = function() {openNavBar();};
	document.getElementById("navBarBtn").innerHTML = "&#9776;";
}

function getDeviceInfo(tableId){
	xmlhttp =new XMLHttpRequest();
	xmlhttp.open("POST","/getData",true);
	xmlhttp.setRequestHeader('Content-type','application/x-www-form-urlencoded');
	xmlhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200){
			var deviceDataArry = JSON.parse(this.responseText);
			populateDataTable(tableId,deviceDataArry);
		}
	};
	xmlhttp.send(tableId+"=1");
}

function populateDataTable(tableId,inArray){
	var tableData = document.getElementById(tableId);
	var i;
	for(i = 0; i < inArray.length; i++){
		var newRow = tableData.insertRow(i);
		var newcell1 = newRow.insertCell(0);
		var newcell2 = newRow.insertCell(1);
		newcell1.innerHTML = inArray[i].Field;
		if (tableId.includes('conf')) //create input box for tables that have Conf in name
			newcell2.innerHTML = "<input type=\"text\" class=inputBox name=\""+inArray[i].Field+"\" value=\""+inArray[i].Data+"\">";
		else //otherwise plain text
			newcell2.innerHTML = inArray[i].Data;
	}
}