#include "Tokenizer.h"

namespace Press {

Tokenizer::Tokenizer(istream &in, const char* source)
	: in(&in), line(0), source(source) {
	currentToken.source = source;
	lastRead = 0;
	readChar();
	column = 0;
}

Tokenizer::~Tokenizer() {
}

const char* Tokenizer::getSource() {
	return source;
}

void Tokenizer::readChar() {
	if (in == NULL)
		return;

	// Last char was a newline. Increment the line counter.
	if (lastReadEq('\n')) {
		line++;
		column = 0;
	} else
		column++;

	in->get(lastRead);

	// check for end of file or escape key
	if (in->eof() || lastRead == 27)
		in = NULL;
	else if (in->fail() || in->bad())
		throw "Error reading input";

	if (lastReadEq('\n') && column > 0)  // don't count newlines as chars
		column--;
}

Token::Type Tokenizer::readNextToken() {

	if (in == NULL) {
		currentToken.type = Token::EOS;
		return Token::EOS;
	}

	currentToken.clear();
	currentToken.line = line;
	currentToken.column = column;

	switch (lastRead) {

		case '$':
			currentToken.type = Token::VAR;
			currentToken.append(lastRead);
			readChar();
			if (!readIdent()) {
				currentToken.type = Token::OTHER;
			}
			break;

		case '@':
			currentToken.type = Token::AT;
			currentToken.append(lastRead);
			readChar();
			if (!readIdent()) {
				currentToken.type = Token::OTHER;
			}
			break;

		case '#':
			currentToken.type = Token::HASH;
			currentToken.append(lastRead);
			readChar();
			if (!readName()) {
				throw new ParseException(
					&lastRead, "name following '#'", line, column, source);
			}
			break;

		case '-':
			currentToken.append(lastRead);
			readChar();
			if (readNum(true)) {
				currentToken.type = Token::NUMBER;
				readNumSuffix();
			} else if (readIdent()) {
				currentToken.type = Token::IDENTIFIER;
			} else
				currentToken.type = Token::OTHER;
			break;

		case '~':
			currentToken.append(lastRead);
			readChar();
			if (lastRead == '=') {
				currentToken.append(lastRead);
				readChar();
				currentToken.type = Token::INCLUDES;
			} else
				currentToken.type = Token::OTHER;
			break;

		case '|':
			currentToken.append(lastRead);
			readChar();
			if (lastRead == '=') {
				currentToken.append(lastRead);
				readChar();
				currentToken.type = Token::DASHMATCH;
			} else
				currentToken.type = Token::OTHER;
			break;

		case '/':
			currentToken.append(lastRead);
			readChar();
			if (readComment())
				currentToken.type = Token::COMMENT;
			else
				currentToken.type = Token::OTHER;
			break;

		case ';':
			currentToken.type = Token::DELIMITER;
			currentToken.append(lastRead);
			readChar();
			break;

		case ':':
			currentToken.type = Token::COLON;
			currentToken.append(lastRead);
			readChar();
			break;

		case '{':
			currentToken.type = Token::BRACKET_OPEN;
			currentToken.append(lastRead);
			readChar();
			break;

		case '}':
			currentToken.type = Token::BRACKET_CLOSED;
			currentToken.append(lastRead);
			readChar();
			break;

		case '(':
			currentToken.type = Token::PAREN_OPEN;
			currentToken.append(lastRead);
			readChar();
			break;

		case ')':
			currentToken.type = Token::PAREN_CLOSED;
			currentToken.append(lastRead);
			readChar();
			break;

		case '[':
			currentToken.type = Token::BRACE_OPEN;
			currentToken.append(lastRead);
			readChar();
			break;

		case ']':
			currentToken.type = Token::BRACE_CLOSED;
			currentToken.append(lastRead);
			readChar();
			break;

		case '.':
			currentToken.append(lastRead);
			readChar();
			if (readNum(false)) {
				currentToken.type = Token::NUMBER;
				readNumSuffix();
			}
			break;

		default:

			if (readString())
				currentToken.type = Token::STRING;
			else if (readNum(true)) {
				currentToken.type = Token::NUMBER;
				readNumSuffix();

			} else if (readIdent()) {
				currentToken.type = Token::IDENTIFIER;

				if (currentToken == "url" && readUrl())
					currentToken.type = Token::URL;
				else if (currentToken == "u" && lastReadEq('+')) {
					currentToken.append(lastRead);
					readChar();
					currentToken.type = Token::UNICODE_RANGE;
					readUnicodeRange();
				}
			} else if (readWhitespace()) {
				currentToken.type = Token::WHITESPACE;
				while (readWhitespace()) {
				};
			} else {
				currentToken.append(lastRead);
				readChar();
			}
			break;
	}

	return currentToken.type;
}

bool Tokenizer::readIdent() {

	if (lastReadEq('-')) {
		currentToken.append(lastRead);
		readChar();
	}

	if (readNMStart() == false) {
		return false;
	} else {
		while (readNMChar());
	}

	return true;
}

bool Tokenizer::readName() {

	if (readNMChar() == false) {
		return false;
	}

	while (readNMChar());

	return true;
}

bool Tokenizer::readNMStart() {

	if (in == NULL) {
		return false;
	}

	if (lastReadEq('_') ||
		lastReadInRange('a', 'z') ||
		lastReadInRange('A', 'Z')) {
		currentToken.append(lastRead);
		readChar();
		return true;
	} else {
		return (readNonAscii() || readEscape());
	}
}

bool Tokenizer::readNonAscii() {

	if (in == NULL || lastRead >= 0) {
		return false;
	}

	currentToken.append(lastRead);
	readChar();

	return true;
}

bool Tokenizer::readEscape() {

	if (lastReadEq('\\') == false) {
		return false;
	}

	currentToken.append(lastRead);
	readChar();

	if (readUnicode()) {
		return true;
	} else if (!lastReadEq('\n') && !lastReadEq('\r') && !lastReadEq('\f')) {
		currentToken.append(lastRead);
		readChar();
		return true;
	} else {
		return false;
	}
}

bool Tokenizer::readUnicode() {

	if (lastReadIsHex() == false) {
		return false;
	}

	// [0-9a-f]{1,6}(\r\n|[ \n\r\t\f])?
	for (int i = 0; i < 6; i++) {

		currentToken.append(lastRead);
		readChar();

		if (readWhitespace() || !lastReadIsHex()) {
			break;
		}
	}

	return true;
}

bool Tokenizer::readNMChar() {

	if (in == NULL) {
		return false;
	}

	if (lastReadEq('_') ||
		lastReadInRange('a', 'z') ||
		lastReadInRange('A', 'Z') ||
		lastReadIsDigit() ||
		lastReadEq('-')) {
		currentToken.append(lastRead);
		readChar();
		return true;
	} else {
		return (readNonAscii() || readEscape());
	}
}

bool Tokenizer::readNum(bool readDecimals) {

	if (lastReadIsDigit() == false) {
		return false;
	}

	while (lastReadIsDigit()) {
		currentToken.append(lastRead);
		readChar();
	}

	if (readDecimals && lastReadEq('.')) {

		currentToken.append(lastRead);
		readChar();

		while (lastReadIsDigit()) {
			currentToken.append(lastRead);
			readChar();
		}
	}

	return true;
}

bool Tokenizer::readNumSuffix() {

	if (lastRead == '%') {
		currentToken.type = Token::PERCENTAGE;
		currentToken.append(lastRead);
		readChar();
		return true;
	}

	if (readIdent()) {
		currentToken.type = Token::DIMENSION;
		return true;
	}

	return false;
}

bool Tokenizer::readString() {

	if (lastReadEq('"') == false &&
		lastReadEq('\'') == false) {
		return false;
	}

	char delim = lastRead;

	currentToken.append(lastRead);
	readChar();

	while (in != NULL) {

		if (lastReadEq(delim)) {
			currentToken.append(lastRead);
			readChar();
			return true;
		}

		if (lastReadEq('\n') ||
			lastReadEq('\r') ||
			lastReadEq('\f')) {
			throw new ParseException(
				"end of line",
				"end of string",
				line, column, source
			);
		}

		if (lastReadEq('\\')) {
			// note that even though readEscape() returns false it still
			// eats the '\'.
			readEscape() || readNewline();
		} else {
			currentToken.append(lastRead);
			readChar();
		}
	}

	throw new ParseException(
		"end of input",
		"end of string",
		line, column, source
	);

	return false;
}

bool Tokenizer::readNewline() {

	if (lastReadEq('\r')) {

		currentToken.append(lastRead);
		readChar();

		if (lastReadEq('\n')) {
			currentToken.append(lastRead);
			readChar();
		}

		return true;
	}

	if (lastReadEq('\n') ||
		lastReadEq('\f')) {
		currentToken.append(lastRead);
		readChar();
		return true;
	}

	return false;
}

bool Tokenizer::readWhitespace() {

	if (lastReadEq(' ') ||
		lastReadEq('\t') ||
		lastReadEq('\r') ||
		lastReadEq('\n') ||
		lastReadEq('\f')) {
		currentToken.append(lastRead);
		readChar();
		return true;
	}

	return false;
}

bool Tokenizer::readUrl() {

	string urlchars = "!#$%&*-[]-~";

	if (lastReadEq('(') == false) {
		return false;
	}

	currentToken.append(lastRead);
	readChar();

	while (readWhitespace());

	if (readString()) {

		if (lastReadEq(')')) {
			currentToken.append(lastRead);
			readChar();
			return true;
		}

		throw new ParseException(
			&lastRead,
			"end of url (')')",
			line, column, source
		);
	}

	while (in != NULL) {

		if (readWhitespace() || lastReadEq(')')) {

			while (readWhitespace());

			if (lastReadEq(')')) {
				currentToken.append(lastRead);
				readChar();
				return true;
			}

			throw new ParseException(
				&lastRead,
				"end of url (')')",
				line, column, source
			);
		}

		if (in != NULL && urlchars.find(lastRead)) {

			currentToken.append(lastRead);
			readChar();

		} else if (!readNonAscii() && !readEscape()) {

			throw new ParseException(
				&lastRead,
				"end of url (')')",
				line, column, source
			);

		}
	}
	throw new ParseException(&lastRead, "end of url (')')", line, column, source);
	return false;
}

bool Tokenizer::readComment() {

	if (lastReadEq('*') == false) {
		return false;
	}

	currentToken.append(lastRead);
	readChar();

	while (in != NULL) {

		if (lastReadEq('*')) {

			currentToken.append(lastRead);
			readChar();

			if (lastReadEq('/')) {
				currentToken.append(lastRead);
				readChar();
				return true;
			}

			continue;
		}

		currentToken.append(lastRead);
		readChar();
	}

	throw new ParseException(
		&lastRead,
		"end of comment (*/)",
		 line, column, source
	 );

	return false;
}

bool Tokenizer::readUnicodeRange() {

	if (in == NULL) {
		return false;
	}

	for (int i = 0; i < 6; i++) {

		if (lastReadIsHex() == false) {
			break;
		}

		currentToken.append(lastRead);
		readChar();
	}

	if (lastReadEq('-') == false) {
		return true;
	}

	for (int i = 0; i < 6; i++) {

		if (lastReadIsHex() == false) {
			break;
		}

		currentToken.append(lastRead);
		readChar();
	}

	return true;
}

Token &Tokenizer::getToken() {
	return currentToken;
}

Token::Type Tokenizer::getTokenType() {
	return currentToken.type;
}

bool Tokenizer::lastReadEq(char c) {
	return (in != NULL && lastRead == c);
}

bool Tokenizer::lastReadInRange(char c1, char c2) {
	return (in != NULL && lastRead >= c1 && lastRead <= c2);
}

bool Tokenizer::lastReadIsDigit() {
	return (in != NULL && lastReadInRange('0', '9'));
}

bool Tokenizer::lastReadIsHex() {
	return (in != NULL && (
		lastReadIsDigit() ||
		lastReadInRange('a', 'f') ||
		lastReadInRange('A', 'F')
   ));
}

}
