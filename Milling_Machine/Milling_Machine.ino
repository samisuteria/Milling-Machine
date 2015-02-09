//Z Axis Stepper Motor Driver
#define m0Z		PE_0
#define m1Z		PE_1
#define m2Z 	PE_2
#define stepZ	PE_3
#define dirZ	PD_7

//X1 Axis Stepper Motor Driver
#define m0X1	PL_4
#define m1X1	PL_5
#define m2X1	PL_0
#define stepX1	PL_1
#define dirX1	PL_2

//X2 Axis Stepper Motor Driver
#define m0X2	PK_4
#define m1X2	PK_5
#define m2X2	PM_0
#define stepX2	PM_1
#define dirX2	PM_2

//Y Axis Stepper Motor Driver
#define m0Y		PH_0
#define m1Y		PH_1
#define m2Y		PK_6
#define stepY	PK_7
#define dirY	PM_6

//Limit Switches (Default HIGH)
#define switchXRed PH_2
#define switchXGreen PH_3
#define switchYRed 		PN_5
#define switchYGreen 	PN_4

void setup()
{
  defineStepperMotorDriverPins();
  setXandYto32();
  setZto1();
  setInitialStepandDirectionforXYZ();
  resetParseModes();

  Serial.begin(9600);
  establishContact();
}


void loop()
{
		
}

//parsing related variables
bool ignoreComments;
char parseMode; //G,M,C
int parseSubMode; //G: 0,1,4,90 M:02,03,04,05 C:None
char calibrateMode; //C,J
char calSubMode; //X,Y,X
int jogDirection; //+1 or -1

//position related variables
float nextX;
float nextY;
float nextZ;
float feedRate;
float dwellTime;

float currentX = 0;
float currentY = 0;
float currentZ = 0;


/*
serialEvent() gets called everytime there is serial input waiting. The "parser" is based on this 
and checks the incoming character and builds up the right variables. It is a horrible function and should die in 
a fire and be reborn like the phoenix at some point - but not by me and not right now. 
printOutBetterParsing() is actually just for debugging when you want to test the UI
*/

void serialEvent() {
	while(Serial.available()){
		char incoming = Serial.read();
		if(incoming == '\n'){
			printOutBetterParsing();
		}

		if(incoming == '('){
			ignoreComments = true;
		}

		if(incoming == ')'){
			ignoreComments = false;
		}

		if(incoming == '['){
			parseMode = 'C';
		}

		if(parseMode == 'C' && incoming == 'C'){
			calibrateMode = 'C';
			parseSubMode = 'C';
		}

		if(parseMode == 'C' && incoming == 'J'){
			calibrateMode = 'J';
			parseSubMode = 'J';
		}

		if(parseMode == 'C' && incoming == 'R'){
			calibrateMode = 'R';
			parseSubMode = 'R';
		}

		if(parseMode == 'C' && incoming == 'S'){
			calibrateMode = 'S';
			parseSubMode = 'S';
		}

		if(incoming == 'X' && calibrateMode == 'C'){
			calSubMode = 'X';
		}

		if(incoming == 'Y' && calibrateMode == 'C'){
			calSubMode = 'Y';
		}

		if(incoming == 'Z' && calibrateMode == 'C'){
			calSubMode = 'Z';
		}

		if(incoming == 'X' && calibrateMode == 'J'){
			calSubMode = 'X';
		}

		if(incoming == 'Y' && calibrateMode == 'J'){
			calSubMode = 'Y';
		}

		if(incoming == 'Z' && calibrateMode == 'J'){
			calSubMode = 'Z';
		}

		if(incoming == '+' && calibrateMode == 'J'){
			jogDirection = 1;
		}

		if(incoming == '-' && calibrateMode == 'J'){
			jogDirection = -1;
		}

		if(incoming == 'X' && calibrateMode == 'R'){
			calSubMode = 'X';
		}

		if(incoming == 'Y' && calibrateMode == 'R'){
			calSubMode = 'Y';
		}

		if(incoming == 'Z' && calibrateMode == 'R'){
			calSubMode = 'Z';
		}

		if(incoming == 'X' && calibrateMode == 'S'){
			calSubMode = 'X';
		}

		if(incoming == 'Y' && calibrateMode == 'S'){
			calSubMode = 'Y';
		}

		if(incoming == 'Z' && calibrateMode == 'S'){
			calSubMode = 'Z';
		}

		if(incoming == '+' && calibrateMode == 'R'){
			jogDirection = 1;
		}

		if(incoming == '-' && calibrateMode == 'R'){
			jogDirection = -1;
		}

		if(incoming == 'G' && !ignoreComments){
			parseMode = 'G';
			parseSubMode = Serial.parseInt();
		}

		if(incoming == 'M' && !ignoreComments){
			parseMode = 'M';
			parseSubMode = Serial.parseInt();
		}

		if(incoming == 'X' && parseMode == 'G'){
			nextX = Serial.parseFloat();
		}

		if(incoming == 'Y' && parseMode == 'G'){
			nextY = Serial.parseFloat();
		}

		if(incoming == 'Z' && parseMode == 'G'){
			nextZ = Serial.parseFloat();
		}

		if(incoming == 'F' && parseMode == 'G'){
			feedRate = Serial.parseFloat();
		}

		if(incoming == 'P' && parseMode == 'G'){
			dwellTime = Serial.parseFloat();
		}
	}
}

