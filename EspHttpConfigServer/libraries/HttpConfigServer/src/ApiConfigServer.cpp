/*
 * ApiConfigServer.cpp
 *
 *  Created on: 16 sept. 2017
 *      Author: Deejc
 */

#include <ApiConfigServer.h>
#include <WiFiClient.h>
#include <IPAddress.h>

constexpr char ApiConfigServer::URL_SHOW[];
constexpr char ApiConfigServer::URL_APPLY[];
constexpr char ApiConfigServer::URL_SAVE[];
constexpr char ApiConfigServer::URL_RESET[];

constexpr char ApiConfigServer::VAR_NAME[];
constexpr char ApiConfigServer::VAR_STA_SSID[];
constexpr char ApiConfigServer::VAR_STA_PASSWORD[];
constexpr char ApiConfigServer::VAR_AP_SSID[];
constexpr char ApiConfigServer::VAR_AP_PASSWORD[];
constexpr char ApiConfigServer::VAR_AP_POWER[];
constexpr char ApiConfigServer::VAR_IPCONFIG[];
constexpr char ApiConfigServer::VAR_WIFIMODE[];
constexpr char ApiConfigServer::VAR_IPADDR[];
constexpr char ApiConfigServer::VAR_IPMASK[];
constexpr char ApiConfigServer::VAR_IPGW[];
constexpr char ApiConfigServer::VAR_IPDNS[];
constexpr char ApiConfigServer::DEFAULT_AP_PASSWORD[];
constexpr char ApiConfigServer::VAR_APPLY[];
constexpr char ApiConfigServer::VAR_STATUS[];
constexpr char ApiConfigServer::VAR_DISABLED[];

ESP8266WebServer* ApiConfigServer::server;
EepromManager ApiConfigServer::wmm;
bool ApiConfigServer::isStaFailed = false;
/**
 * Used in the setup function to start the ApiConfigServer class
 */
void ApiConfigServer::setup(ESP8266WebServer* server) {
#ifdef SERIAL_DEBUG
	if (!wmm.isEmpty()) {
		Serial.printf("Current used EEPROM space is %i over %i\n", LAST_INDEX,
				EepromManager::MEM_SIZE);
	} else {
		Serial.println(F("EEPROM is empty"));
	}
#endif
	server->on(URL_SHOW, show);
	server->on(URL_SAVE, save);
	server->on(URL_APPLY, apply);
	server->on(URL_RESET, reset);
	this->server = server;
	applyConfig();
}
/**
 * Call this method in the 'loop' function
 */
void ApiConfigServer::loop(){
	// TODO test !
	static uint32_t elapsed = millis();
	if(isStaFailed){
		uint32_t since = (millis() - elapsed) / 1000;
		if(since > 60){
			applyStaConfig();
		}
	}
}
/**
 * Disables the module
 */
void ApiConfigServer::setDisabled(bool isDisabled) {
	wmm.setBool(POS_DISABLED, isDisabled);
}
/**
 * True if module has been disabled
 */
bool ApiConfigServer::isDisabled() {
	return wmm.getBool(POS_DISABLED) == true;
}
/**
 * Append the hostname stored in configuration
 */
void ApiConfigServer::getHostname(char hostname[]) {
	if (wmm.isEmpty()) {
		strcpy(hostname, "");
	} else {
		wmm.appendString(POS_HOSTNAME, LEN_HOSTNAME, hostname);
	}
}
/**
 * Apply configuration from web interface
 */
void ApiConfigServer::apply() {
	server->send(200, F("application/json"),
			F(
					"{ \"message\" : \"configuration applied, wait for unit to reboot...\" }"));
	delay(100);
	applyConfig();
}
/**
 * Used to retrieve from fields and store them in flash
 */
