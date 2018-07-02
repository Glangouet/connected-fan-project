/*
 * WiFiMemSaver.h
 *
 *  Created on: 11 ao�t 2017
 *      Author: jcf
 */

#ifndef LIBRARIES_WEBSERVER_WIFI_SRC_WIFIMEMMANAGER_H_
#define LIBRARIES_WEBSERVER_WIFI_SRC_WIFIMEMMANAGER_H_

#include <IPAddress.h>

class EepromManager {

public:
	/**
	 * The size used in the EEPROM
	 */
	static const uint16_t MEM_SIZE = 512;

private:
	static const uint8_t SIZE_ADDRESS = 3 * sizeof(int8_t) + 3;
	static constexpr char INIT_SEQUENCE[] = "!@$";
	char str[21];

public:

	/**
	 * Start an instance of WiFiMemManager
	 */
	EepromManager();
	/**
	 * Write initialization sequence at the beggining of the EEPROM
	 */
	void init();
	/**
	 * Erase initialization sequence at the beggining of the EEPROM
	 */
	void erase();
	/**
	 * Test EEPROM for initialization sequence
	 */
	static bool isEmpty();
	/**
	 * Return caracter at index 'pos'
	 */
	char getChar(uint16_t pos);
	/**
	 * Return boolean at index 'pos'
	 */
	bool getBool(uint16_t pos);
	/**
	 * Return unsigned int at index 'pos'
	 */
	uint8_t getUint8(uint16_t pos);
	/**
	 * Return unsigned int at index 'pos'
	 */
	uint16_t getUint16(uint16_t pos);
	/**
	 * Return unsigned int at index 'pos'
	 */
	uint32_t getUint32(uint16_t pos);
	/**
	 * Return unsigned int at index 'pos'
	 */
	uint64_t getUint64(uint16_t pos);
	/**
	 * Return signed int at index 'pos'
	 */
	int8_t getInt8(uint16_t pos);
	/**
	 * Return signed int at index 'pos'
	 */
	int16_t getInt16(uint16_t pos);
	/**
	 * Return signed int at index 'pos'
	 */
	int32_t getInt32(uint16_t pos);
	/**
	 * Return signed int at index 'pos'
	 */
	int64_t getInt64(uint16_t pos);
	/**
	 * Fill the char array 'tmp' with memory from index 'pos' until size 'size'
	 */
	void getString(uint16_t pos, uint8_t size, char* tmp);
	/**
	 * Append to the char array 'tmp' the memory content from index 'pos' until size 'size'
	 */
	void appendString(uint16_t pos, uint8_t size, char* tmp);
	/**
	 * Return the IP address at index 'pos'
	 */
	IPAddress getAddress(uint16_t pos);
	/**
	 * Write the char 'c' at index 'pos'
	 */
	void setChar(uint16_t pos, const char c);
	/**
	 * Write the boolean 'b' at index 'pos'
	 */
	void setBool(uint16_t pos, bool b);
	/**
	 * Write the unsigned int 'i' at index 'pos'
	 */
	void setUint8(uint16_t pos, uint8_t i);
	/**
	 * Write the unsigned int 'i' at index 'pos'
	 */
	void setUint16(uint16_t pos, uint16_t i);
	/**
	 * Write the unsigned int 'i' at index 'pos'
	 */
	void setUint32(uint16_t pos, uint32_t i);
	/**
	 * Write the unsigned int 'i' at index 'pos'
	 */
	void setUint64(uint16_t pos, uint64_t i);
	/**
	 * Write the signed int 'i' at index 'pos'
	 */
	void setInt8(uint16_t pos, int8_t i);
	/**
	 * Write the signed int 'i' at index 'pos'
	 */
	void setInt16(uint16_t pos, int16_t i);
	/**
	 * Write the signed int 'i' at index 'pos'
	 */
	void setInt32(uint16_t pos, int32_t i);
	/**
	 * Write the signed int 'i' at index 'pos'
	 */
	void setInt64(uint16_t pos, int64_t i);
	/**
	 * Write the char array 'str' at index 'pos'
	 */
	void setString(uint16_t pos, const char str[]);
	/**
	 * Write the IP address 'address' at index 'pos'
	 */
	void setAddress(uint16_t pos, IPAddress address);

private:

	char* getMem(uint16_t pos, uint8_t size);
	void setMem(uint16_t pos, const char str[]);
};

#endif /* LIBRARIES_WEBSERVER_WIFI_SRC_WIFIMEMMANAGER_H_ */
