#include "StylerTokenizer.h"

StylerTokenizer::~StylerTokenizer() {
}

bool StylerTokenizer::readComment() {
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
