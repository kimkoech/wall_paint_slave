/*
Pseudo code:


Fuctions to write:
plus_x(steps) - takes int steps returns void
minus_x(steps) - "
plus_z(steps) - "
minus_z(steps) - "
engage_sharpie() - 
disengage_sharpie() -
Calibrate() 




 Created 22 Jan 2019
 by Billy Koech
*/


//Declaration of pin numbers
#define X_TOP_PIN_1 10
#define X_TOP_PIN_2 11
#define X_TOP_PIN_3 12
#define X_TOP_PIN_4 13
#define X_BTM_PIN_1 4
#define X_BTM_PIN_2 5
#define X_BTM_PIN_3 6
#define X_BTM_PIN_4 7
#define Z_PIN_1 A5
#define Z_PIN_2 A4
#define Z_PIN_3 2
#define Z_PIN_4 3
#define BUTTONS_PIN A0


// Load libraries and modules
#include <Stepper.h>
#include <Servo.h>

//Constants and Variables
int x_stepCount = 0; // number of steps the x motors have taken
int z_stepCount = 0;
int x_pos = 0; //default x position in steps
int y_pos = 0; //default y position in degrees
int z_pos = 0; //default z position
int x_btn_left_R = 146;
int x_btn_right_R = 254;
int z_btn_top_R = 434;
int z_btn_bottom_R = 1013;
int R_offset = 5;

// Initialize stepper object for x motion
Stepper xTopStepper(200,X_TOP_PIN_1,X_TOP_PIN_2,X_TOP_PIN_3,X_TOP_PIN_4);
Stepper xBtmStepper(200, X_BTM_PIN_1,X_BTM_PIN_2,X_BTM_PIN_3,X_BTM_PIN_4);

// Initialize stepper object for z motion
Stepper zStepper(200,Z_PIN_1,Z_PIN_2,Z_PIN_3,Z_PIN_4);


//x forward motion function
void move_x(int no_of_steps) {
  for (int i=1; i <= abs(no_of_steps); i++){
    xTopStepper.step(no_of_steps/abs(no_of_steps)); // move top x stepper
    xBtmStepper.step(-1*(no_of_steps/abs(no_of_steps))); // move bottom x stepper

  }
}

/*
//x forward motion function
void move_x_forward(int no_of_steps) {
  for (int i=0; i <= no_of_steps; i++){
    xTopStepper.step(1); // move top x stepper
    xBtmStepper.step(-1); // move bottom x stepper
    Serial.print(i);
    Serial.print("\n");
  }
}

//x reverse motion function
void move_x_reverse(int no_of_steps) {
  for (int i=1; i <= no_of_steps; i++){
    xTopStepper.step(-1); // move top x stepper
    xBtmStepper.step(1); // move bottom x stepper  
   }
}
*/

//z motion function
void move_z(int no_of_steps) {
  zStepper.step(-1*no_of_steps); // move top z stepper
}


// create servo object to control a servo
Servo yServo; 

// y motion function
void set_angle(int angle){
  yServo.write(angle);  // set servo angle
}


// function to move all 3 axes simultaneosly
// takes x steps to move, y angle to set servo to and z steps to move
void move_axes(int x_steps, int z_steps, int y_angle){
  int xsteps = abs(x_steps);
  int yangle = y_angle;
  int zsteps = abs(z_steps);
  while(xsteps!=0 || yangle!=0 || zsteps!=0){
     if(xsteps!=0){
      move_x(x_steps/abs(x_steps));
      xsteps -= 1;
     }
     if(yangle!=0){
      set_angle(yangle);
      yangle -= y_angle;
     }
     if(zsteps!=0){
      move_z(z_steps/abs(z_steps));
      zsteps -= 1;
     }
  }
  
}


void go_to_coordinates(int x, int z, int y_angle){
  // find difference between current position and next position
  int x_diff = x - x_pos;
  int z_diff = z - z_pos;

  //move to new postion and set x_pos and y_pos
  move_axes(x_diff, z_diff, y_angle);
}

