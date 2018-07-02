/*
 * ApiDiscoveryServer.cpp
 *
 *  Created on: 27 mai 2018
 *      Author: Tala
 */

#include <ApiDiscoveryServer.h>
#include <ApiConfigServer.h>

ESP8266WebServer* ApiDiscoveryServer::server;

constexpr char ApiDiscoveryServer::MESSAGE_HELLO[];
constexpr char ApiDiscoveryServer::MESSAGE_DISCOVERY[];
constexpr char ApiDiscoveryServer::MESSAGE_INFO[];
constexpr char ApiDiscoveryServer::MESSAGE_ACK[];
constexpr char ApiDiscoveryServer::MESSAGE_NACK[];

IPAddress ApiDiscoveryServer::MCAST_ADDR(239, 0, 0, 50);
const uint16_t ApiDiscoveryServer::MCAST_PORT;
const uint16_t ApiDiscoveryServer::DISCOVERY_INTERVAL;
const uint16_t ApiDiscoveryServer::BUFFER_SIZE;
char ApiDiscoveryServer::buffer[BUFFER_SIZE];

void ApiDiscoveryServer::setup(ESP8266WebServer* server) {
	this->server = server;
	elapsed = millis();
	startMulticast();
}
void ApiDiscoveryServer::loop() {
	if (isMsg) {
		isMsg = false;
	}
	readPacket();
	autoDiscovery();
}

void ApiDiscoveryServer::discover() {

}
/**
 * Used to send an UDP packet
 */
void ApiDiscoveryServer::sendPacket(const char content[], IPAddress ip,
		uint16_t port) {
	udp.beginPacket(ip, port);
	udp.write(content);
	udp.endPacket();
#ifdef SERIAL_DEBUG
	Serial.printf("Sending [ %s ] to %s:%i\n", content, ip.toString().c_str(),
			port);
#endif
}
/**
 * Shortcut to respond with ACK
 */
void ApiDiscoveryServer::sendBackAck() {
	sendPacket(MESSAGE_ACK, udp.remoteIP(), udp.remotePort());
}
/**
 * Shortcut to respond with NACK
 */
void ApiDiscoveryServer::sendBackNack() {
	sendPacket(MESSAGE_NACK, udp.remoteIP(), udp.remotePort());
}
/**
 * Send the hostname
 */
void ApiDiscoveryServer::sendInfo() {
	char msg[30] = "INFO:";
	ApiConfigServer::getHostname(msg);
	sendPacket(msg, udp.remoteIP(), udp.remotePort());
}
/**
 * Send the full configuration
 */
void ApiDiscoveryServer::sendConf() {
	char json[700] = "";
	ApiConfigServer::toJson(json);
	sendPacket(json, udp.remoteIP(), udp.remotePort());
}
/**
 * Used to interpret the packet
 */
void ApiDiscoveryServer::readPacket() {
	if (udp.parsePacket() > 0) {
		isMsg = true;
		packet_len = udp.available();
		udp.read(buffer, packet_len);
		buffer[packet_len] = '\0';
#ifdef SERIAL_DEBUG
		Serial.printf(
				"Packet of %i bytes received from %s:%i containing [ %s ]\n",
				packet_len, udp.remoteIP().toString().c_str(), udp.remotePort(),
				buffer);
#endif
		if (strcmp(buffer, MESSAGE_DISCOVERY) == 0) {
			// 'INFO' message, sending INFO back
			sendInfo();
		} else if (strcmp(buffer, MESSAGE_INFO) == 0) {
			// 'CONF' message, sending INFO back
			sendConf();
		} else if (udp.destinationIP() != MCAST_ADDR) {
			// Not multicast
			sendBackNack();
		}
	}
}
/**
 * Starts the multicast server
 */
void ApiDiscoveryServer::startMulticast() {
#ifdef SERIAL_DEBUG
	Serial.printf("Starting multicast on %s:%i ", MCAST_ADDR.toString().c_str(),
			MCAST_PORT);
#endif
	if (WiFi.getMode() == WIFI_AP) {
#ifdef SERIAL_DEBUG
		Serial.println("over AP interface");
#endif
		udp.beginMulticast(WiFi.softAPIP(), MCAST_ADDR, MCAST_PORT);
	} else {
#ifdef SERIAL_DEBUG
		Serial.println("over STA interface");
#endif
		udp.beginMulticast(WiFi.localIP(), MCAST_ADDR, MCAST_PORT);
	}
	sendPacket(MESSAGE_HELLO, MCAST_ADDR, MCAST_PORT);
}
/**
 * Sends HELO packet every DISCOVERY_INTERVAL
 */
void ApiDiscoveryServer::autoDiscovery() {
	uint64_t since = (millis() - this->elapsed) / 1000;
	if (since > DISCOVERY_INTERVAL) {
		sendPacket(MESSAGE_HELLO, MCAST_ADDR, MCAST_PORT);
		this->elapsed = millis();
	}
}
