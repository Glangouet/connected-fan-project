/*
 * HttpConfigServer.cpp
 *
 *  Created on: 16 sept. 2017
 *      Author: Deejc
 */

#include <ApiGpioServer.h>
#include <ApiConfigServer.h>

constexpr char ApiGpioServer::URL_SHOW[];
constexpr char ApiGpioServer::URL_SET_MODE[];
constexpr char ApiGpioServer::URL_SET_STATE[];
constexpr char ApiGpioServer::URL_SET_TOGGLE[];
constexpr char ApiGpioServer::URL_SET_PWM[];

constexpr char ApiGpioServer::URL_SET_TIME[];

constexpr char ApiGpioServer::VAR_GPIO[];
constexpr char ApiGpioServer::VAR_STATE[];
constexpr char ApiGpioServer::VAR_PWM[];
constexpr char ApiGpioServer::VAR_MODE[];
constexpr char ApiGpioServer::VAR_TIME[];

int32_t ApiGpioServer::TIMED[];
int32_t ApiGpioServer::ELAPSED[];

ESP8266WebServer* ApiGpioServer::server;
/**
 * Used in the setup function to start the ApiGpioServer class
 */
void ApiGpioServer::setup(ESP8266WebServer* server) {
	server->on(URL_SHOW, showConfig);
	server->on(URL_SET_MODE, setMode);
	server->on(URL_SET_STATE, setState);
	server->on(URL_SET_TOGGLE, setToggle);
	server->on(URL_SET_PWM, setPwm);
	server->on(URL_SET_TIME, setTimedEvent);
	this->server = server;
	initGpios();
	elapsed = millis();
}
/**
 * Display configuration in JSON format
 *
 * 	- time	: amount of time remaining in seconds or -1
 * 	- state	: 1 for HIGH, 0 for LOW
 *
 */
void ApiGpioServer::showConfig() {
	char content[560] = "";
	strcpy(content, "{ \"gpios\" : { \"A0\" : { \"state\" : ");
	sprintf(content, "%s%i", content, analogRead(A0));
	strcat(content, ", \"time\" : 0");
	strcat(content, " } , \"D0\" : { \"state\" : ");
	getGpioState(D0, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(0));
	strcat(content, " } , \"D1\" : { \"state\" : ");
	getGpioState(D1, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(1));
	strcat(content, " } , \"D2\" : { \"state\" : ");
	getGpioState(D2, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(2));
	strcat(content, " } , \"D3\" : { \"state\" : ");
	getGpioState(D3, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(3));
	strcat(content, " } , \"D4\" : { \"state\" : ");
	getGpioState(D4, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(4));
	strcat(content, " } , \"D5\" : { \"state\" : ");
	getGpioState(D5, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(5));
	strcat(content, " } , \"D6\" : { \"state\" : ");
	getGpioState(D6, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(6));
	strcat(content, " } , \"D7\" : { \"state\" : ");
	getGpioState(D7, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(7));
	strcat(content, " } , \"D8\" : { \"state\" : ");
	getGpioState(D8, content);
	strcat(content, ", \"time\" : ");
	sprintf(content, "%s%i", content, getRemainingTime(8));
	strcat(content, " } } }");
	server->send(200, F("application/json"), content);
}
/**
 * Used to get or set the state of a gpio (HIGH / LOW)
 * - 1 for HOGH
 * - 0 for LOW
 *
 * eg.
 * 		- http://ESP_IP/gpio/state?gpio=D3&state=0
 * 				to set gpio D3 to LOW
 *
 * 		- http://ESP_IP/gpio/state?gpio=D3
 * 				to get D3 gpio mode ('0' for LOW or '1' for HIGH)
 *
 */
