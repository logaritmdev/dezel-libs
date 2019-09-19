#ifndef __PressPressParser_h__
#define __PressPressParser_h__

#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "Parser.h"
#include "Tokenizer.h"
#include "PressSelectorParser.h"
#include "PressMediaQuery.h"
#include "PressRuleset.h"
#include "PressStylesheet.h"
#include "PressMediaQueryRuleset.h"
#include "Stylesheet.h"
#include "Token.h"
#include "TokenList.h"
#include "PressTokenizer.h"

namespace Press {

class PressParser : public Parser {

public:
	static const unsigned int
		IMPORT_REFERENCE = 1,
		IMPORT_INLINE = 2,
		IMPORT_LESS = 4,
		IMPORT_CSS = 8,
		IMPORT_ONCE = 16,
		IMPORT_MULTIPLE = 32,
		IMPORT_OPTIONAL = 64;

	std::list<const char* > *includePaths;

	PressParser(Tokenizer &tokenizer, std::list<const char* > &source_files) : Parser(tokenizer), sources(source_files), reference(false) { }
	PressParser(Tokenizer &tokenizer, std::list<const char* > &source_files, bool isreference) : Parser(tokenizer), sources(source_files), reference(isreference) { }
	virtual ~PressParser() { }
	virtual void parseStylesheet(PressStylesheet &stylesheet);
	void parseStylesheet(PressRuleset &ruleset);

protected:
	std::list<const char* > &sources;
	bool reference;
	PressSelectorParser PressSelectorParser;
	virtual void skipWhitespace();
	bool parseStatement(Stylesheet &stylesheet);
	bool parseAtRule(PressStylesheet &stylesheet);
	bool parseAtRule(PressRuleset &ruleset);
	bool parseAtRuleValue(TokenList &rule);
	bool parseVariable(TokenList &value);
	bool parseVariable(std::string &keyword, TokenList &value);
	bool parseSelector(TokenList &selector);
	bool parseSelectorVariable(TokenList &selector);
	bool parseRuleset(PressStylesheet &parent, TokenList &selector);
	bool parseRuleset(PressRuleset &parent, TokenList &selector);
	void parseMediaQueryRuleset(Token &mediatoken, PressRuleset &parent);
	bool parsePropertyVariable(TokenList &selector);
	bool parseRulesetStatement(PressRuleset &parent);
	bool parseComment(PressRuleset &ruleset);
	bool parseExtension(TokenList &statement, PressRuleset &ruleset);
	bool parseDeclaration(TokenList &tokens, size_t property_i, PressRuleset &ruleset);
	bool parseMixin(TokenList &tokens, PressStylesheet &stylesheet);
	bool parseMixin(TokenList &tokens, PressRuleset &ruleset);
	void parseMixinArguments(TokenList::const_iterator &i, const TokenList &tokens, PressMixin &mixin);

	void parseList(std::list<TokenList *> *list, TokenList *tokens);
	bool parseImportStatement(TokenList &statement, PressStylesheet &stylesheet);
	bool parseImportStatement(TokenList &statement, PressRuleset &ruleset);
	unsigned int parseImportDirective(Token &t);
	bool importFile(Token uri, PressStylesheet &stylesheet, unsigned int directive);
	bool importFile(Token uri, PressRuleset &ruleset, unsigned int directive);
	void parsePressMediaQuery(Token &mediatoken, PressStylesheet &stylesheet);

private:
	TokenList *processValue(TokenList *value);
	std::list<TokenList *> *processArguments(TokenList *arguments);
	bool findFile(Token &uri, std::string &filename);
	bool parseRuleset(TokenList &selector, PressStylesheet *stylesheet, PressRuleset *parentRuleset);
	bool parseMixin(TokenList &tokens, PressRuleset *parent_r, PressStylesheet *parent_s);
	bool parseAtRule(PressStylesheet *stylesheet, PressRuleset *ruleset);
	bool parseImportStatement(TokenList &statement, PressStylesheet *stylesheet, PressRuleset *ruleset);
	bool importFile(Token uri, PressStylesheet *stylesheet, PressRuleset *ruleset, unsigned int directive);
};

}

#endif
