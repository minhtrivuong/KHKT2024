#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PulseSensorPlayground.h>

const char* ssid = "Tang 2.1";  // Your WiFi SSID
const char* password = "88888888";  // Your WiFi password

ESP8266WebServer server(80);  // Initialize web server on port 80

const int maxValues = 15;        // Max number of values to collect before printing
int bpmValues[maxValues];       // Array to hold the BPM values
int currentIndex = 0;           // Current index in the array
unsigned long previousMillis = 0; // Store the last time the BPM was read
const long intervalRead = 2000;  // Interval for reading BPM (2 seconds)

const int PulseWire = A0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN A0
const int LED = LED_BUILTIN;    // The on-board LED.
int Threshold = 750;
PulseSensorPlayground pulseSensor;

// New variables to count BPM events
int countAbove160 = 0;  // Count for BPM above 160
int countBelow50 = 0;   // Count for BPM below 50

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("...");
  }
  Serial.println("Connected to WiFi!");
  Serial.println(WiFi.localIP());

  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED);       // Automatically blink the LED with each heartbeat.
  pulseSensor.setThreshold(Threshold);

  // Double-check the "pulseSensor" object was created and is detecting a signal.
  if (pulseSensor.begin()) {
    Serial.println("PulseSensor initialized successfully!");
  }

  // Set up routes for the web server
  server.on("/", handleRoot);  // Call handleRoot when accessing "/"
  server.on("/download", handleDownload);  // Call handleDownload when accessing "/download"
  server.begin();  // Start the server
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient(); 
  if (pulseSensor.sawStartOfBeat()) {
    int currentBPM = pulseSensor.getBeatsPerMinute();
    Serial.print("BPM: ");
    Serial.println(currentBPM);

   
    bpmValues[currentIndex] = currentBPM;
    currentIndex++;

    // Check if we have collected enough values to print
    if (currentIndex >= maxValues) {
      printBPMValues();  // Print the stored BPM values
      currentIndex = 0;  // Reset index for next collection
    }

    // Count BPM events
    if (currentBPM > 160) {
      countAbove160++;
    } else if (currentBPM < 50) {
      countBelow50++;
    }
  }

  // Time-based reading (every 2 seconds)
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervalRead) {
    previousMillis = currentMillis; 
  }
}

// Function to handle root request
void handleRoot() {
  // Get the latest BPM value to display
  int currentBPM = pulseSensor.getBeatsPerMinute();
  
  // Create the HTML response
  String html = "<html><head><title>heart</title></head><body>";  // Display current BPM value
  html += "<a href=\"/download\">Download BPM Values</a>"; 

  html += "<br/><a href=\"/\">Refresh</a>";  // Link to refresh the page
  html += "</body></html>";
  
  server.send(200, "text/html", html);  // Return HTML data
}

// Function to handle download request
void handleDownload() {
  String dataToSend = "bien dong nhip tim trong 15 nhip:\n"; // Initialize the data string with a header
  dataToSend += "\nCounts:\n";
  dataToSend += "BPM above 160: " + String(countAbove160) + "\n";  // Add count for BPM above 160
  dataToSend += "BPM below 50: " + String(countBelow50) + "\n";    // Add count for BPM below 50
   
   dataToSend += "nhip tim trung binh la:";
   float averageBPM = calculateAverageBPM();
  dataToSend += "\nAverage BPM: " + String(averageBPM) + "\n";
  // Send the data as a text file
  server.sendHeader("Content-Disposition", "attachment; filename=bpm_values.txt");  // Set header for download
  server.send(200, "text/plain", dataToSend);  // Send BPM values as plain text
}
float calculateAverageBPM() {
  int sum = 0;
  for (int i = 0; i < maxValues; i++) {
    sum += bpmValues[i];
  }
  return (float)sum / maxValues;
}
// Function to print BPM values
void printBPMValues() {
  
  for (int i = 0; i < maxValues; i++) {
    Serial.println(bpmValues[i]);  // Print each value in the array
  }
} 