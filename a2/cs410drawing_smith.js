
function rainbowArc(r, color, context) {
	var x = .75, y = .5, start = 0, end = 1.0*Math.PI;
	var h = dboard.height;
	cntxt.beginPath();
	cntxt.strokeStyle = color;
	cntxt.lineWidth   = .025;
	cntxt.arc(x, y, r, start, end);
	cntxt.stroke();
}

function rotate(context, amount, w, h) {
	// context.clearRect(0, 0, w, h);
	
	context.translate(.75, .5);
	context.rotate(amount * Math.PI / 180)
	context.translate(-.75, -.5);
}

// style = inverted || normal
function drawRainbow(style, rotation) {
	board = document.getElementById("dboard");
	cntxt = board.getContext('2d');
	var w = dboard.width, h = dboard.height;

	cntxt.save();
	if(style === "normal"){
		cntxt.setTransform(h, 0.0, 0.0, -h, 0.0, h);
		var red = "red", orange = "orange", yellow = "yellow", green = "green", blue = "blue", purple = "purple";
	} 
	else if(style === "inverted") {
		cntxt.setTransform(h, 0.0, 0.0, -h, w/8, 0);
		var red = "DarkRed", orange = "DarkOrange", yellow = "DarkYellow", green = "DarkGreen", blue = "DarkBlue", purple = "DarkPurple";
	} 
	else {
		return;
	}

	if(rotation != 0) {
		// cntxt.rotate(rotate*Math.PI / 180);
		rotate(cntxt, rotation, w, h);
	}

	var r = .25
	rainbowArc(r, red, cntxt);
	r -= .025;
	rainbowArc(r, orange, cntxt);
	r -= .025;
	rainbowArc(r, yellow, cntxt);
	r -= .025;
	rainbowArc(r, green, cntxt);
	r -= .025;
	rainbowArc(r, blue, cntxt);
	r -= .025;
	rainbowArc(r, purple, cntxt);

	cntxt.restore();
	//http://www.w3schools.com/tags/canvas_strokestyle.asp
	// Canvas Gradient seems cool
}

function drawSpinningRainbow() {
	drawRainbow("normal", 180);
}

function init() {
	// Inverted rainbow uses a different transform, it's kind of like a rainbow shadow
	// drawRainbow("inverted", 0);
	drawRainbow("normal", 0);
	drawRainbow("normal", 180);

	// drawSpinningRainbow();
}
