/*
 * ApiDiscoveryServer.h
 *
 *  Created on: 27 mai 2018
 *      Author: Tala
 */

#ifndef SRC_APIDISCOVERYSERVER_H_
#define SRC_APIDISCOVERYSERVER_H_

#include <WiFiUDP.h>
#include <ESP8266WebServer.h>
#include <IPAddress.h>
#include <Options.h>

class ApiDiscoveryServer {

private:
	static ESP8266WebServer* server;

	static IPAddress MCAST_ADDR;
	static const uint16_t MCAST_PORT = 7777;
	static const uint16_t BUFFER_SIZE = 512;
	static char buffer[BUFFER_SIZE];
	uint16_t packet_len = 0;bool isMsg = false;

	static constexpr char MESSAGE_HELLO[] = "HELO";
	static constexpr char MESSAGE_DISCOVERY[] = "INFO";
	static constexpr char MESSAGE_INFO[] = "CONF";
	static constexpr char MESSAGE_ACK[] = "ACK";
	static constexpr char MESSAGE_NACK[] = "NACK";

	WiFiUDP udp;

	static const uint16_t DISCOVERY_INTERVAL = 60;
	uint64_t elapsed;

public:
	/**
	 * Used in the setup function to start the ApiDiscoveryServer class
	 */
	void setup(ESP8266WebServer* server);
	/**
	 * Call this method in the 'loop' function
	 */
	void loop();
	/**
	 * True if a message has been received
	 * The 'memory' is reset each time loop is called
	 */
	void isMessage();
	/**
	 * Return the received message
	 */
	char* getMessage();

private:
	void discover();
	void sendBackAck();
	void sendBackNack();
	void sendInfo();
	void sendConf();
	void autoDiscovery();
	void sendPacket(const char content[], IPAddress ip, uint16_t port);
	void readPacket();
	void startMulticast();

};

#endif /* SRC_APIDISCOVERYSERVER_H_ */
