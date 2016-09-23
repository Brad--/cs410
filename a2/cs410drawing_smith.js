
// function drawIt() {
// 	dboard = document.getElementById("dboard");
// 	cntxt  = dboard.getContext('2d');
// 	w = dboard.width;
// 	h = dboard.height;
// 	cntxt.save();
// 	cntxt.setTransform(h, 0.0, 0.0, -h, 0.0, h);
// 	cntxt.lineWidth=0.01; 
// 	cntxt.rect(0.2,0.2,1.0,0.6);
// 	cntxt.stroke();
// 	cntxt.restore();
// }
function rainbowArc(r, color, context) {
	var x = .75, y = .5, start = 0, end = 1.0*Math.PI;
	var h = dboard.height;
	cntxt.beginPath();
	cntxt.strokeStyle = color;
	cntxt.lineWidth   = .025;
	cntxt.arc(x, y, r, start, end);
	cntxt.stroke();
}

function drawRainbow() {
	board = document.getElementById("dboard");
	cntxt = board.getContext('2d');
	var w = dboard.width, h = dboard.height;

	cntxt.save();
	cntxt.setTransform(h, 0.0, 0.0, -h, 0.0, h);
	var r = .25
	rainbowArc(r, "red", cntxt);
	r -= .025;
	rainbowArc(r, "orange", cntxt);
	r -= .025;
	rainbowArc(r, "yellow", cntxt);
	r -= .025;
	rainbowArc(r, "green", cntxt);
	r -= .025;
	rainbowArc(r, "blue", cntxt);
	r -= .025;
	rainbowArc(r, "purple", cntxt);


	cntxt.restore();
	//http://www.w3schools.com/tags/canvas_strokestyle.asp
	// Canvas Gradient seems cool
}

function init() {
	drawRainbow();
	// drawIt();
}
