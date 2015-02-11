#pragma once

#include <string>
#include <ostream>
#include "pin.H"
#include "util.h"

using namespace std;

#ifdef DEBUG

	struct call {
		void *call_ins; // address of call instruction
		void *target_addr; // branch target (address)

		call(void *call_ins, void *target_addr):
			call_ins(call_ins), target_addr(target_addr) {}

		call(): call_ins(nullptr), target_addr(nullptr) {}
	};

	ostream& operator<<(ostream& os, const call& c) {
		if (RTN_FindNameByAddress((ADDRINT)c.target_addr).find("programEntryThunkGenerator") != string::npos)
		{
			PIN_LockClient();
			auto rtn = RTN_FindByAddress((ADDRINT)c.target_addr);
			if (RTN_Valid(rtn))
			{
				RTN_Open(rtn);
				auto ins = RTN_InsHead(rtn);
				os << " [ " << StringFromAddrint(INS_Address(ins)) << " ] ";
				RTN_Close(rtn);
			}
			PIN_UnlockClient();
		}
		return os << YELLOW << c.call_ins << RESET ": call " << c.target_addr << " <" << RTN_FindNameByAddress((ADDRINT)c.target_addr) << ">";
	}

#else

	typedef void *call;

#endif