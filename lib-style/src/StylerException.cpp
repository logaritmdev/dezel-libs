#include "StylerException.h"

StylerException::StylerException(unsigned int line,
	unsigned int column,
	std::string source) :
	source(source), line(line), column(column) {
}

StylerException::StylerException(const Token &token) :
	source(token.source), line(token.line), column(token.column) {
}

void StylerException::setLocation(unsigned int line, unsigned int column) {
	this->line = line;
	this->column = column;
}

unsigned int StylerException::getLineNumber() {
	return line;
}

unsigned int StylerException::getColumn() {
	return column;
}

/**
 * URL or file name where the Less code is located.
 */
void StylerException::setSource(std::string source) {
	this->source = source;
}

std::string StylerException::getSource() {
	return source;
}
