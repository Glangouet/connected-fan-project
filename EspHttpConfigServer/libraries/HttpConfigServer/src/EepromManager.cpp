/*
 * WiFiMemManager.cpp
 *
 *  Created on: 11 août 2017
 *      Author: jcf
 */

#include <EepromManager.h>
#include <EEPROM.h>
#include <stdlib_noniso.h>

constexpr char EepromManager::INIT_SEQUENCE[];

EepromManager::EepromManager() {
	EEPROM.begin(MEM_SIZE);
}

void EepromManager::init() {
	for (uint8_t i = 0; i < strlen(INIT_SEQUENCE); i++) {
		EEPROM.write(i, INIT_SEQUENCE[i]);
	}
	EEPROM.commit();
}

void EepromManager::erase() {
	for (uint8_t i = 0; i < strlen(INIT_SEQUENCE); i++) {
		EEPROM.write(i, '\0');
	}
	EEPROM.commit();
}
/**
 * True if memory contains the init sequence, false otherwise
 */
bool EepromManager::isEmpty() {
	uint8_t writtenBytes = 0;
	while (writtenBytes < strlen(INIT_SEQUENCE)) {
		if (INIT_SEQUENCE[writtenBytes] != char(EEPROM.read(writtenBytes))) {
			return true;
		}
		writtenBytes++;
	}
	return false;
}

char EepromManager::getChar(uint16_t pos) {
	return atol(getMem(pos, sizeof(char)));
}
bool EepromManager::getBool(uint16_t pos) {
	char* str = getMem(pos, sizeof(bool));
	return str[0] == '1';
}
uint8_t EepromManager::getUint8(uint16_t pos) {
	return atol(getMem(pos, sizeof(uint8_t)));
}
uint16_t EepromManager::getUint16(uint16_t pos) {
	return atol(getMem(pos, sizeof(uint16_t)));
}
uint32_t EepromManager::getUint32(uint16_t pos) {
	return atol(getMem(pos, sizeof(uint32_t)));
}
uint64_t EepromManager::getUint64(uint16_t pos) {
	return atol(getMem(pos, sizeof(uint64_t)));
}
int8_t EepromManager::getInt8(uint16_t pos) {
	return atol(getMem(pos, sizeof(int8_t)));
}
int16_t EepromManager::getInt16(uint16_t pos) {
	return atol(getMem(pos, sizeof(int16_t)));
}
int32_t EepromManager::getInt32(uint16_t pos) {
	return atol(getMem(pos, sizeof(int32_t)));
}
int64_t EepromManager::getInt64(uint16_t pos) {
	return atol(getMem(pos, sizeof(int64_t)));
}
void EepromManager::getString(uint16_t pos, uint8_t size, char* tmp) {
	char buf[size + 1];
	buf[0] = '\0';
	EepromManager::appendString(pos, size, buf);
	strcpy(tmp, buf);
}
void EepromManager::appendString(uint16_t pos, uint8_t size, char* tmp) {
	pos += strlen(INIT_SEQUENCE);
	uint16_t index = strlen(tmp);
	uint8_t writtenBytes = 0;
	while (writtenBytes < size + 1) {
		tmp[index + writtenBytes] = char(EEPROM.read(pos + writtenBytes));
		writtenBytes++;
	}
	tmp[index + writtenBytes] = '\0';
}

IPAddress EepromManager::getAddress(uint16_t pos) {
	IPAddress addr;
	addr.fromString(getMem(pos, SIZE_ADDRESS));
	return addr;
}
void EepromManager::setChar(uint16_t pos, const char c) {
	char str[2];
	str[0] = c;
	str[1] = '\0';
	setMem(pos, str);
}
void EepromManager::setBool(uint16_t pos, bool b) {
	if (b) {
		setMem(pos, "1");
	} else {
		setMem(pos, "0");
	}
}
void EepromManager::setUint8(uint16_t pos, uint8_t i) {
	char str[sizeof(uint8_t)];
	ltoa(i, str, 10);
	setMem(pos, str);
}
void EepromManager::setUint16(uint16_t pos, uint16_t i) {
	char str[sizeof(uint16_t)];
	ltoa(i, str, 10);
	setMem(pos, str);
}
void EepromManager::setUint32(uint16_t pos, uint32_t i) {
	char str[sizeof(uint32_t)];
	ltoa(i, str, 10);
	setMem(pos, str);
}
void EepromManager::setUint64(uint16_t pos, uint64_t i) {
	char str[sizeof(uint64_t)];
	ltoa(i, str, 10);
	setMem(pos, str);
}
void EepromManager::setInt8(uint16_t pos, int8_t i) {
	char str[sizeof(int8_t)];
	ltoa(i, str, 10);
	setMem(pos, str);
}
void EepromManager::setInt16(uint16_t pos, int16_t i) {
	char str[sizeof(int16_t)];
	ltoa(i, str, 10);
	setMem(pos, str);
}
void EepromManager::setInt32(uint16_t pos, int32_t i) {
	char str[sizeof(int32_t)];
	ltoa(i, str, 10);
	setMem(pos, str);
}
void EepromManager::setInt64(uint16_t pos, int64_t i) {
	char str[sizeof(int64_t)];
	ltoa(i, str, 10);
	setMem(pos, str);
}
void EepromManager::setString(uint16_t pos, const char str[]) {
	setMem(pos, str);
}
void EepromManager::setAddress(uint16_t pos, IPAddress address) {
	setMem(pos, address.toString().c_str());
}

char* EepromManager::getMem(uint16_t pos, uint8_t size) {
	pos += strlen(INIT_SEQUENCE);
	uint8_t writtenBytes = 0;
	while (writtenBytes < size + 1) {
		str[writtenBytes] = char(EEPROM.read(pos + writtenBytes));
		writtenBytes++;
	}
	str[writtenBytes] = '\0';
	return str;
}
void EepromManager::setMem(uint16_t pos, const char str[]) {
	pos += strlen(INIT_SEQUENCE);
	for (uint8_t i = 0; i < strlen(str); i++) {
		EEPROM.write(pos + i, str[i]);
	}
	EEPROM.write(pos + strlen(str), '\0');
	EEPROM.commit();
}
