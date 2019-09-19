#include "PressTokenizer.h"

namespace Press {

PressTokenizer::~PressTokenizer() {
}

bool PressTokenizer::readComment() {
	if (!lastReadEq('/'))
		return Tokenizer::readComment();

	currentToken.append(lastRead);
	readChar();
	while (in != NULL && !lastReadEq('\n')) {
		currentToken.append(lastRead);
		readChar();
	}
	return true;
}

}
