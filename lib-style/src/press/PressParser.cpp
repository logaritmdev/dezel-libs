#include "PressParser.h"

namespace Press {

void PressParser::parseStylesheet(PressStylesheet &stylesheet) {
	Parser::parseStylesheet(stylesheet);
}

void PressParser::parseStylesheet(PressRuleset &ruleset) {
	while (parseRulesetStatement(ruleset));
}

void PressParser::skipWhitespace() {
	while (
		tokenizer->getTokenType() == Token::WHITESPACE || (
		tokenizer->getTokenType() == Token::COMMENT &&
		tokenizer->getToken().compare(0, 2, "//") == 0)) {
		tokenizer->readNextToken();
	}
}

bool PressParser::parseStatement(Stylesheet &ss) {

	Token token;
	TokenList tokens;

	PressStylesheet *stylesheet = reinterpret_cast<PressStylesheet *>(&ss);

	if (tokenizer->getTokenType() == Token::VAR) {

		token = tokenizer->getToken();
		tokenizer->readNextToken();
		skipWhitespace();

		if (parseVariable(tokens)) {

			if (stylesheet) {
				stylesheet->putVariable(token, tokens);
			}

			return true;
		}

		return false;
	}

	if (tokenizer->getTokenType() == Token::COMMENT) {
		Comment* comment = stylesheet->createComment();
		comment->setComment(tokenizer->getToken());
		tokenizer->readNextToken();
		skipWhitespace();
		return true;
	}

	if (parseSelector(tokens) && tokens.empty() == false) {

		if (parseRuleset(*stylesheet, tokens)) {
			return true;
		}

		// Parameter mixin in the root. Inserts nested rules but no
		// declarations.
		if (parseMixin(tokens, *stylesheet)) {

			if (tokenizer->getTokenType() == Token::DELIMITER) {
				tokenizer->readNextToken();
				skipWhitespace();
			}

			return true;
		}

		throw new ParseException(
			tokenizer->getToken(),
			"a declaration block ('{...}') following selector"
		);

	} else {
		return parseAtRule(*stylesheet);
	}
}

bool PressParser::parseAtRule(PressStylesheet *stylesheet, PressRuleset *ruleset) {

	if (tokenizer->getTokenType() != Token::AT) {
		return false;
	}

	Token token;
	TokenList rule;
	TokenList value;

	AtRule* atrule = NULL;

	token = tokenizer->getToken();
	tokenizer->readNextToken();
	Parser::skipWhitespace();

	if (token == "@media") {

		if (stylesheet != NULL) {
			parsePressMediaQuery(token, *stylesheet);
		} else {
			parseMediaQueryRuleset(token, *ruleset);
		}

	} else {

		parseAtRuleValue(rule);

		// parse import
		if (token == "@import" && rule.size() > 0) {
			if (parseImportStatement(rule, stylesheet, ruleset)) {
				return true;
			}
		}

		if (stylesheet != NULL) {
			atrule = stylesheet->createPressAtRule(token);
		} else {
			atrule = ruleset->createPressAtRule(token);
		}

		atrule->setReference(reference);
		atrule->setRule(rule);
	}

	return true;
}

bool PressParser::parseAtRule(PressStylesheet &stylesheet) {
	return parseAtRule(&stylesheet, NULL);
}

bool PressParser::parseAtRule(PressRuleset &ruleset) {
	return parseAtRule(NULL, &ruleset);
}

bool PressParser::parseAtRuleValue(TokenList &rule) {

	while (parseAny(rule));

	if (parseBlock(rule) == false) {

		if (tokenizer->getTokenType() != Token::DELIMITER) {
			throw new ParseException(
				tokenizer->getToken(),
				"delimiter (';') at end of @-rule"
			);
		}

		tokenizer->readNextToken();

		skipWhitespace();
	}

	return true;
}

bool PressParser::parseVariable(TokenList &value) {

	if (tokenizer->getTokenType() != Token::COLON) {
		return false;
	}

	tokenizer->readNextToken();
	skipWhitespace();

	if (parseValue(value) == false || value.size() == 0) {
		throw new ParseException(
			tokenizer->getToken(),
			"value for variable"
		);
	}

	if (tokenizer->getTokenType() != Token::DELIMITER) {
		throw new ParseException(
			tokenizer->getToken(),
			"delimiter (';') at end of variable declaration"
		);
	}

	tokenizer->readNextToken();
	skipWhitespace();

	return true;
}

bool PressParser::parseSelector(TokenList &tokens) {

	if (parseAny(tokens) == false) {
		return false;
	}

	while (parseAny(tokens) || parseSelectorVariable(tokens));

	// delete trailing whitespace
	tokens.rtrim();
	return true;
}

bool PressParser::parseSelectorVariable(TokenList &selector) {
	return false;
}

bool PressParser::parseRuleset(TokenList &tokens, PressStylesheet *stylesheet, PressRuleset *parentRuleset) {

	PressRuleset* ruleset;
	PressSelector* selector;

	if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
		return false;
	}

