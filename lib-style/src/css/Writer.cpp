#include "Writer.h"
#include "Selector.h"

namespace Press {

Writer::Writer() {
	out = NULL;
	column = 0;
	rootpath = NULL;
}

Writer::Writer(std::ostream &out) : out(&out), column(0) {
	sourcemap = NULL;
}

Writer::~Writer() {
}

unsigned int Writer::getColumn() {
	return column;
}

void Writer::newline() {

}

void Writer::writeStr(const char* str, size_t len) {

}

void Writer::writeToken(const Token &token) {

}

void Writer::writeTokenList(const TokenList &tokens) {

}

void Writer::writeSelector(const Selector &selector) {

}

void Writer::writeValue(const TokenList &value) {

}

void Writer::writeAtRule(const Token &keyword, const TokenList &rule) {

}

void Writer::writeRulesetStart(const Selector &selector) {

}

void Writer::writeRulesetEnd() {

}

void Writer::writeDeclaration(const Token &property, const TokenList &value) {

}

void Writer::writeDeclarationDeliminator() {

}

void Writer::writeComment(const Token &comment) {

}

void Writer::writeMediaQueryStart(const TokenList &selector) {

}

void Writer::writeMediaQueryEnd() {

}

void Writer::writeSourceMapUrl(const char* sourcemap_url) {

}

}
