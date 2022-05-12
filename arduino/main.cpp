#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define GET_CHIPID() (ESP.getChipId())

#include <FS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <AutoConnect.h>
#include <SmartButton.h>

#include <AHT10.h>
#include <Wire.h>

uint8_t readStatus = 0;

AHT10 myAHT10(AHT10_ADDRESS_0X38);

#define RGB_RED (1)   // R
#define RGB_GREEN (3) // G
#define RGB_BLUE (0)  // B
int ledState_red = 0;
int ledState_green = 0;
int ledState_blue = 0;

#define BTN_L (13)
#define BTN_T (16)
#define BTN_R (14)
#define BTN_B (12)
#define BTN_C (2)

SmartButton buttons[] = {
  SmartButton("button-left", BTN_L),
  SmartButton("button-top", BTN_T),
  SmartButton("button-center", BTN_C),
  SmartButton("button-right", BTN_R),
  SmartButton("button-bottom", BTN_B)
};

const int SHORT_PRESS_TIME = 500; // 500 milliseconds

#define PARAM_FILE "/param.json"
#define AUX_SETTING_URI "/mqtt_setting"
#define AUX_SAVE_URI "/mqtt_save"
#define AUX_TEST_URI "/test"

typedef ESP8266WebServer WiFiWebServer;
AutoConnect portal;
AutoConnectConfig config;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//Definition of functions:
void callback(String topic, byte *message, int length);
void redirect(String uri);
void handleButton(SmartButton *button, byte normalState, int shortPressTime);

//MQTT Settings
String mqttServer = "192.168.178.99";
String mqttPort = "1884";
String mqttUser = "admin";
String mqttPW = "admin";
String mqttDeviceName = "SmartLightSwitch-" + String(GET_CHIPID(), HEX);

// Declare AutoConnectElements for the page asf /mqtt_setting
ACStyle(style, "label+input,label+select{position:sticky;left:120px;width:230px!important;box-sizing:border-box;}");
ACText(header, "<h2>MQTT Settings</h2>", "text-align:center;color:#2f4f4f;padding:10px;");
ACInput(inMqttserver, mqttServer.c_str(), "MQTT-Server", "", "e.g. 192.168.172.99");
ACInput(inMqttport, mqttPort.c_str(), "MQTT Port", "", "e.g. 1883 or 1884");
ACInput(inMqttuser, mqttUser.c_str(), "MQTT User", "", "default: admin");
ACInput(inMqttpw, mqttPW.c_str(), "MQTT Password", "", "default: admin");
ACInput(inMqttDeviceName, mqttDeviceName.c_str(), "MQTT Device Name", "", (String("default: ") + mqttDeviceName).c_str());
ACText(mqttState, "MQTT-State: none", "");
ACSubmit(save, "Save", AUX_SAVE_URI);
ACSubmit(discard, "Discard", "/");
ACElement(newline, "<hr>");
// Declare the custom Web page as /mqtt_setting and contains the AutoConnectElements
AutoConnectAux mqtt_setting(AUX_SETTING_URI, "MQTT Settings", true, {style, header, newline, inMqttserver, inMqttport, newline, inMqttuser, inMqttpw, inMqttDeviceName, newline, mqttState, newline, save, discard});

// Declare AutoConnectElements for the page as /mqtt_save
ACText(caption2, "<h4>Parameters available as:</h4>", "text-align:center;color:#2f4f4f;padding:10px;");
ACText(parameters);
ACSubmit(back2config, "Back", AUX_SETTING_URI);
// Declare the custom Web page as /mqtt_save and contains the AutoConnectElements
AutoConnectAux mqtt_save(AUX_SAVE_URI, "MQTT Settings", false, {caption2, parameters, back2config});

bool mqttConnect()
{
  if (mqttServer.length() <= 0)
  {
    return false;
  }

  mqttClient.setServer(mqttServer.c_str(), atoi(mqttPort.c_str()));
  mqttClient.setCallback(callback);

  if (mqttClient.connect(mqttDeviceName.c_str(), mqttUser.c_str(), mqttPW.c_str()))
  {
    mqttClient.publish((mqttDeviceName + "/ip").c_str(), WiFi.localIP().toString().c_str());
    mqttClient.subscribe((mqttDeviceName + "/led-red").c_str());
    mqttClient.subscribe((mqttDeviceName + "/led-green").c_str());
    mqttClient.subscribe((mqttDeviceName + "/led-blue").c_str());
    mqttState.value = "MQTT-State: <b style=\"color: green;\">Connected</b>";
    return true;
  }
  else
  {
    mqttState.value = "MQTT-State: <b style=\"color: red;\">Disconnected</b>";
  }
  return false;
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte *message, int length)
{

  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)message[i];
  }
  messageTemp += '\n';

  if (topic == (mqttDeviceName + "/led-red").c_str())
  {
    ledState_red = messageTemp.toInt();
  }
  else if (topic == (mqttDeviceName + "/led-green").c_str())
  {
    ledState_green = messageTemp.toInt();
  }
  else if (topic == (mqttDeviceName + "/led-blue").c_str())
  {
    ledState_blue = messageTemp.toInt();
  }
}