void ApiGpioServer::setState() {
	if (ApiConfigServer::isDisabled()) {
		server->send(403, F("application/json"),
				F("{ \"message\" : \"module is disabled\" }"));
		return;
	}
	int8_t gpio = -1;
	if (!getGpioFromArgument(gpio)) {
		return;
	}
	if (gpio == A0) {
		server->send(400, F("application/json"),
				F("{ \"message\" : \"A0 cannot be use for digital write\" }"));
		return;
	}
	if (server->hasArg(VAR_STATE)) {
		// Set gpio value
		char strState[5] = "";
		if (!getArguments(VAR_STATE, strState, 1, "(0 - 1)")) {
			return;
		}
		int16_t state = atoi(strState);
		if (state < 0 || state > 1) {
			server->send(400, F("application/json"),
					F(
							"{ \"message\" : \"Value not in range (0 for LOW - 1 for HIGH)\" }"));
			return;
		} else {
			// Check si encore utilisé !
			analogWrite(gpio, 0);
			state == 0 ? GPOC = 1 << gpio : GPOS = 1 << gpio;
		}
		TIMED[getTimedGpioIndex(gpio)] = 0;

	}
	// Send gpio value
	char response[30] = "";
	char state[5] = "";
	getGpioState(gpio, state);
	sprintf(response, "{ \"state\" : %s }", state);
	server->send(200, F("application/json"), response);
}
/**
 * Toggle from one state to another
 */
void ApiGpioServer::setToggle() {
	if (ApiConfigServer::isDisabled()) {
		server->send(403, F("application/json"),
				F("{ \"message\" : \"module is disabled\" }"));
		return;
	}
	int8_t gpio = -1;
	if (!getGpioFromArgument(gpio)) {
		return;
	}
	if (gpio == A0) {
		server->send(400, F("application/json"),
				F("{ \"message\" : \"A0 cannot be use for digital toggle\" }"));
		return;
	}
	// Set gpio value
	uint8_t state = digitalRead(gpio);
	analogWrite(gpio, 0);
	state == HIGH ? GPOC = 1 << gpio : GPOS = 1 << gpio;
	TIMED[getTimedGpioIndex(gpio)] = 0;
	// Send gpio value
	char response[30] = "";
	sprintf(response, "{ \"state\" : %i } ", state == HIGH ? 0 : 1);
	server->send(200, F("application/json"), response);
}
/**
 * Used to perform analogWrite on GPIO (PWM)
 */
void ApiGpioServer::setPwm() {
	if (ApiConfigServer::isDisabled()) {
		server->send(403, F("application/json"),
				F("{ \"message\" : \"module is disabled\" }"));
		return;
	}
	int8_t gpio = -1;
	if (!getGpioFromArgument(gpio)) {
		return;
	}
	if (gpio == A0) {
		server->send(400, F("application/json"),
				F("{ \"message\" : \"A0 cannot be use for PWM\" }"));
		return;
	}
	// Set gpio value
	char strPwm[5] = "";
	if (!getArguments(VAR_PWM, strPwm, 4, "(0 - 1023)")) {
		return;
	}
	int16_t pwm = atoi(strPwm);
	if (pwm < 0 || pwm > 1023) {
		server->send(400, F("application/json"),
				F("{ \"message\" : \"Value not in range (0 - 1023)\" }"));
		return;
	}
	analogWrite(gpio, pwm);
	TIMED[getTimedGpioIndex(gpio)] = 0;
	server->send(200, F("application/json"), F("{ \"message\" : \"ok\" }"));
}
/**
 * Used to get or set the mode of a gpio (INPUT / OUTPUT)
 *
 * - 'i' for INPUT
 * - 'o' for OUTPUT
 *
 * eg.
 * 		- http://ESP_IP/gpio/mode?gpio=D3&mode=o
 * 				to set gpio D3 to OUTPUT
 *
 * 		- http://ESP_IP/gpio/mode?gpio=D3
 * 				to get D3 gpio mode ('o' or 'i')
 *
 */
