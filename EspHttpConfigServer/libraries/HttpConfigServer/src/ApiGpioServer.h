/*
 * HttpGpioServer.h
 *
 *  Created on: 11 ao�t 2017
 *      Author: jcf
 */

#ifndef SRC_APIGPIOSERVER_H_
#define SRC_APIGPIOSERVER_H_

#include <ESP8266WebServer.h>
#include <Options.h>

class ApiGpioServer {

private:

	static constexpr char URL_SHOW[] = "/api/gpio";
	static constexpr char URL_SET_MODE[] = "/api/gpio/mode";
	static constexpr char URL_SET_STATE[] = "/api/gpio/state";
	static constexpr char URL_SET_TOGGLE[] = "/api/gpio/toggle";
	static constexpr char URL_SET_PWM[] = "/api/gpio/pwm";
	static constexpr char URL_SET_TIME[] = "/api/gpio/time";

	static constexpr char VAR_GPIO[] = "gpio";
	static constexpr char VAR_STATE[] = "state";
	static constexpr char VAR_PWM[] = "pwm";
	static constexpr char VAR_MODE[] = "mode";
	static constexpr char VAR_TIME[] = "time";

	static int32_t TIMED[10];
	static int32_t ELAPSED[10];
	uint64_t elapsed;

	static ESP8266WebServer* server;

public:
	/**
	 * Used in the setup function to start the ApiGpioServer class
	 */
	void setup(ESP8266WebServer* server);
	/**
	 * Call this method in the 'loop' function
	 */
	void loop();

private:

	static void showConfig();
	static void setMode();
	static void setState();
	static void setToggle();
	static void setPwm();
	static void setTimedEvent();
	static void getGpioState(uint8_t gpio, char content[]);
	static bool getArguments(const char name[], char value[], uint8_t maxLen, const char errorMessage[]);
	static bool getGpioFromArgument(int8_t &gpio);
	static int8_t getGpioFromString(char strGpio[]);
	void initGpios();
	static int8_t getTimedGpioIndex(uint8_t gpio);
	static uint32_t getRemainingTime(uint8_t gpio);
};

#endif /* SRC_APIGPIOSERVER_H_ */
