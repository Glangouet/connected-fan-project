/*
 * EspHttpServer.h
 *
 *  Created on: 14 mai 2018
 *      Author: Tala
 */

#ifndef SRC_ESPHTTPSERVER_H_
#define SRC_ESPHTTPSERVER_H_

#include <ESP8266WebServer.h>
#include <ApiCommandServer.h>
#include <ApiConfigServer.h>
#include <ApiGpioServer.h>
#include <ApiDiscoveryServer.h>
#include <Options.h>

class EspHttpServer {

public:
	static ESP8266WebServer* server;
private:
	static constexpr char URL_BASE[] = "/esp";

	ApiConfigServer aconfs;
	ApiGpioServer agpios;
	ApiCommandServer acmds;
	ApiDiscoveryServer ads;

public:
	void setup(ESP8266WebServer* server);
	void loop();

private:
	static void displayHtml();

};

#endif /* SRC_ESPHTTPSERVER_H_ */