void ApiConfigServer::save() {
	wmm.init();
	char error[200] = "";
	char update[200] = "";
	saveValue(VAR_NAME, POS_HOSTNAME, LEN_HOSTNAME) ?
			sprintf(update, "%s\"%s\", ", update, VAR_NAME) :
			sprintf(error, "%s\"%s\", ", error, VAR_NAME);
	saveValue(VAR_WIFIMODE, POS_WIFI_MODE, LEN_WIFI_MODE) ?
			sprintf(update, "%s\"%s\", ", update, VAR_WIFIMODE) :
			sprintf(error, "%s\"%s\", ", error, VAR_WIFIMODE);
	saveValue(VAR_DISABLED, POS_DISABLED, LEN_DISABLED) ?
			sprintf(update, "%s\"%s\", ", update, VAR_DISABLED) :
			sprintf(error, "%s\"%s\", ", error, VAR_DISABLED);
	if (server->hasArg(VAR_WIFIMODE)) {
		uint8_t wifimode = atoi(server->arg(VAR_WIFIMODE).c_str());
		if (wifimode == VALUE_MODE_CLIENT || wifimode == VALUE_MODE_BOTH) {
			saveValue(VAR_STA_SSID, POS_STA_SSID, LEN_SSID) ?
					sprintf(update, "%s\"%s\", ", update, VAR_STA_SSID) :
					sprintf(error, "%s\"%s\", ", error, VAR_STA_SSID);
			saveValue(VAR_STA_PASSWORD, POS_STA_PASSWORD, LEN_PASSWORD) ?
					sprintf(update, "%s\"%s\", ", update, VAR_STA_PASSWORD) :
					sprintf(error, "%s\"%s\", ", error, VAR_STA_PASSWORD);
			saveValue(VAR_IPCONFIG, POS_IPCONFIG, LEN_IPCONFIG) ?
					sprintf(update, "%s\"%s\", ", update, VAR_IPCONFIG) :
					sprintf(error, "%s\"%s\", ", error, VAR_IPCONFIG);
			uint8_t ipconfig = atoi(server->arg(VAR_IPCONFIG).c_str());
			if (ipconfig == VALUE_IP_STATIC) {
				saveValue(VAR_IPADDR, POS_IP_ADDR, LEN_IP_ADDR) ?
						sprintf(update, "%s\"%s\", ", update, VAR_IPADDR) :
						sprintf(error, "%s\"%s\", ", error, VAR_IPADDR);
				saveValue(VAR_IPMASK, POS_IP_MASK, LEN_IP_ADDR) ?
						sprintf(update, "%s\"%s\", ", update, VAR_IPMASK) :
						sprintf(error, "%s\"%s\", ", error, VAR_IPMASK);
				saveValue(VAR_IPGW, POS_IP_GW, LEN_IP_ADDR) ?
						sprintf(update, "%s\"%s\", ", update, VAR_IPGW) :
						sprintf(error, "%s\"%s\", ", error, VAR_IPGW);
				saveValue(VAR_IPDNS, POS_IP_GW, LEN_IP_ADDR) ?
						sprintf(update, "%s\"%s\", ", update, VAR_IPDNS) :
						sprintf(error, "%s\"%s\", ", error, VAR_IPDNS);
			}
		}
		if (wifimode == VALUE_MODE_AP || wifimode == VALUE_MODE_BOTH) {
			saveValue(VAR_AP_SSID, POS_AP_SSID, LEN_SSID) ?
					sprintf(update, "%s\"%s\", ", update, VAR_AP_SSID) :
					sprintf(error, "%s\"%s\", ", error, VAR_AP_SSID);
			saveValue(VAR_AP_PASSWORD, POS_AP_PASSWORD, LEN_PASSWORD) ?
					sprintf(update, "%s\"%s\", ", update, VAR_AP_PASSWORD) :
					sprintf(error, "%s\"%s\", ", error, VAR_AP_PASSWORD);
			saveValue(VAR_AP_POWER, POS_AP_POWER, LEN_AP_POWER) ?
					sprintf(update, "%s\"%s\", ", update, VAR_AP_POWER) :
					sprintf(error, "%s\"%s\", ", error, VAR_AP_POWER);
		}
	}
	char response[450] = "{ \"configuration\" : { \"updated\" : [ ";
	if (strlen(update) > 2) {
		// Removing last ', ' from string;
		update[strlen(update) - 2] = '\0';
		strcat(response, update);
	}
	strcat(response, " ], \"error\" : [ ");
	if (strlen(error) > 2) {
		// Removing last ', ' from string;
		error[strlen(error) - 2] = '\0';
		strcat(response, error);
	}
	strcat(response, " ] ");
	strcat(response, ", \"to_apply\" : ");
	strcat(response, server->hasArg(VAR_APPLY) ? "true" : "false");
	strcat(response, " } }");
	server->send(200, F("application/json"), response);
	if (server->hasArg(VAR_APPLY)) {
		delay(100);
		applyConfig();
	}
}
/**
 * Return the current configuration in Json format
 */