// calibration function
bool calibrate(){
  /* Step 1: Calibrate
   * Move hub towards -x until switch is triggered at the end 
   * Move hub towards -z until switch is triggered at the top
   * Move +x while counting steps, store number of steps. No. of Steps = width.
   * Move -z while counting steps, store number of steps. No. of Steps = height. 
   * Move hub to origin, x = 0, y = 0 
   */
   bool calibrated = false;
   int sensorVal = 0; // varaible to hold analog value of buttons
   bool left_trig = false;
   bool right_trig = false;
   bool bottom_trig = false;
   bool top_trig = false;
   
   while(!left_trig){
    // move x to left until switch is trigerred
    sensorVal = analogRead(BUTTONS_PIN);
    if (x_btn_left_R-R_offset < sensorVal && sensorVal < x_btn_left_R+R_offset){
      left_trig = true; // stop moving
    }
    else{
      move_x(-1);
    }
    
   }

   
   while(!right_trig){
    // move x to right until switch is trigerred
    sensorVal = analogRead(BUTTONS_PIN);
    if (x_btn_right_R-R_offset < sensorVal && sensorVal < x_btn_right_R+R_offset){
      right_trig = true; //stop moving
      move_x(-x_stepCount);//go back to origin
      x_pos = 0; //reset x coordinate to 0
      
    }
    else{
      move_x(1);
      x_stepCount +=1; // start counting steps
    }
    
   }
   while(!bottom_trig){
    // move z to bottom until switch is trigerred
    sensorVal = analogRead(BUTTONS_PIN);
    if (z_btn_bottom_R-R_offset < sensorVal && sensorVal < z_btn_bottom_R+R_offset){
      bottom_trig = true; // stop moving
    }
    else{
      move_z(-1);
    }
    
   }

   while(!top_trig){
    // move z to top until switch is trigerred
    sensorVal = analogRead(BUTTONS_PIN);
    if (z_btn_top_R-R_offset < sensorVal && sensorVal < z_btn_top_R+R_offset){
      top_trig = true; //stop moving
      move_z(-z_stepCount);//go back to origin
      z_pos = 0; //reset x coordinate to 0
      
    }
    else{
      move_z(1);
      z_stepCount +=1; // start counting steps
    }
    
   }

   calibrated = true;
   return calibrated;
}




void setup() {

  //speed
  xTopStepper.setSpeed(60); //rpms
  xBtmStepper.setSpeed(60); //rpms
  zStepper.setSpeed(60); //rpms
  
  // attaches the servo on pin 9 to the servo object
  yServo.attach(9); 
  set_angle(110); 
  
  // initialize the serial port:
  Serial.begin(9600);

  //setup pinmode
  pinMode(X_TOP_PIN_1, OUTPUT);
  pinMode(X_TOP_PIN_2, OUTPUT);
  pinMode(X_TOP_PIN_3, OUTPUT);
  pinMode(X_TOP_PIN_4, OUTPUT);
  pinMode(X_BTM_PIN_1, OUTPUT);
  pinMode(X_BTM_PIN_2, OUTPUT);
  pinMode(X_BTM_PIN_3, OUTPUT);
  pinMode(X_BTM_PIN_4, OUTPUT);
  pinMode(Z_PIN_1, OUTPUT);
  pinMode(Z_PIN_2, OUTPUT);
  pinMode(Z_PIN_3, OUTPUT);
  pinMode(Z_PIN_4, OUTPUT);

  
}

void loop() {
  // put your main code here, to run repeatedly:

// code for going back and forth using move
/*
  for(int i=1; i<=300; i++){
    move_x(1);
    move_z(1);
  }
  for(int i=1; i<=300; i++){
    move_x(-1);
    move_z(-1);
    //set_angle((i/300)*180);
  }

  */

 // code for going back and forth using move_axes
  /*
  move_axes(300, 300, 80);
  move_axes(-300, -300, 110);
  */
 
}
