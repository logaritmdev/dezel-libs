#ifndef __PressTokenizer_h__
#define __PressTokenizer_h__

#include <iostream>
#include "Tokenizer.h"

namespace Press {

class PressTokenizer : public Tokenizer {

public:
	PressTokenizer(istream &in, const char* source) : Tokenizer(in, source) {};
	virtual ~PressTokenizer();

protected:
	bool readComment();
};

}

#endif