void printOutBetterParsing(){
	Serial.println("<STARTMOVE>");

	Serial.print("Mode: ");
	Serial.print(parseMode);
	Serial.print(parseSubMode);
	Serial.println();
	Serial.print("Calibrate Mode: [");
	Serial.print(calibrateMode);
	Serial.print(calSubMode);
	Serial.println(']');

	if(parseMode == '0' && parseSubMode == 0){
		Serial.println("No Command/Ignored");
	}

	if(parseMode == 'C' && calibrateMode == 'C'){
		Serial.print("Calibrating ");
		Serial.print(calSubMode);
		Serial.println(" axis.");
	}

	if(parseMode == 'C' && calibrateMode == 'S'){
		Serial.print("Zeroing ");
		Serial.print(calSubMode);
		Serial.println(" axis.");
	}

	if(parseMode == 'C' && calibrateMode == 'J'){
		Serial.print("Jogging ");
		Serial.print(calSubMode);
		Serial.print(" axis ");
		if(jogDirection > 0){
			Serial.println("up/positive.");
		} else if (jogDirection < 0){
			Serial.println("down/negative.");
		} else {
			Serial.println("no way.");
		}
	}
	if(parseMode == 'C' && calibrateMode == 'R'){
		Serial.print("Running ");
		Serial.print(calSubMode);
		Serial.print(" axis ");
		if(jogDirection > 0){
			Serial.println("up/positive.");
		} else if (jogDirection < 0){
			Serial.println("down/negative.");
		} else {
			Serial.println("no way.");
		}
	}

	if(parseMode == 'G' && parseSubMode == 0){
		Serial.println("Rapid Postioning");
	}
	if(parseMode == 'G' && parseSubMode == 1){
		Serial.println("Linear Interpolation");
	}
	if(parseMode == 'G' && parseSubMode == 4){
		Serial.print("Dwell for ");
		Serial.print(dwellTime,2);
		Serial.println(" milliseconds");
	}
	if(parseMode == 'G' && parseSubMode == 90){
		Serial.println("Absolute Programming");
	}
	if(parseMode == 'M' && parseSubMode == 02){
		Serial.println("End of Program");
	}
	if(parseMode == 'M' && parseSubMode == 03){
		Serial.println("Spindle On (clockwise)");
	}
	if(parseMode == 'M' && parseSubMode == 04){
		Serial.println("Spindle On (counter clockwise)");
	}
	if(parseMode == 'M' && parseSubMode == 05){
		Serial.println("Spindle Off");
	}
	if(parseMode == 'G' && (parseSubMode == 0 || parseSubMode == 1)){
		float dx = nextX - currentX;
		float dy = nextY - currentY;
		float dz = nextZ - currentZ;

		if(dx != 0){
			Serial.print("Move X from ");
			Serial.print(currentX,4);
			Serial.print(" to ");
			Serial.println(nextX,4);
		} else {
			Serial.println("No Change for X");
		}

		if(dy != 0){
			Serial.print("Move Y from ");
			Serial.print(currentY,4);
			Serial.print(" to ");
			Serial.println(nextY,4);
		} else {
			Serial.println("No Change for Y");
		}

		if(dz != 0){
			Serial.print("Move Z from ");
			Serial.print(currentZ,4);
			Serial.print(" to ");
			Serial.println(nextZ,4);
		} else {
			Serial.println("No Change for Z");
		}
		
		Serial.print("with Feedrate: ");
		Serial.println(feedRate);
	}

	Serial.println("-------------------------------------------");
	runParsedCommand();
	Serial.println("<ENDMOVE>");
	resetParseModes();
}

unsigned long fullRotation32 = 12800;


