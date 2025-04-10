/*
Prototype for Wall Following Robot code
by Noah Galdona

Using code from Ultrasonic Sensor testing assignment 
and Robotic Behavior Assignment without the LCD parts

Timings still need to be tested for how long it takes the robot to turn.
Distance ranges may need to be updated.
*/

// Pins related to the 2 ultrasonic sensors
int trigPin1 = 2;    // connect TRIG pin to Arduino pin 2 (recieves control signal)
int echoPin1 = 3;    // connect ECHO pin to Arduino pin 3 (sends pulse signal for distance)
int trigPin2 = 4;    // connect TRIG pin to Arduino pin 4 (recieves control signal)
int echoPin2 = 5;    // connect ECHO pin to Arduino pin 5 (sends pulse signal for distance)

// Pins related to the two motors/wheels
int in1 = 7; // variable names for the L298N (Left Wheel Forward)
int in2 = 8; // variable names for the L298N (Left Wheel Backward)
int in3 = 9; // variable names for the L298N (Right Wheel Forward)
int in4 = 10; // variable names for the L298N (Right Wheel Backward)

// Pin related to the buzzer
int trigPin3 = 6;   // connect power of buzzer to Arduino pin 6

float duration_f, duration_l;  // pulse signal variables
float distance_f, distance_l; // distance variables for front and left sensors

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
  if (left_open) { // Turn Counter Clockwise
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH); // Left Wheel moves backwards
    digitalWrite(in3, HIGH); // Right Wheel moves forwards
    digitalWrite(in4, LOW);
  } else { // Turn Clockwise
    digitalWrite(in1, HIGH); // Left Wheel moves forwards
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH); // Right Wheel moves backwards
  }
  delay(2000); //Gives enough time for turning 90 degrees (NEEDS TESTING)
}

void loop() {
  /* Start each loop by sending out a pulse signal, recieveing the duration of
  the pulse, and calculate the distance based on that pulse*/

  // generate 10-microsecond pulse to TRIG pins (control signal)
  digitalWrite(trigPin1, HIGH); // Checking front sensor
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);

  digitalWrite(trigPin2, HIGH); // Checking left sensor
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);

  // measure duration of pulse from ECHO pin (recieve resulting pulse)
  /* if within the 10 microsecond delay no return pulse was recieved, a pulse
  value of zero is returned. This could indicate no wall is there and that it
  could turn in that direction.*/
  duration_f = pulseIn(echoPin1, HIGH);
  duration_l = pulseIn(echoPin2, HIGH);

  // calculate the distance
  distance_f = 0.017 * duration_f;// (cm) Speed of sound wave, 0.034m/us divided by 2
  distance_l = 0.017 * duration_l;

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance_f);
  Serial.println(" cm");

  if (distance_f <= 15) {
    //Intense buzzing and move backwards
    Serial.print("Too Close! Move Back!");

    // Both wheels rotating backwards
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH); 
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    digitalWrite(trigPin3, HIGH); // Buzzer turns on
    
  } else if (distance_f <= 30) {
    // Buzzing noise and try to stop
    Serial.print("Should start turning!");
    digitalWrite(trigPin3, HIGH); // Buzzer turns on
    if (distance_l > 30) {
      // No wall detected on left so continue following left wall
      Serial.print("Turn left.");
      turn_time(true);
    } else {
      // Wall detected on left so rotate right
      Serial.print("Turn right.");
      turn_time(false);
    }
  } else if (distance_f <= 45) {
    // Clear to move forward
    Serial.print("Safe to proceed!");
    digitalWrite(trigPin3, LOW); // Buzzer turns off
    if (distance_l > 30) {
      // Should begin turning left soon to follow wall
      Serial.print("Turn left.");
      turn_time(true);
    } else { // Both wheels move forward
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
    }
  } else {
    // Out of Range of Sensor
    Serial.print("No Wall to read!");
    digitalWrite(trigPin3, LOW); // Buzzer turns off
    // Move forward until walls are found
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }

  delay(500); // only sends out readings every 500 ns (MAY NEED TO UPDATE)
}
