// WiFly HTTP Server Servo Control
// by Chris Grimmett <http://grimtech.net> 
// This example code is in the public domain.
//
//
// TEH PARTZ
//
//   Arduino UNO                          (http://www.adafruit.com/products/50)
//   Adafruit XBee adapter                (https://www.adafruit.com/products/126)
//   Rocket Brand Studios Pan Tilt Kit    (https://sites.google.com/a/rocketbrandstudios.com/rocket-brand-studios/store/hardware/pan-tilt-kit)
//   RN-XV WiFly Module                   (https://www.sparkfun.com/products/10822)
//
// TEH CIRCUIT
//
//   * WiFly TX pin      to digital pin 1
//   * WiFly RX pin      to digital pin 2
//   * Servo signal pin  to digital pin 8
//   



//
// INCLUDES
//
#include <Servo.h> 
#include <WiFlyHQ.h>

//
// CONFIG
//
const char ssid[] = "changeme";
const char password[] = "changeme";
const int ptzDelay = 10;  // default rate at which the camera will move. lower is faster

int ssRx = 4;
int ssTx = 5;

//
// INIT
//
Servo servoPan;             // create servo object to control a servo 
//SsServo servoTilt;        // a maximum of eight servo objects can be created 
int panPos = 90;             // variable to store the servo position 
unsigned long now;
unsigned long lastPanStep;  // last point in time at which the pan server stepped left or right
WiFly wifly;

void sendIndex();
//void sendInfo(char *info);
void send404();

char buf[80]; // buffer for talking to WiFly

//
// FUNKYS
//
  
 
/**
 * Pan, tilt, and/or zoom the camera.
 * rate controlled using r. 0 for immediate movement,
 * or set an arbitrary millisecond delay inbetween
 * each movement step.
 *
 * @param {int} p    pan to this position
 * @parma {int} t    tilt to this position    
 * @param {int} z    zoom to this position
 * @param {int} d    millisecond delay between moving 1 step
 */
void ptz(int p, int t, int z, int d = ptzDelay) {
  boolean panLeft;
  
  
  // find if we're moving left (p is positive number)
  // or right (p is negative)
  if (p < panPos) {
    // user requested to pan right
    panLeft = false;
    
    
  } else if (p > panPos) {
    // user requested to pan left
    panLeft = true;
  }
  
    
  while (p != panPos) {
    // we still need to pan
    // to reach requested position
      
      
    now = millis();
    if (now - lastPanStep > d) {
      // delay specified between steps has elapsed
      // and now it's time to step the servo
      
      
      if (panLeft) {
        servoPan.write(panPos+=1);
      } else {
        servoPan.write(panPos-=1);
      }
      lastPanStep = now;
      
    }
  }
 
  // got to requested position

} 


void sendIndex() {
  wifly.println(F("HTTP/1.1 200 OK"));
  wifly.println(F("Content-Type: text/html"));
  wifly.println(F("Transfer-Encoding: chunked"));
  wifly.println();
  
  wifly.sendChunkln(F("<html>"));
  wifly.sendChunkln(F("<title>Pan Tilt Control</title>"));
  wifly.sendChunkln(F("<h1>"));
  wifly.sendChunkln(F("<p>Pan Tilt Control</p>"));
  wifly.sendChunkln(F("</h1>"));
  wifly.sendChunkln(F("<form name=\"input\" action=\"/\" method=\"post\">"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"left\" />"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"right\" />"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"pos1\" />"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"pos2\" />"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"pos3\" />"));
  wifly.sendChunkln(F("</form>")); 
  wifly.sendChunkln(F("</html>"));
  wifly.sendChunkln();
}