	tokenizer->readNextToken();
	skipWhitespace();

	selector = new PressSelector();
	PressSelectorParser.parse(tokens, *selector);

	// Create the ruleset and parse ruleset statements.
	if (parentRuleset == NULL) {
		ruleset = stylesheet->createPressRuleset(*selector);
	} else {
		ruleset = parentRuleset->createNestedRule(*selector);
	}

	ruleset->setReference(reference);

	while (parseRulesetStatement(*ruleset));

	if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
		throw new ParseException(
			tokenizer->getToken(),
			"end of declaration block ('}')"
		);
	}

	tokenizer->readNextToken();
	skipWhitespace();

	return true;
}

bool PressParser::parseRuleset(PressStylesheet &parent, TokenList &selector) {
	return parseRuleset(selector, &parent, NULL);
}

bool PressParser::parseRuleset(PressRuleset &parent, TokenList &selector) {
	return parseRuleset(selector, NULL, &parent);
}

void PressParser::parseMediaQueryRuleset(Token &mediatoken, PressRuleset &parent) {

	PressMediaQueryRuleset *query;
	TokenList selector;

	selector.push_back(mediatoken);
	selector.push_back(Token::BUILTIN_SPACE);

	Parser::skipWhitespace();

	while (parseAny(selector) || tokenizer->getTokenType() == Token::AT) {
		if (tokenizer->getTokenType() == Token::AT) {
			selector.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			parseWhitespace(selector);
		}
	}

	query = parent.createMediaQuery(selector);
	query->setReference(reference);

	if (tokenizer->getTokenType() != Token::BRACKET_OPEN) {
		throw new ParseException(tokenizer->getToken(), "{");
	}

	tokenizer->readNextToken();
	skipWhitespace();

	while (parseRulesetStatement(*query));

	if (tokenizer->getTokenType() != Token::BRACKET_CLOSED) {
		throw new ParseException(
			tokenizer->getToken(),
		 	"end of media query block ('}')"
		);
	}

	tokenizer->readNextToken();
	skipWhitespace();
}

bool PressParser::parsePropertyVariable(TokenList &selector) {

	Token variable = tokenizer->getToken();

	if (tokenizer->getTokenType() != Token::OTHER || variable != "$") {
		return false;
	}

	if (tokenizer->readNextToken() != Token::BRACKET_OPEN) {
		throw new ParseException(
			tokenizer->getToken(),
			"Opening bracket following @"
		);
	 }

	variable.append(tokenizer->getToken());

	if (tokenizer->readNextToken() != Token::IDENTIFIER) {
		throw new ParseException(
			tokenizer->getToken(),
			"Variable inside selector (e.g.: @{identifier})"
	 	);
	}

	variable.append(tokenizer->getToken());

	if (tokenizer->readNextToken() != Token::BRACKET_CLOSED) {
		throw new ParseException(
			tokenizer->getToken(),
			"Closing bracket after variable."
		);
	 }

	variable.append(tokenizer->getToken());
	tokenizer->readNextToken();

	selector.push_back(variable);
	parseWhitespace(selector);

	return true;
}