void runParsedCommand(){

	if(calibrateMode == 'R' && calSubMode == 'X'){
		if(jogDirection > 0){
			moveXForward(fullRotation32);
		} else if(jogDirection < 0){
			moveXBackwards(fullRotation32);
		}
	}
	if(calibrateMode == 'R' && calSubMode == 'Y'){
		if(jogDirection > 0){
			moveYForward(fullRotation32);
		} else if(jogDirection < 0){
			moveYBackwards(fullRotation32);
		}
	}
	if(calibrateMode == 'R' && calSubMode == 'Z'){
		if(jogDirection > 0){
			moveZForward(400);
		} else if(jogDirection < 0){
			moveZBackwards(400);
		}
	}

	if(calibrateMode == 'J' && calSubMode == 'X'){
		if(jogDirection > 0){
			moveXForward(400);
		} else if(jogDirection < 0){
			moveXBackwards(400);
		}
	}
	if(calibrateMode == 'J' && calSubMode == 'Y'){
		if(jogDirection > 0){
			moveYForward(400);
		} else if(jogDirection < 0){
			moveYBackwards(400);
		}
	}
	if(calibrateMode == 'J' && calSubMode == 'Z'){
		if(jogDirection > 0){
			moveZForward(100);
		} else if(jogDirection < 0){
			moveZBackwards(100);
		}
	}

	if(calibrateMode == 'C' && calSubMode == 'X'){
		calibrateXAxis();
	}
	if(calibrateMode == 'C' && calSubMode == 'Y'){
		calibrateYAxis();
	}
	if(calibrateMode == 'C' && calSubMode == 'Z'){
		Serial.println("Currently can not calibrate Z Axis - Above is a lie.");
	}

	if(calibrateMode == 'S' && calSubMode == 'X'){
		currentX = 0;
	}
	if(calibrateMode == 'S' && calSubMode == 'Y'){
		currentY = 0;
	}
	if(calibrateMode == 'S' && calSubMode == 'Z'){
		currentZ = 0;
	}

	if(parseMode == 'G' && (parseSubMode == 0 || parseSubMode == 1)){
		Serial.println("Starting Linear Interpolation");
		linearInterloption();

	}




}

void resetParseModes() {
	parseMode = '0';
	parseSubMode = 0;
	calibrateMode = '0';
	calSubMode = '0';
	jogDirection = 0;
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("Waiting for Input");
    delay(1000);
  }
}

void defineStepperMotorDriverPins(){
	pinMode(m0Z, OUTPUT);
	pinMode(m1Z, OUTPUT);
	pinMode(m2Z, OUTPUT);
	pinMode(stepZ, OUTPUT);	
	pinMode(dirZ, OUTPUT);

	pinMode(m0X1, OUTPUT);
	pinMode(m1X1, OUTPUT);
	pinMode(m2X1, OUTPUT);
	pinMode(stepX1, OUTPUT);	
	pinMode(dirX1, OUTPUT);

	pinMode(m0X2, OUTPUT);
	pinMode(m1X2, OUTPUT);
	pinMode(m2X2, OUTPUT);
	pinMode(stepX2, OUTPUT);	
	pinMode(dirX2, OUTPUT);

	pinMode(m0Y, OUTPUT);
	pinMode(m1Y, OUTPUT);
	pinMode(m2Y, OUTPUT);
	pinMode(stepY, OUTPUT);	
	pinMode(dirY, OUTPUT);

	pinMode(switchXRed, INPUT);
	pinMode(switchYRed, INPUT);
	pinMode(switchXGreen, INPUT);
	pinMode(switchYGreen, INPUT);
}

void setInitialStepandDirectionforXYZ() {
	digitalWrite(stepZ, LOW);
	digitalWrite(dirZ, LOW);

	digitalWrite(stepX1, LOW);
	digitalWrite(dirX1, LOW);

	digitalWrite(stepY, LOW);
	digitalWrite(dirY, LOW);

	digitalWrite(stepX2, LOW);
	digitalWrite(dirX2, LOW);
}

void setXandYto32(){
	digitalWrite(m0X1, HIGH);
	digitalWrite(m1X1, HIGH);
	digitalWrite(m2X1, HIGH);

	digitalWrite(m0X2, HIGH);
	digitalWrite(m1X2, HIGH);
	digitalWrite(m2X2, HIGH);

	digitalWrite(m0Y, HIGH);
	digitalWrite(m1Y, HIGH);
	digitalWrite(m2Y, HIGH);
}

void setZto1(){
	digitalWrite(m0Z, LOW);
	digitalWrite(m1Z, LOW);
	digitalWrite(m2Z, LOW);
}

void customDelay(){
	delayMicroseconds(30);
}

void customDelayZ(){
	delayMicroseconds(500);
}

float xdistanceperstep = 0.123;
float ydistanceperstep = 0.123;
float zdistanceperstep = 0.123;

