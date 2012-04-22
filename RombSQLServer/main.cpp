#include "RServer.h"
#include "RException.h"
#include <iostream>
using namespace RSQL;

int main( void ) {
	try {
	RServer::Start();
	}
	catch(RException e) {cout << e.message();}
	return 0;	
}