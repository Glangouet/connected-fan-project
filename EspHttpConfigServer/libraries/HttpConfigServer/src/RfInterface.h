/*
 * RfInterface.h
 *
 *  Created on: 26 mai 2018
 *      Author: Tala
 */

#ifndef SRC_RFINTERFACE_H_
#define SRC_RFINTERFACE_H_

class RfInterface {

private:
	Stream* serial;

public :

	virtual ~RfInterface(Stream* serial){
		this->serial = serial;
	}
	virtual void sendMessage();

private:
	virtual void readMessage();

};

#endif /* SRC_RFINTERFACE_H_ */
