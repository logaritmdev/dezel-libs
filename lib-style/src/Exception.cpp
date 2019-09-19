#include "Exception.h"

namespace Press {

Exception::Exception(unsigned int line,
	unsigned int column,
	std::string source) :
	source(source), line(line), column(column) {
}

Exception::Exception(const Token &token) :
	source(token.source), line(token.line), column(token.column) {
}

void Exception::setLocation(unsigned int line, unsigned int column) {
	this->line = line;
	this->column = column;
}

unsigned int Exception::getLine() {
	return line;
}

unsigned int Exception::getColumn() {
	return column;
}

/**
 * URL or file name where the Press code is located.
 */
void Exception::setSource(std::string source) {
	this->source = source;
}

std::string Exception::getSource() {
	return source;
}

}