void ApiConfigServer::toJson(char* json) {
	strcpy(json, "{ \"configuration\" : ");
	if (!wmm.isEmpty()) {
		strcat(json, "{ \"hostname\" : \"");
		wmm.appendString(POS_HOSTNAME, LEN_HOSTNAME, json);
		strcat(json, "\", \"mac\" : \"");
		sprintf(json, "%s%s", json, WiFi.macAddress().c_str());
		strcat(json, "\", \"channel\" : ");
		sprintf(json, "%s%i", json, WiFi.channel());
		strcat(json, ", \"uptime\" : ");
		sprintf(json, "%s%lu", json, millis() / 1000);
		strcat(json, ", \"disabled\" : ");
		sprintf(json, "%s%s", json,
				wmm.getBool(POS_DISABLED) ? "true" : "false");
		uint8_t mode = wmm.getUint8(POS_WIFI_MODE);
		if (mode == 2 || mode == 3) {
			strcat(json, ", \"ap\" : { ");
			strcat(json, "\"ssid\" : \"");
			wmm.appendString(POS_AP_SSID, LEN_SSID, json);
			strcat(json, "\", \"password\" : \"");
			wmm.appendString(POS_AP_PASSWORD, LEN_PASSWORD, json);
			strcat(json, "\", \"power\" : \"");
			sprintf(json, "%s%i", json, wmm.getUint8(POS_AP_POWER));
			strcat(json, "\", \"ip\" : \"");
			strcat(json, WiFi.softAPIP().toString().c_str());
			strcat(json, "\" }");
		}
		if (mode == 1 || mode == 3) {
			strcat(json, ", \"sta\" : { ");
			strcat(json, "\"mode\" : \"");
			strcat(json,
					wmm.getUint8(POS_IPCONFIG) == VALUE_IP_DHCP ?
							"dhcp" : "static");
			strcat(json, "\", \"ssid\" : \"");
			wmm.appendString(POS_STA_SSID, LEN_SSID, json);
			strcat(json, "\", \"rssi\" : ");
			sprintf(json, "%s%i", json, WiFi.RSSI());
			strcat(json, ", \"bssid\" : \"");
			sprintf(json, "%s%s", json, WiFi.BSSIDstr().c_str());
			if (wmm.getUint8(POS_IPCONFIG) == VALUE_IP_STATIC) {
				strcat(json, "\", \"ip\" : \"");
				wmm.appendString(POS_IP_ADDR, LEN_IP_ADDR, json);
				strcat(json, "\", \"netmask\" : \"");
				wmm.appendString(POS_IP_MASK, LEN_IP_ADDR, json);
				strcat(json, "\", \"gateway\" : \"");
				wmm.appendString(POS_IP_GW, LEN_IP_ADDR, json);
				strcat(json, "\", \"dns\" : \"");
				wmm.appendString(POS_IP_DNS, LEN_IP_ADDR, json);

			}
			strcat(json, "\" } ");
		}
		strcat(json, " }");
	} else {
		strcat(json, " false ");
	}
	strcat(json, " }");
}
/**
 * Display hardware configuration
 */
