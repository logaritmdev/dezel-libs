#include "Parser.h"
#include <iostream>
#include "MediaQuery.h"
#include "AtRule.h"
#include "Ruleset.h"

namespace Press {

Parser::Parser(Tokenizer &tokenizer) {
	this->tokenizer = &tokenizer;
}

void Parser::parseStylesheet(Stylesheet &stylesheet) {

	tokenizer->readNextToken();

	skipWhitespace();
	while (parseStatement(stylesheet) || parseEmptyStatement()) {
		skipWhitespace();
	}

	// stream should end here
	if (tokenizer->getTokenType() != Token::EOS) {
		throw new ParseException(tokenizer->getToken(), "end of input");
	}
}

void Parser::skipWhitespace() {
	while (
		tokenizer->getTokenType() == Token::WHITESPACE ||
		tokenizer->getTokenType() == Token::COMMENT) {
		tokenizer->readNextToken();
	}
}

bool Parser::parseWhitespace(TokenList &tokens) {

	while (
		tokenizer->getTokenType() == Token::WHITESPACE ||
		tokenizer->getTokenType() == Token::COMMENT) {

		if (tokenizer->getTokenType() == Token::WHITESPACE) {
			tokens.push_back(Token::BUILTIN_SPACE);
		}

		tokenizer->readNextToken();
	}

	return true;
}

bool Parser::parseEmptyStatement() {

	Token *t;

	// empty statement
	if (tokenizer->getTokenType() == Token::DELIMITER) {

		t = &tokenizer->getToken();

		cerr << t->source << ": Line " << t->line << ", Column"
			 << t->column
			 << " Warning: Semicolon without statement." << endl;

		tokenizer->readNextToken();

		return true;

	} else {
		return false;
	}
}

bool Parser::parseStatement(Stylesheet &stylesheet) {

	Ruleset *ruleset = parseRuleset(stylesheet);
	if (ruleset != NULL) {
		return true;
	}

	MediaQuery *query = parseMediaQuery(stylesheet);
	if (query != NULL) {
		return true;
	}

	AtRule *atrule = parseAtRule(stylesheet);
	if (atrule != NULL) {
		return true;
	}

	return false;
}

MediaQuery *Parser::parseMediaQuery(Stylesheet &stylesheet) {

	TokenList selector;
	MediaQuery *query;

	if (tokenizer->getTokenType() != Token::AT ||
		tokenizer->getToken() != "@media")
		return NULL;

	selector.push_back(tokenizer->getToken());

	tokenizer->readNextToken();
	skipWhitespace();

	parseSelector(selector);

	query = stylesheet.createMediaQuery(selector);

	if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
		throw new ParseException(tokenizer->getToken(), "{");
	}

	tokenizer->readNextToken();

	skipWhitespace();
	while (parseStatement(*query)) {
		skipWhitespace();
	}

	if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
		throw new ParseException(
			tokenizer->getToken(),
			"end of media query block ('}')"
		);
	}

	tokenizer->readNextToken();
	skipWhitespace();

	return query;
}

AtRule *Parser::parseAtRule(Stylesheet &stylesheet) {

	AtRule *atrule;

	if (tokenizer->getTokenType() != Token::AT) {
		return NULL;
	}

	atrule = stylesheet.createAtRule(tokenizer->getToken());
	tokenizer->readNextToken();
	skipWhitespace();

	while (parseAny(atrule->getRule()));

	if (parseBlock(atrule->getRule()) == false) {

		if (tokenizer->getTokenType() != Token::DELIMITER) {
			throw new ParseException(
				tokenizer->getToken(),
				"delimiter (';') at end of @-rule"
			);
		}

		tokenizer->readNextToken();
		skipWhitespace();
	}

	return atrule;
}

bool Parser::parseBlock(TokenList &tokens) {

	if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
		return false;
	}

	tokens.push_back(tokenizer->getToken());
	tokenizer->readNextToken();
	skipWhitespace();

	while (true) {

		if ((parseAny(tokens) || parseBlock(tokens)) == false) {

			if (tokenizer->getTokenType() == Token::AT) {

				tokens.push_back(tokenizer->getToken());
				tokenizer->readNextToken();
				parseWhitespace(tokens);

			} else if (tokenizer->getTokenType() == Token::VAR) {

				tokens.push_back(tokenizer->getToken());
				tokenizer->readNextToken();
				parseWhitespace(tokens);

			} else if (tokenizer->getTokenType() == Token::DELIMITER) {

				tokens.push_back(tokenizer->getToken());
				tokenizer->readNextToken();
				skipWhitespace();

			} else {
				break;
			}
		}
	}

	if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
		throw new ParseException(tokenizer->getToken(), "end of block ('}')");
	}

	tokens.push_back(tokenizer->getToken());
	tokenizer->readNextToken();
	skipWhitespace();

	return true;
}

