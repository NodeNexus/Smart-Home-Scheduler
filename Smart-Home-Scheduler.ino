// ----------------------------------------------------
// Smart Home Scheduler: ESP32 Code
// Microcontroller: ESP32/ESP8266
// ----------------------------------------------------

#include <WiFi.h>
#include <WebServer.h>
#include <TimeLib.h> // For time management

// Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const int RELAY_PIN = 27; // GPIO pin for the appliance relay

// Scheduling variables (Defaults to ON 7:00-8:00 and 18:00-22:00)
int onHour1 = 7;
int offHour1 = 8;
int onHour2 = 18;
int offHour2 = 22;

// Web Server Setup
WebServer server(80);

// HTML content function
String getPage() {
  // Simple HTML/CSS for the local web interface
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart Scheduler</title>
<style>
body { font-family: Arial, sans-serif; background: #f4f4f9; color: #333; margin: 0; padding: 20px;}
.container { max-width: 600px; margin: auto; background: #fff; padding: 30px; border-radius: 8px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
h1 { color: #007bff; text-align: center; margin-bottom: 20px; }
.status { padding: 10px; border-radius: 4px; margin-bottom: 20px; text-align: center; font-size: 1.2em; font-weight: bold; }
.status.on { background-color: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
.status.off { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
form { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
label { font-weight: bold; }
input[type="number"] { width: 100%; padding: 10px; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }
button { grid-column: 1 / 3; padding: 12px; background-color: #28a745; color: white; border: none; border-radius: 4px; cursor: pointer; font-size: 1em; }
button:hover { background-color: #218838; }
</style>
<script>
function validateForm() {
    var on1 = parseInt(document.getElementById('on1').value);
    var off1 = parseInt(document.getElementById('off1').value);
    var on2 = parseInt(document.getElementById('on2').value);
    var off2 = parseInt(document.getElementById('off2').value);

    if (on1 < 0 || on1 > 23 || off1 < 0 || off1 > 23 || on2 < 0 || on2 > 23 || off2 < 0 || off2 > 23) {
        alert("Hours must be between 0 and 23.");
        return false;
    }
    if (off1 <= on1) {
        alert("OFF time 1 must be after ON time 1.");
        return false;
    }
    if (off2 <= on2) {
        alert("OFF time 2 must be after ON time 2.");
        return false;
    }
    return true;
}
</script>
</head>
<body>
<div class="container">
<h1>Home Appliance Scheduler</h1>
<div class="status %s">Appliance Status: %s</div>
<p>Current Time (Simulated): %02d:%02d:%02d</p>

<h2>Set Schedule (24hr format)</h2>
<form action="/set" method="POST" onsubmit="return validateForm()">
    <label for="on1">ON Time 1 (Hour):</label>
    <input type="number" id="on1" name="on1" min="0" max="23" required value="%d">

    <label for="off1">OFF Time 1 (Hour):</label>
    <input type="number" id="off1" name="off1" min="0" max="23" required value="%d">

    <label for="on2">ON Time 2 (Hour):</label>
    <input type="number" id="on2" name="on2" min="0" max="23" required value="%d">

    <label for="off2">OFF Time 2 (Hour):</label>
    <input type="number" id="off2" name="off2" min="0" max="23" required value="%d">
    
    <button type="submit">Update Schedule</button>
</form>
<p style="margin-top: 20px; font-size: 0.9em; color: #666;">Current Schedule: ON %02d:00 to %02d:00 and ON %02d:00 to %02d:00</p>
</div>
</body>
</html>
)rawliteral";

  // Determine current relay status for the status box
  String status_class = (digitalRead(RELAY_PIN) == HIGH) ? "on" : "off";
  String status_text = (digitalRead(RELAY_PIN) == HIGH) ? "ON" : "OFF";
  
  // Get simulated time
  time_t currentTime = now();
  
  // Format the HTML content with current data
  return String::format(
    html.c_str(),
    status_class.c_str(),
    status_text.c_str(),
    hour(currentTime),
    minute(currentTime),
    second(currentTime),
    onHour1, offHour1, onHour2, offHour2,
    onHour1, offHour1, onHour2, offHour2
  );
}

// Handler for the root page ("/")
void handleRoot() {
  server.send(200, "text/html", getPage());
}

// Handler for updating the schedule
void handleSet() {
  if (server.hasArg("on1") && server.hasArg("off1") && server.hasArg("on2") && server.hasArg("off2")) {
    onHour1 = server.arg("on1").toInt();
    offHour1 = server.arg("off1").toInt();
    onHour2 = server.arg("on2").toInt();
    offHour2 = server.arg("off2").toInt();
    // Redirect back to the main page
    server.sendHeader("Location", "/");
    server.send(303);
    Serial.printf("Schedule Updated: [%d-%d] and [%d-%d]\n", onHour1, offHour1, onHour2, offHour2);
  } else {
    server.send(400, "text/plain", "Bad Request: Missing parameters");
  }
}

// Core scheduling logic
void checkSchedule() {
  time_t currentTime = now();
  int currentHour = hour(currentTime);
  
  bool shouldBeOn = false;

  // Check Schedule 1
  if (currentHour >= onHour1 && currentHour < offHour1) {
    shouldBeOn = true;
  }
  
  // Check Schedule 2
  if (currentHour >= onHour2 && currentHour < offHour2) {
    shouldBeOn = true;
  }
  
  // Update relay state (Assuming HIGH = ON for the appliance)
  if (shouldBeOn) {
    if (digitalRead(RELAY_PIN) == LOW) {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Appliance ON by schedule.");
    }
  } else {
    if (digitalRead(RELAY_PIN) == HIGH) {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Appliance OFF by schedule.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Start OFF

  // --- WiFi Connection ---
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- Time Synchronization (NTP for accurate time) ---
  configTime(3600, 3600, "pool.ntp.org"); // Adjust for your timezone (e.g., 3600 for +1 hr, 3600 for DST)
  while (!timeStatus()) {
    Serial.print("Waiting for NTP sync...");
    delay(500);
  }
  Serial.println("Time synchronized.");

  // --- Web Server Initialization ---
  server.on("/", HTTP_GET, handleRoot);
  server.on("/set", HTTP_POST, handleSet);
  server.begin();
  Serial.println("HTTP Server started.");
}

void loop() {
  server.handleClient(); // Handle incoming web requests
  checkSchedule();       // Run the scheduling logic
  delay(1000);           // Check every second
}