bool PressParser::parseRulesetStatement(PressRuleset &ruleset) {

	TokenList tokens;
	size_t property_i;

	if (parseComment(ruleset)) {
		return true;
	}

	if (parseAtRule(ruleset)) {
		return true;
	}

	while (parseProperty(tokens) || parsePropertyVariable(tokens));

	// merge properties have a '+' or '+_' suffix
	if (tokenizer->getToken() == "+") {

		tokens.push_back(tokenizer->getToken());
		tokenizer->readNextToken();

		if (tokenizer->getToken() == "_") {
			tokens.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
		}
	}

	property_i = tokens.size();

	parseWhitespace(tokens);
	if (tokenizer->getTokenType() != Token::COLON) {
		property_i = 0;
	}

	parseSelector(tokens);
	tokens.trim();

	if (tokens.empty()) {
		return false;
	}

	if (parseRuleset(ruleset, tokens)) {
		return true;
	}

	parseValue(tokens);

	if (parseExtension(tokens, ruleset) ||
		parseDeclaration(tokens, property_i, ruleset)) {
	} else {
		parseMixin(tokens, ruleset);
	}

	if (tokenizer->getTokenType() == Token::DELIMITER) {
		tokenizer->readNextToken();
		skipWhitespace();
	}

	return true;
}

bool PressParser::parseComment(PressRuleset &ruleset) {

	if (tokenizer->getTokenType() != Token::COMMENT) {
		return false;
	}

	Comment* comment = ruleset.createComment();
	comment->setComment(tokenizer->getToken());
	tokenizer->readNextToken();
	skipWhitespace();

	return true;
}

bool PressParser::parseExtension(TokenList &statement, PressRuleset &ruleset) {

	TokenList::iterator i = statement.begin();
	int parentheses = 1;
	PressExtension extension;
	TokenList target;

	if (statement.size() <= 5 ||
		(*i) != "&" ||
		(*++i).type != Token::COLON ||
		(*++i).type != Token::IDENTIFIER ||
		(*i) != "extend" ||
		(*++i).type != Token::PAREN_OPEN)
		return false;

	i++;
	for (; i != statement.end() && parentheses > 0; i++) {
		switch ((*i).type) {
			case Token::PAREN_OPEN:
				parentheses++;
				break;
			case Token::PAREN_CLOSED:
				parentheses--;
				break;
			default:
				break;
		}
		if (parentheses > 0)
			target.push_back(*i);
	}

	if (parentheses > 0) {
		throw new ParseException("end of statement",
								 ")",
								 statement.front().line,
								 statement.front().column,
								 statement.front().source);
	}

	if (!target.empty() && target.back() == "all") {
		extension.setAll(true);
		target.pop_back();
		target.rtrim();
	}
	selectorParser.parse(target, extension.getTarget());

	ruleset.addExtension(extension);

	return true;
}

bool PressParser::parseDeclaration(TokenList &tokens,
	size_t property_i,
	PressRuleset &ruleset) {
	PressDeclaration *d;
	TokenList::iterator i;
	TokenList property;
	Token keyword;

	if (property_i == 0 ||
		tokens.front().type == Token::HASH ||
		tokens.front() == ".") {
		return false;
	}

	d = ruleset.createPressDeclaration();

	i = tokens.begin();
	std::advance(i, property_i);

	property.insert(property.begin(), tokens.begin(), i);
	keyword = property.front();
	keyword.assign(property.toString());
	d->setProperty(keyword);

	while (i != tokens.end() && (*i).type == Token::WHITESPACE)
		i++;

	if (i != tokens.end() && (*i).type == Token::COLON)
		i++;

	while (i != tokens.end() && (*i).type == Token::WHITESPACE)
		i++;

	d->getValue().insert(d->getValue().begin(), i, tokens.end());

	return true;
}

bool PressParser::parseMixin(TokenList &tokens, PressRuleset &parent) {
	return parseMixin(tokens, &parent, NULL);
}

bool PressParser::parseMixin(TokenList &tokens, PressStylesheet &parent) {
	return parseMixin(tokens, NULL, &parent);
}

bool PressParser::parseMixin(TokenList &tokens, PressRuleset *parent_r, PressStylesheet *parent_s) {

	TokenList::const_iterator i = tokens.begin();
	PressMixin *mixin;
	TokenList name;

	for (; i != tokens.end() && (*i).type != Token::PAREN_OPEN; i++) {
		name.push_back(*i);
	}

	name.rtrim();

	if (parent_r != NULL) {
		mixin = parent_r->createMixin(name);
	} else {
		mixin = parent_s->createMixin(name);
	}

	mixin->setReference(reference);
	parseMixinArguments(i, tokens, *mixin);

	while (i != tokens.end() && (*i).type == Token::WHITESPACE)
		i++;

	if (i != tokens.end() && *i == "!") {
		if (++i != tokens.end() && *i == "important") {
			mixin->setImportant(true);
			i++;
		} else
			i--;
	}

	if (i != tokens.end())
		throw new ParseException(*i, "end of mixin statement");
	return true;
}