void ApiGpioServer::setMode() {
	if (ApiConfigServer::isDisabled()) {
		server->send(403, F("application/json"),
				F("{ \"message\" : \"module is disabled\" }"));
		return;
	}
	int8_t gpio = -1;
	if (!getGpioFromArgument(gpio)) {
		return;
	}
	char strMode[2] = "";
	if (!getArguments(VAR_MODE, strMode, 2,
			"must be 'i' for INPUT or 'o' for OUTPUT")) {
		return;
	}
	if (strcmp(strMode, "i") == 0) {
		pinMode(gpio, INPUT);
	} else if (strcmp(strMode, "o") == 0) {
		pinMode(gpio, OUTPUT);
	} else {
		server->send(400, F("application/json"),
				F(
						"{ \"message\" : \"mode must be 'i' for INPUT or 'o' for OUTPUT\" }"));
		return;
	}
	server->send(200, F("application/json"), F("{ \"message\" : \"ok\" }"));
}
/**
 * Used to time an event on specific gpio (event is !state then state)
 *
 * eg.
 * 		- http://ESP_IP/gpio/time?gpio=D3&time=20
 * 				to set gpio D3 to HIGH for 20 seconds (if initialy LOW)
 *
 * 		- http://ESP_IP/gpio/mode?gpio=D3
 * 				to get D3 gpio time ('-1' if none or the remaining time in seconds)
 */
void ApiGpioServer::setTimedEvent() {
	if (ApiConfigServer::isDisabled()) {
		server->send(403, F("application/json"),
				F("{ \"message\" : \"module is disabled\" }"));
		return;
	}
	int8_t gpio = -1;
	if (!getGpioFromArgument(gpio)) {
		return;
	}
	if (gpio == A0) {
		server->send(400, F("application/json"),
				F(
						"{ \"message\" : \"A0 cannot be use for timed digital write\" }"));
		return;
	}
	if (!server->hasArg(VAR_TIME)) {
		// Send gpio time
		gpio = getTimedGpioIndex(gpio);
		uint32_t time = getRemainingTime(gpio);
		char response[30] = "";
		char timeStr[6] = "";
		itoa(time, timeStr, 10);
		sprintf(response, "{ \"%s\" : { \"time\" : %s } }",
				server->arg(VAR_GPIO).c_str(), timeStr);
		server->send(200, F("application/json"), response);
	}
	char strTime[6] = "";
	if (!getArguments(VAR_TIME, strTime, 6,
			"must be an integer between 1 - 65535")) {
		return;
	}
	uint16_t time = atoi(strTime);
	if (time < 1) {
		server->send(400, F("application/json"),
				F(
						"{ \"message\" : \"time must be an integer between 1 - 65535\" }"));
		return;
	}
	digitalRead(gpio) == HIGH ? GPOC = 1 << gpio : GPOS = 1 << gpio;
	gpio = getTimedGpioIndex(gpio);
	TIMED[gpio] = time;
	ELAPSED[gpio] = millis();
	server->send(200, F("application/json"), F("{ \"message\" : \"ok\" }"));
}
/**
 * Call this method in the loop function
 */
void ApiGpioServer::loop() {
	if (millis() - this->elapsed > 1000) {
		this->elapsed = millis();
		for (uint8_t i = 0; i < 9; i++) {
			if (TIMED[i] > 0 && getRemainingTime(i) == 0) {
				TIMED[i] = 0;
				uint8_t gpio = -1;
				if (i == 0) {
					gpio = D0;
				} else if (i == 1) {
					gpio = D1;
				} else if (i == 2) {
					gpio = D2;
				} else if (i == 3) {
					gpio = D3;
				} else if (i == 4) {
					gpio = D4;
				} else if (i == 5) {
					gpio = D5;
				} else if (i == 6) {
					gpio = D6;
				} else if (i == 7) {
					gpio = D7;
				} else if (i == 8) {
					gpio = D8;
				} else if (i == 9) {
					gpio = A0;
				}
				digitalRead(gpio) == HIGH ? GPOC = 1 << gpio : GPOS = 1 << gpio;
			}
		}
	}
}
/**
 * Helper to get the arguments.
 */