String loadParams()
{
  File param = SPIFFS.open(PARAM_FILE, "r");
  if (param)
  {
    if (mqtt_setting.loadElement(param))
    {
      mqttServer = mqtt_setting["inMqttserver"].value;
      mqttServer.trim();
      mqttPort = mqtt_setting["inMqttport"].value;
      mqttPort.trim();
      mqttUser = mqtt_setting["inMqttuser"].value;
      mqttUser.trim();
      mqttPW = mqtt_setting["inMqttpw"].value;
      mqttPW.trim();
      mqttDeviceName = mqtt_setting["inMqttDeviceName"].value;
      mqttDeviceName.trim();
    }
    param.close();
  }
  return String("");
}

// Retreive the value of each element entered by '/mqtt_setting'.
String saveParams(AutoConnectAux &aux, PageArgument &args)
{
  mqttServer = inMqttserver.value;
  mqttServer.trim();

  mqttPort = inMqttport.value;
  mqttPort.trim();

  mqttUser = inMqttuser.value;
  mqttUser.trim();

  mqttPW = inMqttpw.value;
  mqttPW.trim();

  mqttDeviceName = inMqttDeviceName.value;
  mqttDeviceName.trim();

  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // To retrieve the elements of /mqtt_setting, it is necessary to get
  // the AutoConnectAux object of /mqtt_setting.
  File param = SPIFFS.open(PARAM_FILE, "w");
  mqtt_setting.saveElement(param, {"inMqttserver", "inMqttport", "inMqttuser", "inMqttpw", "inMqttDeviceName"});
  param.close();

  // Echo back saved parameters to AutoConnectAux page.
  String echo = "Server: " + mqttServer + "<br>";
  echo += "Port: " + mqttPort + "<br>";
  echo += "User: " + mqttUser + "<br>";
  echo += "Password: " + mqttPW + "<br>";
  parameters.value = echo;
  mqttClient.disconnect();
  return String("");
}

void handleRoot()
{
  redirect("/_ac");
}

void redirect(String uri)
{
  WiFiWebServer &webServer = portal.host();
  webServer.sendHeader("Location", String("http://") + webServer.client().localIP().toString() + uri);
  webServer.send(302, "text/plain", "");
  webServer.client().flush();
  webServer.client().stop();
}

void setup()
{
  Wire.begin(SDA, SCL);

  SPIFFS.begin();
  config.title = "MQTT Smart Light Switch";
  config.apid = "esp-smart-light-switch";
  config.autoReconnect = true;
  config.reconnectInterval = 4;   // Seek interval time is 120[s].
  config.psk = "";
  config.autoReset = true;
  config.portalTimeout = 60000; // It will time out in 60 seconds
  config.retainPortal = true;
  portal.config(config);
  loadParams();
  // Join the custom Web pages and register /mqtt_save handler
  portal.join({mqtt_setting, mqtt_save});
  portal.on(AUX_SAVE_URI, saveParams);

  if (portal.begin())
  {
    ArduinoOTA.begin();
  }
  WiFiWebServer &webServer = portal.host();
  webServer.on("/", handleRoot);

  //Handle LEDs
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  analogWrite(RGB_RED, ledState_red);
  analogWrite(RGB_GREEN, ledState_green);
  analogWrite(RGB_BLUE, ledState_blue);

  //Init Buttons
  pinMode(BTN_L, INPUT_PULLUP);
  pinMode(BTN_T, INPUT_PULLDOWN_16);
  pinMode(BTN_C, INPUT_PULLUP);
  pinMode(BTN_R, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);

  myAHT10.begin();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    //The following things can only be handled if wifi is connected
    ArduinoOTA.handle();
    if (!mqttClient.connected())
    {
      mqttConnect();
    }
    else
    {
      mqttClient.loop();
    }
  }

  // set the LED with the ledState of the variable:
  analogWrite(RGB_RED, ledState_red);
  analogWrite(RGB_GREEN, ledState_green);
  analogWrite(RGB_BLUE, ledState_blue);

  //BTN
  handleButton(&buttons[0], HIGH, SHORT_PRESS_TIME);
  handleButton(&buttons[1], LOW, SHORT_PRESS_TIME);
  handleButton(&buttons[2], HIGH, SHORT_PRESS_TIME * 2);
  handleButton(&buttons[3], HIGH, SHORT_PRESS_TIME);
  handleButton(&buttons[4], HIGH, SHORT_PRESS_TIME);

  //*Temperature
  if(millis() % 5000 == 0){
    mqttClient.publish((mqttDeviceName + "/temperature").c_str(), String(myAHT10.readTemperature()).c_str());
    mqttClient.publish((mqttDeviceName + "/humidity").c_str(), String(myAHT10.readHumidity()).c_str());
  }
  portal.handleClient();
}

void handleButton(SmartButton *btn, byte normalState, int shortPressTime)
{
  int currentState = digitalRead(btn->pin);

  if (btn->lastState == normalState && currentState != normalState)
  { // button is pressed
    btn->pressedTime = millis();
    btn->isPressing = true;
    btn->isLongDetected = false;
  }
  else if (btn->lastState != normalState && currentState == normalState)
  { // button is released
    btn->isPressing = false;
    btn->releasedTime = millis();

    long pressDuration = btn->releasedTime - btn->pressedTime;

    if (pressDuration < shortPressTime && pressDuration > 20)
      mqttClient.publish((mqttDeviceName + "/" + btn->name).c_str(), "S");
  }

  if (btn->isPressing == true && btn->isLongDetected == false)
  {
    long pressDuration = millis() - btn->pressedTime;

    if (pressDuration > shortPressTime)
    {
      mqttClient.publish((mqttDeviceName + "/" + btn->name).c_str(), "L");
      btn->isLongDetected = true;
    }
  }
  // save the the last state
  btn->lastState = currentState;
}
