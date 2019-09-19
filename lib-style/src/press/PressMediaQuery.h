#ifndef __PressMediaQuery_h__
#define __PressMediaQuery_h__

#include <list>

#include "Selector.h"
#include "Stylesheet.h"
#include "StylesheetStatement.h"
#include "Writer.h"
#include "PressFunction.h"
#include "PressRuleset.h"
#include "PressStylesheet.h"
#include "PressMixin.h"
#include "ProcessingContext.h"

namespace Press {

class PressMediaQuery : public PressStylesheet, public StylesheetStatement {

private:
	TokenList selector;
	const PressStylesheet *parent;

public:
	PressMediaQuery(const TokenList &selector, const PressStylesheet &parent);
	virtual ~PressMediaQuery();
	TokenList &getSelector();
	const TokenList &getSelector() const;
	void setSelector(const TokenList &s);
	const PressStylesheet &getPressStylesheet() const;
	virtual void getFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, const ProcessingContext &context) const;
	virtual const TokenList *getVariable(const std::string &key, const ProcessingContext &context) const;
	virtual void process(Stylesheet &s, void *context) const;
	virtual void write(Writer &writer) const;
};

}

#endif
