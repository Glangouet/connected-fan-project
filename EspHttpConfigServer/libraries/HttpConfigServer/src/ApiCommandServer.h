/*
 * ApiCommandServer.h
 *
 *  Created on: 16 sept. 2017
 *      Author: Deejc
 */

#ifndef SRC_APICOMMANDSERVER_H_
#define SRC_APICOMMANDSERVER_H_

#include <ESP8266WebServer.h>
#include <Options.h>

class ApiCommandServer {

private:
	static constexpr char URL_LOCATE[] = "/api/cmd/locate";
	static constexpr char URL_REBOOT[] = "/api/cmd/reboot";
	static constexpr char URL_DISABLE[] = "/api/cmd/disable";
	static constexpr char URL_ENABLE[] = "/api/cmd/enable";

	static ESP8266WebServer* server;
	static bool isLocate, ledState;
	static uint32_t BLINK_TIMER;
	static uint32_t BLINK_DURATION;
	static uint32_t elapsed_timer;
	static uint32_t elapsed_duration;

public:
	/**
	 * Used in the setup function to start the ApiCommandServer class
	 */
	void setup(ESP8266WebServer* server);
	/**
	 * Call this method in the 'loop' function
	 */
	void loop();

private:

	static void locate();
	static void reboot();
	static void disable();
	static void enable();

};

#endif /* SRC_APICOMMANDSERVER_H_ */
