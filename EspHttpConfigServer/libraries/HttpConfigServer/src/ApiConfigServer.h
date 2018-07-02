/*
 * HttpConfigServer.h
 *
 *  Created on: 16 sept. 2017
 *      Author: Deejc
 */

#ifndef SRC_APICONFIGSERVER_H_
#define SRC_APICONFIGSERVER_H_

#include <EepromManager.h>
#include <ESP8266WebServer.h>
#include <Options.h>

class ApiConfigServer {

private:
	static constexpr char URL_SHOW[] = "/api/conf";
	static constexpr char URL_APPLY[] = "/api/conf/apply";
	static constexpr char URL_SAVE[] = "/api/conf/save";
	static constexpr char URL_RESET[] = "/api/conf/reset";

	static constexpr char VAR_NAME[] = "hostname";
	static constexpr char VAR_STA_SSID[] = "sta_ssid";
	static constexpr char VAR_STA_PASSWORD[] = "sta_pwd";
	static constexpr char VAR_AP_SSID[] = "ap_ssid";
	static constexpr char VAR_AP_PASSWORD[] = "ap_pwd";
	static constexpr char VAR_AP_POWER[] = "ap_pwr";
	static constexpr char VAR_IPCONFIG[] = "ipconfig";
	static constexpr char VAR_WIFIMODE[] = "wifimode";
	static constexpr char VAR_IPADDR[] = "ipaddr";
	static constexpr char VAR_IPMASK[] = "netmask";
	static constexpr char VAR_IPGW[] = "gateway";
	static constexpr char VAR_IPDNS[] = "dns";
	static constexpr char VAR_APPLY[] = "to_apply";
	static constexpr char VAR_STATUS[] = "status";
	static constexpr char VAR_DISABLED[] = "disabled";

	static const uint8_t POS_HOSTNAME = 0;
	static const uint8_t LEN_HOSTNAME = 20;
	static const uint8_t POS_STA_SSID = POS_HOSTNAME + LEN_HOSTNAME + 1;
	static const uint8_t LEN_SSID = 20;
	static const uint8_t POS_STA_PASSWORD = POS_STA_SSID + LEN_SSID + 1;
	static const uint8_t LEN_PASSWORD = 20;
	static const uint8_t POS_IPCONFIG = POS_STA_PASSWORD + LEN_PASSWORD + 1;
	static const uint8_t LEN_IPCONFIG = 1;
	static const uint8_t POS_WIFI_MODE = POS_IPCONFIG + LEN_IPCONFIG + 1;
	static const uint8_t LEN_WIFI_MODE = 1;
	static const uint8_t POS_IP_ADDR = POS_WIFI_MODE + LEN_WIFI_MODE + 1;
	static const uint8_t LEN_IP_ADDR = 15;
	static const uint8_t POS_IP_MASK = POS_IP_ADDR + LEN_IP_ADDR + 1;
	static const uint8_t POS_IP_GW = POS_IP_MASK + LEN_IP_ADDR + 1;
	static const uint8_t POS_IP_DNS = POS_IP_GW + LEN_IP_ADDR + 1;
	static const uint8_t POS_AP_SSID = POS_IP_DNS + LEN_IP_ADDR + 1;
	static const uint8_t POS_AP_PASSWORD = POS_AP_SSID + LEN_SSID + 1;
	static const uint8_t POS_AP_POWER = POS_AP_PASSWORD + LEN_PASSWORD + 1;
	static const uint8_t LEN_AP_POWER = 2;
	static const uint8_t POS_DISABLED = POS_AP_POWER + LEN_AP_POWER + 1;
	static const uint8_t LEN_DISABLED = 1;
	static const uint8_t LAST_INDEX = POS_DISABLED + LEN_DISABLED;

	static const uint8_t VALUE_MODE_CLIENT = WIFI_STA;
	static const uint8_t VALUE_MODE_AP = WIFI_AP;
	static const uint8_t VALUE_MODE_BOTH = WIFI_AP_STA;
	static const uint8_t VALUE_IP_DHCP = 0;
	static const uint8_t VALUE_IP_STATIC = 1;

	static constexpr char DEFAULT_AP_PASSWORD[] = "123456789";

	static ESP8266WebServer* server;
	static EepromManager wmm;
	static bool isStaFailed;

public:
	/**
	 * Used in the setup function to start the ApiConfigServer class
	 */
	void setup(ESP8266WebServer* server);
	/**
	 * Call this method in the 'loop' function
	 */
	void loop();
	/**
	 * Disables the module
	 */
	static void setDisabled(bool isDisabled);
	/**
	 * True if module has been disabled
	 */
	static bool isDisabled();
	/**
	 * Append the hostname stored in configuration
	 */
	static void getHostname(char hostname[]);
	/**
	 * Return the current configuration in Json format
	 */
	static void toJson(char* json);
private:

	static void show();
	static void save();
	static void apply();
	static void reset();
	static void defaultWiFiConfig();
	static void applyConfig();
	static void applyApConfig();
	static void applyStaConfig();
	static void displayConsoleMsg(uint8_t mode);
	static bool saveValue(const char name[], uint16_t pos, uint8_t length);

};

#endif /* SRC_APICONFIGSERVER_H_ */