Ruleset *Parser::parseRuleset(Stylesheet &stylesheet) {

	Ruleset *ruleset;
	TokenList selector;
	Selector *s;

	if (parseSelector(selector) == false) {

		if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
			return NULL;
		}

	} else if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
		throw new ParseException(
			tokenizer->getToken(),
			"a declaration block ('{...}')"
		);
	}

	tokenizer->readNextToken();

	s = new Selector();
	selectorParser.parse(selector, *s);
	ruleset = stylesheet.createRuleset(*s);

	skipWhitespace();
	parseDeclaration(*ruleset);

	while (tokenizer->getTokenType() == Token::DELIMITER) {
		tokenizer->readNextToken();
		skipWhitespace();
		parseDeclaration(*ruleset);
	}

	if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
		throw new ParseException(
			tokenizer->getToken(),
			"end of declaration block ('}')"
		);
	}

	tokenizer->readNextToken();
	skipWhitespace();

	return ruleset;
}

bool Parser::parseSelector(TokenList &selector) {

	if (parseAny(selector) == false) {
		return false;
	}

	while (parseAny(selector));

	// delete trailing whitespace
	selector.rtrim();
	return true;
}

Declaration *Parser::parseDeclaration(Ruleset &ruleset) {

	Declaration *declaration = NULL;
	TokenList property;
	Token keyword;

	if (parseProperty(property) == false) {
		return NULL;
	}

	skipWhitespace();

	keyword = property.front();
	keyword.assign(property.toString());

	declaration = ruleset.createDeclaration(keyword);

	if (tokenizer->getTokenType() != Token::COLON) {
		throw new ParseException(
			tokenizer->getToken(),
	 		"colon following property(':')"
		);
	}

	tokenizer->readNextToken();
	skipWhitespace();

	if (parseValue(declaration->getValue()) == false) {
		throw new ParseException(
			tokenizer->getToken(),
			"value for property"
		);
	}

	return declaration;
}

bool Parser::parseProperty(TokenList &tokens) {

	if (tokenizer->getToken() == "*") {
		// suppor for an IE Hack
		tokens.push_back(tokenizer->getToken());
		tokenizer->readNextToken();

		if (tokenizer->getTokenType() == Token::IDENTIFIER) {
			tokens.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
		}

		return true;

	} else if (tokenizer->getTokenType() != Token::IDENTIFIER) {
		return false;
	}

	tokens.push_back(tokenizer->getToken());
	tokenizer->readNextToken();

	return true;
}

bool Parser::parseValue(TokenList &value) {

	if (parseAny(value) || parseBlock(value)) {

		// Nothing

	} else if (tokenizer->getTokenType() == Token::VAR) {

		value.push_back(tokenizer->getToken());
		tokenizer->readNextToken();
		parseWhitespace(value);

	} else {
		return false;
	}

	while (true) {

		if (parseAny(value) || parseBlock(value)) {

			// Nothing

		} else if (tokenizer->getTokenType() == Token::VAR) {

			value.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			parseWhitespace(value);

		} else {
			return true;
		}
	}
}

bool Parser::parseAny(TokenList &tokens) {
	switch (tokenizer->getTokenType()) {
		case Token::NUMBER:
		case Token::PERCENTAGE:
		case Token::DIMENSION:
		case Token::STRING:
		case Token::URL:
		case Token::HASH:
		case Token::UNICODE_RANGE:
		case Token::INCLUDES:
		case Token::DASHMATCH:
		case Token::COLON:
		case Token::OTHER:
			tokens.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			break;

		case Token::PAREN_OPEN:
			tokens.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			skipWhitespace();

			while (parseAny(tokens) || parseUnused(tokens)) {
			}
			if (tokenizer->getTokenType() != Token::PAREN_CLOSED) {
				throw new ParseException(tokenizer->getToken(),
										 "closing parenthesis (')')");
			}
			tokens.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			break;

		case Token::IDENTIFIER:
			tokens.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			break;

		case Token::BRACE_OPEN:
			tokens.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			skipWhitespace();
			while (parseAny(tokens) || parseUnused(tokens)) {
			}
			if (tokenizer->getTokenType() != Token::BRACE_CLOSED) {
				throw new ParseException(tokenizer->getToken(), "closing brace (']')");
			}
			tokens.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			break;

		default:
			return false;
	}

	parseWhitespace(tokens);

	return true;
}

bool Parser::parseUnused(TokenList &tokens) {

	if (parseBlock(tokens)) {

	} else if (tokenizer->getTokenType() == Token::AT) {

		tokens.push_back(tokenizer->getToken());
		tokenizer->readNextToken();
		parseWhitespace(tokens);

	} else if (tokenizer->getTokenType() == Token::VAR) {

		tokens.push_back(tokenizer->getToken());
		tokenizer->readNextToken();
		parseWhitespace(tokens);

	} else if (tokenizer->getTokenType() == Token::DELIMITER) {

		tokens.push_back(tokenizer->getToken());
		tokenizer->readNextToken();
		skipWhitespace();

	} else {
		return false;
	}

	return true;
}

}