void moveXForward(unsigned long steps){

	digitalWrite(dirX1, LOW);
	digitalWrite(dirX2, HIGH);
	
	for(unsigned long i=0; i<steps; i++){
	    digitalWrite(stepX1, HIGH);
	    digitalWrite(stepX2, HIGH);
	    customDelay();
	    digitalWrite(stepX1, LOW);
	    digitalWrite(stepX2, LOW);
	    customDelay();
	    currentX += xdistanceperstep;
	}
}

void moveXBackwards(unsigned long steps){

	digitalWrite(dirX1, HIGH);
	digitalWrite(dirX2, LOW);

	for(unsigned long i=0; i<steps; i++){
	    digitalWrite(stepX1, HIGH);
	    digitalWrite(stepX2, HIGH);
	    customDelay();
	    digitalWrite(stepX1, LOW);
	    digitalWrite(stepX2, LOW);
	    customDelay();
	    currentX -= xdistanceperstep;
	}
}

void moveYForward(unsigned long steps){
	digitalWrite(dirY, LOW);

	for(int i=0; i<steps; i++){
	    digitalWrite(stepY, HIGH);
	    customDelay();
	    digitalWrite(stepY, LOW);
	    customDelay();
	    currentY += ydistanceperstep;
	}
}

void moveYBackwards(unsigned long steps){
	digitalWrite(dirY, HIGH);

	for(int i=0; i<steps; i++){
	    digitalWrite(stepY, HIGH);
	    customDelay();
	    digitalWrite(stepY, LOW);
	    customDelay();
	    currentY -= ydistanceperstep;
	}
}

void moveZForward(unsigned long steps){

	digitalWrite(dirZ, LOW);

	for(int i=0; i<steps; i++){
	    digitalWrite(stepZ, HIGH);
	    customDelayZ();
	    digitalWrite(stepZ, LOW);
	    customDelayZ();
	    currentZ += zdistanceperstep;
	}
}

void moveZBackwards(unsigned long steps){
	digitalWrite(dirZ, HIGH);

	for(int i=0; i<steps; i++){
	    digitalWrite(stepZ, HIGH);
	    customDelayZ();
	    digitalWrite(stepZ, LOW);
	    customDelayZ();
	    currentZ -= zdistanceperstep;
	}
}

void calibrateXAxis(){

	Serial.println("Calibrating X Axis Now. This might take a bit.");

	while(digitalRead(switchXRed) == HIGH){
	    moveXBackwards(1);
	}
	unsigned long xaxislength = 0;
	while(digitalRead(switchXGreen) == HIGH){
		moveXForward(1);
		xaxislength++;
	}
	Serial.print("X Axis is this long in steps: ");
	Serial.println(xaxislength);
	moveXBackwards(xaxislength/2);
	currentX = 0;
}

void calibrateYAxis(){

	Serial.println("Calibrating Y Axis Now. This might take a bit.");

	while(digitalRead(switchYRed) == HIGH){
	    moveYBackwards(1);
	}
	unsigned long yaxislength = 0;
	while(digitalRead(switchYGreen) == HIGH){
		moveYForward(1);
		yaxislength++;
	}
	Serial.print("Y Axis is this long in steps: ");
	Serial.println(yaxislength);
	moveYBackwards(yaxislength/2);
	currentY = 0;
}

void linearInterloption(){

	float dx = nextX - currentX;
	float dy = nextY - currentY;
	float dz = nextZ - currentZ;

	int xdir = dx>0?1:-1;
	int ydir = dy>0?1:-1;
	int zdir = dz>0?1:-1;

	Serial.print("dx: ");
	Serial.print(dx);
	Serial.print(" dy: ");
	Serial.print(dy);
	Serial.print(" xdir: ");
	Serial.print(xdir);
	Serial.print(" ydir: ");
	Serial.print(ydir);
	Serial.println();

	dx = abs(dx);
	dy = abs(dy);
	dz = abs(dz);

	float i;
	float over = 0.0;

	if(dz > 0){
		while(dz > 0){
			if(zdir > 0){ moveZForward(1);
			} else { moveZBackwards(1); }
			dz-=zdistanceperstep;
		}
	}
	

	if(dx>dy){
		for(i=0.0;i<dx;i+=xdistanceperstep){
			if(xdir > 0){ moveXForward(1);
			} else { moveXBackwards(1); }

			over+=dy;

			if(over >= dx){
				over -= dx;

				if(ydir > 0){ moveYForward(1);
				} else { moveYBackwards(1); }
			}
			//pause but built into steps
		}
	} else {
		for(i=0.0; i<dy; i+=ydistanceperstep){
		    if(ydir > 0){ moveYForward(1);
			} else { moveYBackwards(1); }

			over+=dx;

			if(over >= dy){
				over -= dy;

				if(xdir > 0){ moveXForward(1);
				} else { moveXBackwards(1); }
			}
			//pause but built into steps
		}
	}


}