void PressParser::parseMixinArguments(TokenList::const_iterator &i,
	const TokenList &tokens,
	PressMixin &mixin) {
	TokenList::const_iterator j;
	std::string delimiter = ",";

	TokenList argument;
	size_t nestedParenthesis = 0;
	std::string argName;

	if (i != tokens.end() && (*i).type == Token::PAREN_OPEN) {
		i++;
	}

	// if a ';' token occurs then that is the delimiter instead of the ','.
	if (tokens.contains(Token::DELIMITER, ";")) {
		delimiter = ";";
	}

	while (i != tokens.end() && (*i).type != Token::PAREN_CLOSED) {
		while (i != tokens.end() && (*i).type == Token::WHITESPACE) {
			i++;
		}

		// atkeyword followed by comma is an argument name
		if ((*i).type == Token::VAR) {
			argName = (*i);
			i++;
			if (i != tokens.end() && (*i).type == Token::COLON) {
				i++;
			} else {
				argName = "";
				i--;
			}
		}
		// parse until delimiter, or end of argument list.
		while (i != tokens.end() &&
			   (nestedParenthesis > 0 ||
				((*i) != delimiter && (*i).type != Token::PAREN_CLOSED))) {
			if ((*i).type == Token::PAREN_OPEN)
				nestedParenthesis++;

			if ((*i).type == Token::PAREN_CLOSED)
				nestedParenthesis--;

			argument.push_back(*i);

			i++;
		}

		if (*i == delimiter)
			i++;

		if (argName == "")
			mixin.arguments.add(argument);
		else {
			mixin.arguments.add(argName, argument);
			argName = "";
		}
		argument.clear();
	}

	if (i != tokens.end())
		i++;
}

bool PressParser::parseImportStatement(TokenList &statement,
	PressStylesheet *stylesheet,
	PressRuleset *ruleset) {
	unsigned int directive = 0;

	// parse directives and strip from statement (the statement becomes a valid
	// css import statement.)
	if (statement.size() >= 4 && statement.front().type == Token::PAREN_OPEN) {
		statement.pop_front();
		statement.ltrim();

		directive = parseImportDirective(statement.front());
		statement.pop_front();
		statement.ltrim();

		while (statement.size() > 0 && statement.front() == ",") {
			statement.pop_front();
			statement.ltrim();

			directive |= parseImportDirective(statement.front());
			statement.pop_front();
			statement.ltrim();
		}

		if (statement.size() > 0 && statement.front().type != Token::PAREN_CLOSED) {
			throw new ParseException(statement, ")");
		} else {
			statement.pop_front();
			statement.ltrim();
		}
	}

	if (statement.size() > 0 && (statement.front().type == Token::URL ||
								 statement.front().type == Token::STRING)) {
		return importFile(statement.front(), stylesheet, ruleset, directive);

	} else
		throw new ParseException(statement,
								 "A string with the file path, "
									 "or an import directive.");
}

bool PressParser::parseImportStatement(TokenList &statement,
	PressStylesheet &stylesheet) {
	return parseImportStatement(statement, &stylesheet, NULL);
}

bool PressParser::parseImportStatement(TokenList &statement,
	PressRuleset &ruleset) {
	return parseImportStatement(statement, NULL, &ruleset);
}

unsigned int PressParser::parseImportDirective(Token &t) {
	if (t.type != Token::IDENTIFIER)
		throw new ParseException(t, "an import directive.");
	if (t == "reference")
		return IMPORT_REFERENCE;
	else if (t == "inline")
		return IMPORT_INLINE;
	else if (t == "less")
		return IMPORT_LESS;
	else if (t == "css")
		return IMPORT_CSS;
	else if (t == "once")
		return IMPORT_ONCE;
	else if (t == "multiple")
		return IMPORT_MULTIPLE;
	else if (t == "optional")
		return IMPORT_OPTIONAL;
	else
		throw new ParseException(t,
								 "valid import directive: reference, "
									 "inline, less, css, once, multiple or optional");
}