void ApiConfigServer::show() {
	char json[700] = "";
	toJson(json);
	server->send(200, F("application/json"), json);
}
/**
 * Used to reset configuration
 */
void ApiConfigServer::reset() {
	wmm.erase();
	server->send(200, F("application/json"),
			F(
					"{ \"message\" : \"configuration reset, going down for reboot\" }"));
	delay(200);
	ESP.restart();
}
/**
 * Used to retrieve WiFi configuration from flash and apply it
 */
void ApiConfigServer::applyConfig() {
	WiFi.disconnect(true);
	if (EepromManager::isEmpty()) {
		defaultWiFiConfig();
#ifdef SERIAL_DEBUG
		displayConsoleMsg(WIFI_AP);
#endif
	} else {
		char name[LEN_HOSTNAME + 1] = "";
		wmm.getString(POS_HOSTNAME, LEN_HOSTNAME, name);
		WiFi.hostname(name);
#ifdef SERIAL_DEBUG
		Serial.print(F("Hostname [ "));
		Serial.print(name);
		Serial.println(F(" ]"));
#endif
		uint8_t mode = wmm.getUint8(POS_WIFI_MODE);
		if (mode == VALUE_MODE_AP) {
			WiFi.mode(WIFI_AP);
		} else if (mode == VALUE_MODE_CLIENT) {
			WiFi.mode(WIFI_STA);
		} else if (mode == VALUE_MODE_BOTH) {
			WiFi.mode(WIFI_AP_STA);
		} else {
#ifdef SERIAL_DEBUG
			Serial.print(F("Unknown wifi mode [ "));
			Serial.print(mode);
			Serial.println(F(" ] !"));
#endif
			defaultWiFiConfig();
			return;
		}
		applyApConfig();
		applyStaConfig();
#ifdef SERIAL_DEBUG
		displayConsoleMsg(mode);
#endif
	}
}
/**
 * Apply the STA WiFi configuration
 */
void ApiConfigServer::applyApConfig() {
	uint8_t mode = wmm.getUint8(POS_WIFI_MODE);
	if (mode == VALUE_MODE_AP || mode == VALUE_MODE_BOTH) {
		char password[LEN_PASSWORD + 1] = "";
		char ssid[LEN_SSID + 1] = "";
		wmm.getString(POS_AP_PASSWORD, LEN_PASSWORD, password);
		wmm.getString(POS_AP_SSID, LEN_SSID, ssid);
		uint8_t power = wmm.getInt8(POS_AP_POWER);
#ifdef SERIAL_DEBUG
		Serial.printf("Starting AP with SSID %s and password %s at %i dBm\n",
				ssid, password, power);
#endif
		WiFi.softAP(ssid, password);
		WiFi.setOutputPower(wmm.getInt8(POS_AP_POWER));
	}
}
/**
 * Apply the STA WiFi configuration
 */
