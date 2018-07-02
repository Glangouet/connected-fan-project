/*
 * ApiCommandServer.cpp
 *
 *  Created on: 16 sept. 2017
 *      Author: Deejc
 */

#include <ApiCommandServer.h>
#include <ApiConfigServer.h>

constexpr char ApiCommandServer::URL_LOCATE[];
constexpr char ApiCommandServer::URL_REBOOT[];
constexpr char ApiCommandServer::URL_DISABLE[];
constexpr char ApiCommandServer::URL_ENABLE[];

bool ApiCommandServer::isLocate = false;
bool ApiCommandServer::ledState = false;
uint32_t ApiCommandServer::BLINK_TIMER = 1000;
uint32_t ApiCommandServer::BLINK_DURATION = 120000;
uint32_t ApiCommandServer::elapsed_timer = 0;
uint32_t ApiCommandServer::elapsed_duration = 0;

ESP8266WebServer* ApiCommandServer::server;

void ApiCommandServer::setup(ESP8266WebServer* server) {
	this->server = server;
	server->on(URL_LOCATE, locate);
	server->on(URL_REBOOT, reboot);
	server->on(URL_DISABLE, disable);
	server->on(URL_ENABLE, enable);
}
/**
 * Call this in loop function
 */
void ApiCommandServer::loop() {
	// TODO passer en static les timers !
	if (isLocate) {
		uint32_t since_timer = (millis() - elapsed_timer);
		uint32_t since_duration = (millis() - elapsed_duration);
		if (since_timer >= BLINK_DURATION) {
			digitalWrite(LED_BUILTIN, LOW);
			ledState = LOW;
			isLocate = false;
			return;
		}
		if (since_duration >= BLINK_TIMER) {
			ledState = !ledState;
			ledState ? GPOC = 1 << 2 : GPOS = 1 << 2;
			elapsed_duration = millis();
		}
	}
}
/**
 * Blinks the onboard led of the ESP
 */
void ApiCommandServer::locate() {
	isLocate = true;
	elapsed_timer = millis();
	elapsed_duration = millis();
	server->send(200, F("text/plain"), F(""));
}
/**
 * Used to reboot the module
 */
void ApiCommandServer::reboot() {
	server->send(200, F("text/html"), F(""));
	ESP.restart();
}
/**
 * Used to enable module
 */
void ApiCommandServer::enable(){
	ApiConfigServer::setDisabled(false);
	server->send(200, F("text/plain"), F(""));
}
/**
 * Used to disable module
 */
void ApiCommandServer::disable(){
	ApiConfigServer::setDisabled(true);
	server->send(200, F("text/plain"), F(""));
}