void sendInfo(char *info) {
  wifly.println(F("HTTP/1.1 200 OK"));
  wifly.println(F("Content-Type: text/html"));
  wifly.println(F("Transfer-Encoding: chunked"));
  wifly.println();
  
  wifly.sendChunkln(F("<html>"));
  wifly.sendChunkln(F("<title>Pan Tilt Control</title>"));
  wifly.sendChunkln(F("<h1>"));
  wifly.sendChunkln(F("<p>Pan Tilt Control - Response</p>"));
  wifly.sendChunkln(F("</h1>"));
  wifly.sendChunkln(F("<p>"));
  wifly.sendChunkln(info);
  wifly.sendChunkln(F(", "));
  
  snprintf_P(buf, sizeof(buf), PSTR("panPos: %d"), panPos);
  wifly.sendChunkln(buf);  
  
  wifly.sendChunkln(F("</p>"));  
  wifly.sendChunkln(F("<form name=\"input\" action=\"/\" method=\"post\">"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"left\" />"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"right\" />"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"pos1\" />"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"pos2\" />"));
  wifly.sendChunkln(F("<input type=\"submit\" name=\"btn\" value=\"pos3\" />"));
  wifly.sendChunkln(F("</form>")); 
  wifly.sendChunkln(F("</html>"));
  wifly.sendChunkln();
}


void send404() {
  wifly.println(F("HTTP/1.1 404 Not Found"));
  wifly.println(F("Content-Type: text/html"));
  wifly.println(F("Transfer-Encoding: chunked"));
  wifly.println();
  wifly.sendChunkln(F("<html><head>"));
  wifly.sendChunkln(F("<title>404 Not Found</title>"));
  wifly.sendChunkln(F("</head><body>"));
  wifly.sendChunkln(F("<h1>Not Found</h1>"));
  wifly.sendChunkln(F("<hr>"));
  wifly.sendChunkln(F("</body></html>"));
  wifly.sendChunkln();
}


//
// RUNNER
//
void setup() { 

  Serial.begin(9600);
  wifly.begin(&Serial);
  servoPan.attach(8);  // attaches the servo on pin 9 to the servo object 
  servoPan.write(50);
  //servoTilt.attach(9);
  
  // Join wifi network
  if (!wifly.isAssociated()) {
    wifly.setSSID(ssid);
    wifly.setPassphrase(password);
    wifly.enableDHCP();
    wifly.save();
    servoPan.write(0);
    
    if (wifly.join()) {
      //servoPan.write(50);
    } else {
      //servoPan.write(100);
    }
  } else {
  }
  
  wifly.setBroadcastInterval(0);  // turn off UPD broadcast
  
  
  wifly.setDeviceID("wifly");
  
  if (wifly.isConnected()) {
    wifly.close();
  }
  
  wifly.setProtocol(WIFLY_PROTOCOL_TCP);
  if (wifly.getPort() != 80) {
    wifly.setPort(80);
    wifly.save();
    wifly.reboot();
    delay(3000);
  }
}
    

 
 
 
void loop() {

  if (wifly.available() > 0) {
    if (wifly.gets(buf, sizeof(buf))) {
      if (strncmp_P(buf, PSTR("GET / "), 6) == 0) {
        // get request
        while (wifly.gets(buf, sizeof(buf)) > 0) {
          // skip rest of request???
        }
        sendIndex();
        
        
      } else if (strncmp_P(buf, PSTR("POST"), 4) == 0) {
        // POST request was received
        
        // Get posted value
        char button[6];
        if (wifly.match(F("btn="))) {
          wifly.gets(button, sizeof(button));
          wifly.flushRx();
          
          if (strcmp(button, "left") == 0) {
            ptz(180, 0, 0);
            sendInfo(button);            
            
          } else if (strcmp(button, "right") == 0) {
            ptz(0, 0, 0);
            sendInfo(button);       
            
          } else if (strcmp(button, "pos1") == 0) {
            ptz(80, 0, 0, 50);
            sendInfo(button);
            
          } else if (strcmp(button, "pos2") == 0) {
            ptz(300, 0, 0, 50);
            sendInfo(button);       
    
          } else if (strcmp(button, "pos3") == 0) {
            ptz(180, 0, 0, 50);
            sendInfo(button);         
            
          } else {
            sendIndex();
          }
        }
        
      } else {
        // unexpected req
        wifly.flushRx();
        send404();
      }
    }
  }
}
      
       