void ApiConfigServer::applyStaConfig() {
	uint8_t mode = wmm.getUint8(POS_WIFI_MODE);
	if (mode == VALUE_MODE_CLIENT || mode == VALUE_MODE_BOTH) {
		char password[LEN_PASSWORD + 1] = "";
		char ssid[LEN_SSID + 1] = "";
		uint8_t ip_config = wmm.getUint8(POS_IPCONFIG);
		if (ip_config == VALUE_IP_STATIC) {
			char temp[LEN_IP_ADDR + 1] = "";
			wmm.getString(POS_IP_ADDR, LEN_IP_ADDR, temp);
			IPAddress ip;
			if (ip.fromString(temp)) {
#ifdef SERIAL_DEBUG
				Serial.print(F("STA ip [ "));
				Serial.print(temp);
				Serial.println(F(" ]"));
#endif
				IPAddress mask;
				wmm.getString(POS_IP_MASK, LEN_IP_ADDR, temp);
				if (mask.fromString(temp)) {
#ifdef SERIAL_DEBUG
					Serial.print(F("STA mask [ "));
					Serial.print(temp);
					Serial.println(F(" ]"));
#endif
					IPAddress gw;
					wmm.getString(POS_IP_GW, LEN_IP_ADDR, temp);
					if (gw.fromString(temp)) {
#ifdef SERIAL_DEBUG
						Serial.print(F("STA gateway [ "));
						Serial.print(temp);
						Serial.println(F(" ]"));
#endif
						IPAddress dns;
						wmm.getString(POS_IP_DNS, LEN_IP_ADDR, temp);
						if (dns.fromString(temp)) {
#ifdef SERIAL_DEBUG
							Serial.print(F("STA dns [ "));
							Serial.print(temp);
							Serial.println(F(" ]"));
#endif
							WiFi.config(ip, gw, mask, dns, dns);
						} else {
#ifdef SERIAL_DEBUG
							Serial.println(F("Fail parsing dns address"));
#endif
						}
					} else {
#ifdef SERIAL_DEBUG
						Serial.println(F("Fail parsing gateway address"));
#endif
					}
				} else {
#ifdef SERIAL_DEBUG
					Serial.println(F("Fail parsing mask"));
#endif
				}
			} else {
#ifdef SERIAL_DEBUG
				Serial.println(F("Fail parsing IP address"));
#endif
			}
		}
		wmm.getString(POS_STA_PASSWORD, LEN_PASSWORD, password);
		wmm.getString(POS_STA_SSID, LEN_SSID, ssid);
#ifdef SERIAL_DEBUG
		Serial.printf("Trying to connect to %s with password %s with %s\n",
				ssid, password,
				ip_config == VALUE_IP_STATIC ? "static parameters" : "DHCP");
#endif
		WiFi.begin(ssid, password);
		// TODO see if network is not available what happens
		if (WiFi.waitForConnectResult() != WL_CONNECTED) {
			isStaFailed = true;
			defaultWiFiConfig();
#ifdef SERIAL_DEBUG
			displayConsoleMsg(WIFI_AP);
#endif
			return;
		}
		isStaFailed = false;
	}
}
/**
 * Apply the default WiFi configuration
 */
void ApiConfigServer::defaultWiFiConfig() {
#ifdef SERIAL_DEBUG
	Serial.printf("Switching to AP mode with SSID %s and password %s\n",
			WiFi.macAddress().c_str(), DEFAULT_AP_PASSWORD);
#endif
	WiFi.softAP(WiFi.macAddress().c_str(), DEFAULT_AP_PASSWORD);
}
#ifdef SERIAL_DEBUG
/**
 * Display WiFi configuration in serial console
 */
void ApiConfigServer::displayConsoleMsg(uint8_t mode) {
	if (mode == 1 || mode == 3) {
		Serial.printf("Open http://%s/ in your browser to see it working\n",
				WiFi.localIP().toString().c_str());
	}
	if (mode == 2 || mode == 3) {
		Serial.printf("Open http://%s/ in your browser to see it working\n",
				WiFi.softAPIP().toString().c_str());
	}
}
#endif
/**
 * Used to save a value and check if len is ok
 */
bool ApiConfigServer::saveValue(const char name[], uint16_t pos,
		uint8_t length) {
#ifdef SERIAL_DEBUG
	Serial.print(F("Parameter "));
	Serial.print(name);
#endif
	if (server->hasArg(name) && strlen(server->arg(name).c_str()) <= length) {
		wmm.setString(pos, server->arg(name).c_str());
#ifdef SERIAL_DEBUG
		Serial.println(F(" saved"));
#endif
		return true;
	}
#ifdef SERIAL_DEBUG
	Serial.println(F(" not saved !"));
#endif
	return false;
}
