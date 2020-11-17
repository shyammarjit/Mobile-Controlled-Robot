// SHYAM MARJIT
// This is the final project for IIT-Kanpur Summer-Internship-2020
// I am persuing my B.tech in CSE from IIIT Guwahtai. Currently I am in 2nd year of my B.Tech.
// sujaykumarmarjit@gmail.com
// ph-8927860176

#include<ESP8266WiFi.h>

const char* ssid = "shyammarjit";        // SSID i.e Service Set Identifier is the name of your mobile WIFI
const char* password = "shyam123";       //  Your WiFi password, in case you have open network comment the whole statement.


//.....................................................................................................................................................

// Motor Control pins and sonar sensor pins
#define led D0            // Defining led with Node-Mcu D0 Pin
#define ML D1             // Defining ML with Node-Mcu D1 Pin
#define MR D3             // Defining MR with Node-Mcu D3 Pin

#define trigpin D5        // Defining Trig-Pin of the motor driver with Node-Mcu D5 Pin
#define echopin D6        // Defining Echo-Pin of the motor driver with Node-Mcu D6 Pin

#define ENA D7            // Defining enable-A Pin of the motor driver with Node-Mcu D7 Pin
#define ENB D8            // Defining enable-B Pin of the motor driver with Node-Mcu D8 Pin
#define device_pin D4     // Defining device_Pin of the motor driver with Node-Mcu D4 Pin

//.....................................................................................................................................................

WiFiServer server(80);    //  Creates a server that listens for incoming connections on the specified port, here in this case port is 80
int flag;

//......................................................................................................................................................

void setup()
{
  // Put your setup code here, to run once:
  pinMode(led, OUTPUT);   // Sets the digital led pin as output
  pinMode(ML, OUTPUT);    // Sets the ML pin 13 as output
  pinMode(MR, OUTPUT);    // Sets the MR as output    

  pinMode(ENA, OUTPUT);   // Sets the ENA as output  
  pinMode(ENB, OUTPUT);   // Sets the ENB as output

  analogWrite(ENA, 1023);   // Seeting ENA to 1023
  analogWrite(ENB, 1023);   // Seeting ENA to 1023
 
  pinMode(trigpin, OUTPUT); // Sets the ENA as output  
  pinMode(echopin, INPUT);  // Sets the ENA as output  
  Serial.begin(115200);     // Opens serial port, sets data rate to 115200 bps

  digitalWrite(led, LOW);   // Sets the LED off  
  digitalWrite(ML, HIGH);   // Sets the ML high
  digitalWrite(MR, HIGH);   // Sets the MR high

  pinMode(device_pin,OUTPUT); // Sets the device_pin as output  

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  /*
   The following four lines of the code will assign a static IP Address to the ESP Module. If you do not want this, comment out the following four lines
   */
  //IPAddress ip(192, 168, 1, 254);
  //IPAddress gateway(192, 168, 1, 1);
  //IPAddress subnet(255, 255, 255, 0);
  //WiFi.config(ip, gateway, subnet);
  //delay(5000);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);               // Delay of 500ms
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP()); // Gets the WiFi shield's IP address and Print the IP address of serial monitor
  Serial.println("/");
}
//..........................................................................................................................................................

int getdistance_cm()      // Function to compute the distance in cm
{
  long duration;
  int distance;
  digitalWrite(trigpin, LOW);         // Set the trigpin as LOW ic set the trigpin off
  delayMicroseconds(2);               // Pauses for 2 microseconds
  digitalWrite(trigpin, HIGH);        // Set the trigpin as HIGH ic set the trigpin on
  delayMicroseconds(15);              // Pauses for 2 microseconds
  digitalWrite(trigpin, LOW);         // Again set the trigpin as LOW ic set the trigpin off
  duration = pulseIn(echopin, HIGH);  // The Node-MCU measures the length the signal pin is HIGH with this
  // d' = v*t  where  sound velocity v = 343 mtr/sec  , t = duration
  // d' = (343*100 * (10^-6) cm)* duration  =  0.0343 * duration cm
  //  distance = d' /2 ;

  distance = (0.0343 * duration) / 2;    // Computing the distance


  delay(100);       // Delay for 100ms
  return distance;  // Return the distance to the called function
}

//..........................................................................................................................................................

// Function to move the Mobile-Robot forward
void forward()
{
  digitalWrite(ML, LOW);  // Set the ML as Low
  digitalWrite(MR, LOW);  // Set the MR as Low

}

//..........................................................................................................................................................

// Function to turn the Mobile-Robot towards the left i.c the anticlock-wise motion
void left_turn()
{
  digitalWrite(ML, HIGH);   // Set the ML as HIGH
  digitalWrite(MR, LOW);    // Set the MR as Low

}
//..........................................................................................................................................................

// Function to turn the Mobile-Robot towards the right i.c the clock-wise motion
void right_turn()
{
  digitalWrite(ML, LOW);    // Set the ML as Low
  digitalWrite(MR, HIGH);   // Set the MR as Low

}

//..........................................................................................................................................................

// Function to stop the robot at that instant
void stop_now()
{
  digitalWrite(ML, HIGH);   // Set the ML as HIGH
  digitalWrite(MR, HIGH);   // Set the MR as HIGH

}


//..........................................................................................................................................................
void loop()
{
   
  // Put your main code here, to run repeatedly:
  int distance_cm = getdistance_cm();
 
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available())
  {
    delay(1);   //Delay for 1ms
  }
 
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  //client.println("<!DOCTYPE HTML>");
  //client.println("<html>");
 
  client.println(distance_cm);    // Print the distance in cm
 
  //client.println(analogRead(AO));
  //client.println("</html>");
  delay(1);   //Delay for 1ms

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();


  // Match the request

  //..................................................................Forward Motion.................................................
 
  if (request.indexOf("/Forward") != -1)    // Forward Motion request
  {
    Serial.print("distance = ");
    Serial.println(distance_cm);  // Printing the distance in cm
    forward();    // Forward function call
  }

  if (request.indexOf("/Stop_F") != -1)     // Stopping the Forward Motion request
  {
    stop_now();    // Stop Forward function call
    Serial.println("Robot Stop");
  }

  //..................................................................Left Motion.................................................
 
  if (request.indexOf("/Left") != -1)       // Left Motion i.c. anticlockwise motion request
  {
    left_turn();    // Left function call
    Serial.println("Robot turning left");
  }

  if (request.indexOf("/Stop_L") != -1)     // Stopping request of left motion
  {
    stop_now();   // Stop left function call
    Serial.println("Robot Stop");
  }

  //..................................................................Right Motion.................................................
 
  if (request.indexOf("/Right") != -1)      // Right Motion i.c. clockwise motion request
  {
    right_turn();   // Right function call
    Serial.println("Robot turning Right");
  }

  if (request.indexOf("/Stop_R") != -1)     // Stopping request of Right motion
  {
    stop_now();   // Stop Right function call
    Serial.println("Robot Stop");
  }

  //..................................................................Device Control.................................................

  if (request.indexOf("/device_on") != -1)   // Device on request
  {
    digitalWrite(device_pin, LOW);
    stop_now();    // Stop function call
    Serial.println("Device ON");
  }

  if (request.indexOf("/device_off") != -1)   // Device off request
  {
    digitalWrite(device_pin, HIGH);
    Serial.println("Device OFF");
  }


  // Return the responce
  /*client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println(distance_cm);
    client.println("</html>");
  */

}