bool PressParser::importFile(Token uri,
	PressStylesheet *stylesheet,
	PressRuleset *ruleset,
	unsigned int directive) {
	size_t pathend;
	size_t extension_pos;
	std::list<const char* >::iterator i;
	std::string relative_filename;
	char* relative_filename_cpy;
	std::string extension;

	if (uri.type == Token::URL) {
		uri = uri.getUrlString();

	} else if (uri.type == Token::STRING) {
		uri.removeQuotes();
	}

	// no remote includes
	if (uri.compare(0, 7, "http://") == 0 || uri.compare(0, 8, "https://") == 0)
		return false;

	pathend = uri.rfind('?');
	if (pathend == std::string::npos)
		pathend = uri.size();

	extension_pos = uri.rfind('.', pathend);
	if (extension_pos == std::string::npos) {
		uri.insert(pathend, ".less");
		pathend += 5;
		extension = "less";
	} else
		extension = uri.substr(extension_pos + 1, pathend);

	// don't import css, unless specified with directive
	// don't import if css directive is given
	if ((extension == "css" && !(directive & IMPORT_LESS)) ||
		(directive & IMPORT_CSS)) {
		return false;
	}

	if (!findFile(uri, relative_filename)) {
		if (directive & IMPORT_OPTIONAL)
			return true;
		else {
			throw new ParseException(
				uri, "existing file", uri.line, uri.column, uri.source);
		}
	}

	if (!(directive & IMPORT_MULTIPLE)) {
		// check if the file has already been imported.
		for (i = sources.begin(); i != sources.end(); i++) {
			if (relative_filename == (*i))
				return true;
		}
	}

	ifstream in(relative_filename.c_str());

	relative_filename_cpy = new char[relative_filename.length() + 1];
	std::strcpy(relative_filename_cpy, relative_filename.c_str());

	sources.push_back(relative_filename_cpy);
	PressTokenizer tokenizer(in, relative_filename_cpy);
	PressParser parser(tokenizer, sources, (directive & IMPORT_REFERENCE));

	parser.includePaths = includePaths;

	if (stylesheet != NULL)
		parser.parseStylesheet(*stylesheet);
	else
		parser.parseStylesheet(*ruleset);
	in.close();
	return true;
}

bool PressParser::importFile(Token uri,
	PressStylesheet &stylesheet,
	unsigned int directive) {
	return importFile(uri, &stylesheet, NULL, directive);
}

bool PressParser::importFile(Token uri,
	PressRuleset &ruleset,
	unsigned int directive) {
	return importFile(uri, NULL, &ruleset, directive);
}

bool PressParser::findFile(Token &uri, std::string &filename) {
	ifstream *in;
	size_t pos;
	std::string source;
	std::list<const char* >::iterator i;

	source = uri.source;
	pos = source.find_last_of("/\\");

	// if the current stylesheet is outside of the current working
	//  directory then add the directory to the filename.
	if (pos != std::string::npos) {
		filename.append(source.substr(0, pos + 1));
	}
	filename.append(uri);

	in = new ifstream(filename.c_str());
	if (in->good()) {
		in->close();
		return true;
	}

	if (includePaths != NULL) {
		for (i = includePaths->begin(); i != includePaths->end(); i++) {
			filename.clear();

			filename.append((*i));
			filename.append(uri);

			in = new ifstream(filename.c_str());
			if (in->good()) {
				in->close();
				return true;
			}
		}
	}
	return false;
}

void PressParser::parsePressMediaQuery(Token &mediatoken, PressStylesheet &stylesheet) {

	PressMediaQuery *query;
	TokenList selector;

	selector.push_back(mediatoken);
	selector.push_back(Token::BUILTIN_SPACE);

	Parser::skipWhitespace();

	while (parseAny(selector) ||
		   tokenizer->getTokenType() == Token::AT) {
		if (tokenizer->getTokenType() == Token::AT) {
			selector.push_back(tokenizer->getToken());
			tokenizer->readNextToken();
			parseWhitespace(selector);
		}
	}

	query = stylesheet.createPressMediaQuery(selector);
	query->setReference(reference);

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
}

}
