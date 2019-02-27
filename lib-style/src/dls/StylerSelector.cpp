#include "StylerRuleset.h"

StylerSelector::StylerSelector() : Selector() {
	_unlimitedArguments = false;
	_needsArguments = false;
}

StylerSelector::~StylerSelector() {
}


const TokenList *StylerSelector::getDefault(const std::string &keyword) const {
	std::list<std::string>::const_iterator pit = parameters.begin();
	std::list<TokenList>::const_iterator dit = defaults.begin();

	for (; pit != parameters.end(); pit++, dit++) {
		if ((*pit) == keyword)
			return &(*dit);
	}
	return NULL;
}

const std::list<std::string> &StylerSelector::getParameters() const {
	return parameters;
}

const std::list<TokenList> &StylerSelector::getConditions() const {
	return conditions;
}

void StylerSelector::addExtension(StylerExtension &extension) {
	extensions.push_back(extension);
}

const std::list<StylerExtension> &StylerSelector::getExtensions() const {
	return extensions;
}

void StylerSelector::addParameter(Token &keyword, TokenList &value) {
	parameters.push_back(keyword);
	defaults.push_back(value);
}

void StylerSelector::setUnlimitedArguments(bool b) {
	_unlimitedArguments = b;
}

void StylerSelector::setUnlimitedArguments(bool b, Token restKeyword) {
	_unlimitedArguments = b;
	restIdentifier = restKeyword;
}

void StylerSelector::setNeedsArguments(bool b) {
	_needsArguments = b;
}

void StylerSelector::eraseArguments() {
	parameters.clear();
	defaults.clear();
	_unlimitedArguments = false;
	restIdentifier = "";
	_needsArguments = false;
}

void StylerSelector::addCondition(TokenList &condition) {
	conditions.push_back(condition);
}

bool StylerSelector::matchArguments(const StylerMixinArguments &args) const {
	std::list<std::string>::const_iterator p_it = parameters.begin();
	std::list<TokenList>::const_iterator d_it = defaults.begin();
	size_t pos = 0;
	const TokenList *a;

	for (; p_it != parameters.end(); p_it++, d_it++) {
		if ((a = args.get(*p_it)) == NULL &&
			(a = args.get(pos++)) == NULL &&
			(a = &(*d_it))->empty()) {
			return false;
		} else if ((*p_it)[0] != '@' &&
				   a->size() == 1 &&
				   (*p_it) != a->front()) {
			return false;
		}
	}
	return (pos >= args.count() || unlimitedArguments());
}

bool StylerSelector::needsArguments() const {
	return _needsArguments;
}

bool StylerSelector::unlimitedArguments() const {
	return _unlimitedArguments;
}

string StylerSelector::getRestIdentifier() const {
	return restIdentifier;
}

