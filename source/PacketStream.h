
#ifndef PACKETSTREAM_H
#define PACKETSTREAM_H

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class PacketStream {

private:    
	ostringstream outputStream;
	istringstream inputStream;

public:

	PacketStream();
	~PacketStream();

	void writeInt(int pVal);
	void readInt(int &pVal);

	void writeFloat(float pVal);
	void readFloat(float &pVal);

	void writeBool(bool pVal);
	void readBool(bool &pVal);

	void writeChar(char pVal);
	void readChar(char* pVal);

	void toCharArray(char* arrayIn);
	void fromCharArray(char* arrayIn);
};

#endif