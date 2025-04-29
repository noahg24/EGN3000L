/*
Prototype for Wall Following Robot code
by Noah Galdona

Using code from Ultrasonic Sensor testing assignment 
and Robotic Behavior Assignment without the LCD parts

This code allows for a robot utilizing 2 ultrasonic sensors (one on its front and one on its left side)
to follow a wall on its left side, being able to make both left and right turns when required. Due to
hardware limitations, this code is specific to our Minecraft Bee final prototype design, as the turning
timings are customized to it, along with how the wheels are powered since our left motor started having
issues during our final tests. It also includes some code that allows for a buzzer to be turned on and 
off when a wall is too close to the robot's front.
*/

// Pins related to the 2 ultrasonic sensors
int trigPin1 = 2;    // connect TRIG pin to Arduino pin 2 (recieves control signal) (Orange)
int echoPin1 = 3;    // connect ECHO pin to Arduino pin 3 (sends pulse signal for distance) (Yellow)
int trigPin2 = 4;    // connect TRIG pin to Arduino pin 4 (recieves control signal) (Green)
int echoPin2 = 5;    // connect ECHO pin to Arduino pin 5 (sends pulse signal for distance) (Purple)

// Pins related to the two motors/wheels
int in1 = 6; // variable names for the L298N (Left Wheel Forward) (White)
int in2 = 7; // variable names for the L298N (Left Wheel Backward) (Yellow)
int in3 = 8; // variable names for the L298N (Right Wheel Forward) (Orange)
int in4 = 9; // variable names for the L298N (Right Wheel Backward) (Blue)

// Pin related to the buzzer
int trigPin3 = 13;   // connect power of buzzer to Arduino pin 6

// Setup Checker
int setup_time = 1;

float duration_f, duration_l;  // pulse signal variables
float distance_f, distance_l; // distance variables for front and left sensors
bool front_blocked, left_blocked;

void setup() {
  // begin serial communication with 9600 baudrate speed
  Serial.begin (9600);

  // configure the trigger pins to output mode (sending control signals)
  pinMode(trigPin1, OUTPUT); // front sensor
  pinMode(trigPin2, OUTPUT); // left sensor
  pinMode(trigPin3, OUTPUT); // buzzer
  // configure the echo pins to input mode (recieving pulse info)
  pinMode(echoPin1, INPUT); // front sensor
  pinMode(echoPin2, INPUT); // left sensor
  // configure the motor pins to output mode (all L298N digital pins are outputs)
  pinMode (in1, OUTPUT); // Left Wheel Forwards Movement
  pinMode (in2, OUTPUT); // Left Wheel Backwards Movement
  pinMode (in3, OUTPUT); // Right Wheel Forwards Movement
  pinMode (in4, OUTPUT); // Right Wheel Backwards Movement
}

// Function for turning the robot 90 degrees
void turn_time(bool left_open) {
  // Have both wheels stop turning for a moment
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(1000);
  if (left_open) { // Turn Counter Clockwise
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW); // Left Wheel moves backwards
    digitalWrite(in3, HIGH); // Right Wheel moves forwards
    digitalWrite(in4, LOW);
  } else { // Turn Clockwise
    digitalWrite(in1, LOW); // Left Wheel moves forwards
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH); // Right Wheel moves backwards
  }
  delay(150); //Gives enough time for turning 90 degrees (NEEDS TESTING)
}

float measure_dist(int trigger, int echo) {
  digitalWrite(trigger, HIGH); // Sending sensor signal
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  float duration = pulseIn(echo, HIGH);
  float distance = 0.017 * duration;// (cm) Speed of sound wave, 0.034m/us divided by 2
  return distance;
}

void loop() {

  // Gives the user time to setup robot by enclosing all
  // parts and setting it up in its testing environment
  if (setup_time == 1) {
    delay(15000);
    setup_time = 0;
  }

  Serial.println("-----------------------------------------------");

  // Have both wheels stop moving for a moment for better control
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(50);

  /* Start each loop by sending out a pulse signal, recieveing the
  duration of the pulse, and calculate the distance based on that pulse */

  // generate 10-microsecond pulse to TRIG pins (control signal)
  distance_f = measure_dist(trigPin1, echoPin1);
  delayMicroseconds(10);
  distance_l = measure_dist(trigPin2, echoPin2);

  // Sets important boolean variables for later
  if(distance_f <= 30){
    front_blocked = true;
  } else {
    front_blocked = false;
  }

  if(distance_l <= 30){
    left_blocked = true;
  } else {
    left_blocked = false;
  }

  // print the value to Serial Monitor
  Serial.print("distance front: ");
  Serial.print(distance_f);
  Serial.println(" cm");
  Serial.print("distance left: ");
  Serial.print(distance_l);
  Serial.println(" cm");

  if (distance_f <= 15) {
    //Intense buzzing and move backwards
    Serial.println("Too Close! Move Back!");

    // Both wheels rotating backwards (Needed unique powering for our robot)
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW); 
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    digitalWrite(trigPin3, HIGH); // Buzzer turns on
    
  } else if (front_blocked) {
    // Buzzing noise and try to stop
    Serial.print("Should start turning! ");
    digitalWrite(trigPin3, HIGH); // Buzzer turns on
    if (left_blocked) {
      // No wall detected on left so continue following left wall
      Serial.println("Turn right.");
      turn_time(false);
    } else {
      // Wall detected on left so rotate right
      Serial.println("Turn left.");
      turn_time(true);
    }
  } else {
    // Clear to move forward
    Serial.println("Safe to proceed!");
    if (left_blocked) {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
    } else { // Both wheels move forward
      // Should begin turning left soon to follow wall
      Serial.println("Left Wall Open. Turn left.");
      turn_time(true);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
    }

  digitalWrite(trigPin3, LOW); // Buzzer turns off
  delay(150); // only sends out readings every 150 ns
}
