#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EspHttpServer.h>

#define SERIAL_DEBUG

ESP8266WebServer server(80);
EspHttpServer ehs;


// WiFi variables
//const char* www_username = "admin";
//const char* www_password = "password";
//
//void handleRoot() {
//	if (!server.authenticate(www_username, www_password)) {
//		return server.requestAuthentication();
//	}
//	server.send(200, F("text/plain"), F("Login OK"));
//}

void setup() {
	ehs.setup(&server);
}

void loop() {
	ehs.loop();
}