bool ApiGpioServer::getArguments(const char name[], char value[],
		uint8_t maxLen, const char errorMessage[]) {
	char content[130] = "";
	strcpy(content, "{ \"message\" : \"argument ");
	strcat(content, name);
	if (server->hasArg(name)) {
		int8_t len = strlen(server->arg(name).c_str());
		if (len > maxLen) {
			strcat(content, " values is too big\" }");
			strcat(content, errorMessage);
			server->send(400, F("application/json"), content);
			return false;
		} else if (len == 0) {
			// Error
			strcat(content, " has no value\" }");
			server->send(400, F("application/json"), content);
			return false;
		}
		strcpy(value, server->arg(name).c_str());
		return true;
	}
	strcat(content, " is missing\" }");
	server->send(400, F("application/json"), content);
	return false;
}
/**
 *	Return the gpio numnber from the string argument
 */
bool ApiGpioServer::getGpioFromArgument(int8_t &gpio) {
	char strGpio[3] = "";
	if (!getArguments(VAR_GPIO, strGpio, 2, "(eg. D2, D3, ...)")) {
		return false;
	}
	gpio = getGpioFromString(strGpio);
	if (gpio == -1) {
		server->send(400, F("application/json"),
				F("{ \"message\" : \"bad gpio number\" }"));
		return false;
	}
	return true;
}
/**
 * Return the GPIO state as a string
 */
void ApiGpioServer::getGpioState(uint8_t gpio, char content[]) {
	if (gpio == A0) {
		sprintf(content, "%s%i", content, analogRead(A0));
	} else {
		uint16_t len = strlen(content);
		if (digitalRead(gpio) == HIGH) {
			content[len++] = '1';
		} else {
			content[len++] = '0';
		}
		content[len] = '\0';
	}
}
/**
 * Return the gpio number from the string representation
 */
int8_t ApiGpioServer::getGpioFromString(char strGpio[]) {
	if (strcmp(strGpio, "A0") == 0) {
		return A0;
	} else if (strcmp(strGpio, "D0") == 0) {
		return D0;
	} else if (strcmp(strGpio, "D1") == 0) {
		return D1;
	} else if (strcmp(strGpio, "D2") == 0) {
		return D2;
	} else if (strcmp(strGpio, "D3") == 0) {
		return D3;
	} else if (strcmp(strGpio, "D4") == 0) {
		return D4;
	} else if (strcmp(strGpio, "D5") == 0) {
		return D5;
	} else if (strcmp(strGpio, "D6") == 0) {
		return D6;
	} else if (strcmp(strGpio, "D7") == 0) {
		return D7;
	} else if (strcmp(strGpio, "D8") == 0) {
		return D8;
	}
	return -1;
}
/**
 * Initialize gpios to output
 */
void ApiGpioServer::initGpios() {
	pinMode(A0, OUTPUT);
	pinMode(D0, OUTPUT);
	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);
	pinMode(D5, OUTPUT);
	pinMode(D6, OUTPUT);
	pinMode(D7, OUTPUT);
	pinMode(D8, OUTPUT);
	for (uint8_t i = 0; i < 10; i++) {
		this->TIMED[i] = 0;
	}
}
/**
 * Convert gpio names into array TIMED indexes
 */
int8_t ApiGpioServer::getTimedGpioIndex(uint8_t gpio) {
	if (gpio == D0) {
		return 0;
	} else if (gpio == D1) {
		return 1;
	} else if (gpio == D2) {
		return 2;
	} else if (gpio == D3) {
		return 3;
	} else if (gpio == D4) {
		return 4;
	} else if (gpio == D5) {
		return 5;
	} else if (gpio == D6) {
		return 6;
	} else if (gpio == D7) {
		return 7;
	} else if (gpio == D8) {
		return 8;
	} else if (gpio == A0) {
		return 9;
	}
	return -1;
}
/**
 * Return the remaining time, in second, before gpio state change
 */
uint32_t ApiGpioServer::getRemainingTime(uint8_t gpio) {
	if (TIMED[gpio] > 0) {
		uint32_t since = millis() - ELAPSED[gpio];
		uint32_t then = TIMED[gpio] * 1000;
		if (since > then) {
			return 0;
		}
		return (then - since) / 1000;
	}
	return 0;
}
