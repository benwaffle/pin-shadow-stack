#ifndef _CALL_H_
#define _CALL_H_

#include <string>
#include <sstream>
#include "util.h"

using namespace std;

#ifdef DEBUG

	struct call {
		void *call_ins; // address of call instruction
		string caller; // calling function
		
		void *target_addr; // branch target (address)
		string callee; // function being called

		call(void *call_ins, string caller, void *target_addr, string callee):
			call_ins(call_ins), caller(caller), target_addr(target_addr), callee(callee) {}

		call(): call_ins(nullptr), caller(""), target_addr(nullptr), callee("") {}

		string str() {
			stringstream str;
			str << YELLOW << call_ins << RESET ": call "
				<< target_addr << " <" << callee << ">";
			return str.str();
		}
	};

#else

	typedef void *call;

#endif

#endif