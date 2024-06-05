#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

// Debug mode is enabled if not zero
#define DEBUG_MODE 0
// Board name
#define BOARD_NAME "ESP8266"
// HTTP server port
#define HTTP_SERVER_PORT 8000
// HTTP metrics endpoint
#define HTTP_METRICS_ENDPOINT "/metrics"
// Temperature offset in degrees Celsius
#define TEMPERATURE_CORRECTION_OFFSET 0
// Humidity offset in percent
#define HUMIDITY_CORRECTION_OFFSET 0
// How long to cache the sensor results, in milliseconds
#define READ_INTERVAL 5000
// How many times to try to read the sensor before returning an error
#define READ_TRY_COUNT 5
// WiFi SSID (required)
#define WIFI_SSID "That Guy"
// WiFi password (required)
#define WIFI_PASSWORD "Thisismysuperlongpassword1"

enum LogLevel {
    DEBUG,
    INFO,
    ERROR,
};

void setup_wifi();
void setup_http_server();
void handle_http_home_client();
void handle_http_metrics_client();
void read_sensors(boolean force=false);
bool read_sensor(float (*function)(), float *value);
void log(char const *message, LogLevel level=LogLevel::INFO);

ESP8266WebServer http_server(HTTP_SERVER_PORT);

float temp, fan_speed=30.0f;
uint32_t previous_read_time = 0;
String strArr[2];

// Set your Static IP address
IPAddress local_IP(192, 168, 2, 35);
// Set your Gateway IP address
IPAddress gateway(192, 168, 2, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(1, 1, 1, 1);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional


void setup(void) {
    Serial.begin(115200);
    //setup_dht_sensor();
    setup_wifi();
    setup_http_server();
}

void setup_wifi() {
    char message[128];
    snprintf(message, 128, "Wi-Fi SSID: %s", WIFI_SSID);
    log(message);
    log("Wi-Fi connecting ...");
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA Failed to configure");
    }    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      log("Wi-Fi waiting ...", LogLevel::DEBUG);
      delay(500);
    }
    const IPAddress &ipaddr = WiFi.localIP();
    log("Wi-Fi connected.");
    snprintf(message, 128, "IPv4 address: %d.%d.%d.%d:%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], HTTP_SERVER_PORT);
    log(message);
}

void setup_http_server() {
    http_server.on("/", HTTPMethod::HTTP_GET, handle_http_home_client);
    http_server.on(HTTP_METRICS_ENDPOINT, HTTPMethod::HTTP_GET, handle_http_metrics_client);
    http_server.begin();
    log("HTTP server started.");
    char message[128];
    snprintf(message, 128, "Metrics endpoint: %s", HTTP_METRICS_ENDPOINT);
    log(message);
}

void loop(void) {
    http_server.handleClient();
    String rxString = "";
    while (Serial.available()) {
      //Delay to allow byte to arrive in input buffer.
      delay(2);
      //Read a single character from the buffer.
      char ch = Serial.read();
      //Append that single character to a string.
      rxString+= ch;
    }
    int stringStart = 0;
    int arrayIndex = 0;
    for (int i=0; i < rxString.length(); i++){
      //Get character and check if it's our "special" character.
      if(rxString.charAt(i) == ','){
        //Clear previous values from array.
        strArr[arrayIndex] = "";
        //Save substring into array.
        strArr[arrayIndex] = rxString.substring(stringStart, i);
        //Set new string starting point.
        stringStart = (i+1);
        arrayIndex++;
      }
    }
    //Put values from the array into the variables.
    temp = strArr[0].toFloat();
    fan_speed = strArr[1].toFloat();
}

void handle_http_home_client() {
    char message[128];
    snprintf(message, 128, "ping received");
    log(message);
    static size_t const BUFSIZE = 256;
    static char const *response_template =
        "Prometheus ESP8266 DHT Exporter by HON95.\n"
        "\n"
        "Project: https://github.com/HON95/prometheus-esp8266-dht-exporter\n"
        "\n"
        "Usage: %s\n";
    char response[BUFSIZE];
    snprintf(response, BUFSIZE, response_template, HTTP_METRICS_ENDPOINT);
    http_server.send(200, "text/plain; charset=utf-8", response);
}

void handle_http_metrics_client() {
    static size_t const BUFSIZE = 2048;
    static char const *response_template =
        "# HELP temp Rack Temp\n"
        "# TYPE temp gauge\n"
        "# UNIT temp %%\n"
        "temp %f\n"
        "# HELP fan_speed Fan Speed %%\n"
        "# TYPE fan_speed gauge\n"
        "# UNIT fan_speed %%\n"
        "fan_speed %f\n";

    char response[BUFSIZE];
    snprintf(response, BUFSIZE, response_template, temp, fan_speed);
    log(response);
    http_server.send(200, "text/plain; charset=utf-8", response);
}

void log(char const *message, LogLevel level) {
    if (DEBUG_MODE == 0 && level == LogLevel::DEBUG) {
        return;
    }
    // Will overflow after a while
    float seconds = millis() / 1000.0;
    char str_level[10];
    switch (level) {
        case DEBUG:
            strcpy(str_level, "DEBUG");
            break;
        case INFO:
            strcpy(str_level, "INFO");
            break;
        case ERROR:
            strcpy(str_level, "ERROR");
            break;
        default:
            break;
    }
    char record[200];
    snprintf(record, 200, "[%10.3f] [%-5s] %s", seconds, str_level, message);
    Serial.println(record);
}
