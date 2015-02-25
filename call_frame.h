#pragma once

#include <ostream>
#include "pin.H"
#include "util.h"

#ifdef DEBUG

	struct CallFrame {
		void *call_ins; // address of call instruction
		void *target_addr; // target of call instruction
	};

	inline
	std::ostream& operator<<(std::ostream& os, const CallFrame& c) {
		return os <<
			YELLOW << c.call_ins << RESET ": "
			"call " << c.target_addr <<
			" <" << RTN_FindNameByAddress((ADDRINT)c.target_addr) << ">";
	}

#else

	typedef void* CallFrame;

#endif
