var board, context, w, h, theta = 0.0;

function rainbowArc(r, color) {
	// x and y are preset so it draws centered rainbows
	var x = .75, y = .5, start = 0, end = 1.0*Math.PI;
	context.beginPath();
	context.strokeStyle = color;
	context.lineWidth   = .025;
	context.arc(x, y, r, start, end);
	context.stroke();
}

function rotate(amount) {
	// translate to center, rotate, and translate back so that you don't get a weird skew-ish thing
	context.translate(.75, .5);
	context.rotate(amount * Math.PI / 180)
	context.translate(-.75, -.5);
}

function drawRainbow(style, rotation) {

	context.save();
	// Set all the color variables based on normal or dark rainbow
	if(style === "normal"){
		context.setTransform(h, 0.0, 0.0, -h, 0.0, h);
		var red = "red", orange = "orange", yellow = "yellow", green = "green", blue = "blue", purple = "purple";
	} 
	else if(style === "dark") {
		context.setTransform(h, 0.0, 0.0, -h, 0.0, h);
		var red = "DarkRed", orange = "DarkOrange", yellow = "DarkYellow", green = "DarkGreen", blue = "DarkBlue", purple = "DarkPurple";
	} 

	if(rotation != 0)
		rotate(rotation);
	
	// Decrementing the radius by the linewidth
	var r = .25
	rainbowArc(r, red);
	r -= .025;
	rainbowArc(r, orange);
	r -= .025;
	rainbowArc(r, yellow);
	r -= .025;
	rainbowArc(r, green);
	r -= .025;
	rainbowArc(r, blue);
	r -= .025;
	rainbowArc(r, purple);

	context.restore();
	//http://www.w3schools.com/tags/canvas_strokestyle.asp
	// Canvas Gradient seems cool
}

function drawStar(cx, fill, color) {
	context.save();
	context.setTransform(h, 0.0, 0.0, -h, cx, h);
	context.translate(.75, .5);
	context.strokeStyle = color;
	context.fillStyle   = color;
	context.lineWidth   = .0125;

	// Center of the rainbow is radius .125
	context.beginPath();
	context.moveTo(0, 0.12); // Top
	context.lineTo( 0.0625, -0.0625); // Bottom Right
	context.lineTo(-0.0833,  0.05); // Left
	context.lineTo( 0.0833,  0.05); // Right
	context.lineTo(-0.0625, -0.0625); // Bottom Right
	context.lineTo(0, 0.12); // Top

	if(fill) 
		context.fill();
	else
		context.stroke();
	context.restore();
}

function drawRectangle(cx, color) {
	context.save();
	context.setTransform(h, 0.0, 0.0, -h, cx, h);
	// context.translate(.75, .5);
	context.strokeStyle = color;
	context.fillStyle   = color;
	context.lineWidth   = .0125;
	context.rect(.1, .2, .2, .1);
	context.stroke();
	context.restore();
}

// This is Professor Beveridges from the rotating pacman lecture
function clearScene() {
	context.save();
	context.setTransform(h, 0.0, 0.0, -h, 0.0, h);
	context.clearRect(0, 0, w, h);
	context.restore();
}

function drawScene() {
	clearScene();
	drawRectangle(0, "red");
	drawRectangle(-20, "blue");
	drawStar(0, true, "yellow");
	drawStar(w/3, false, "purple");
	drawRainbow("normal",  theta);
	drawRainbow("dark"  , -theta);
	theta = (theta + 5) % 360;
	setTimeout(drawScene, 100);
}

function init() {
	board   = document.getElementById("dboard");
	context = board.getContext('2d');
	w = board.width;
	h = board.height;
	drawScene();
}
