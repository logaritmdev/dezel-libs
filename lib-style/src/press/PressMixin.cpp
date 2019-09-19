#include "PressMixin.h"
#include "PressRuleset.h"
#include "PressStylesheet.h"
#include "PressMixinException.h"

namespace Press {

PressMixin::PressMixin(const TokenList &name, const PressStylesheet &parent)
	: pressStylesheet(&parent), pressRuleset(NULL), important(false), name(name) {
}

PressMixin::PressMixin(const TokenList &name, const PressRuleset &parent)
	: pressStylesheet(NULL), pressRuleset(&parent), important(false), name(name) {
}

PressMixin::~PressMixin() {
}


bool PressMixin::call(ProcessingContext &context,
	Ruleset *r_target,
	Stylesheet *s_target) const {

	std::list<const PressFunction *>::iterator i;
	std::list<const PressFunction *> functionList;
	const PressFunction *function;
	bool success = false;

	PressMixinArguments arguments_p;

	context.getFunctions(functionList, *this);

	if (functionList.empty()) {
		throw new PressMixinException(*this);
	}

	arguments_p = arguments;
	arguments_p.process(context);

	for (i = functionList.begin(); i != functionList.end(); i++) {
		function = *i;

		if (function->getPressSelector().needsArguments() ||
			!context.isInStack(*function)) {
			context.pushMixinCall(*function, false, isImportant());

			if (r_target != NULL)
				success = function->call(arguments_p, *r_target, context) || success;
			else
				success = function->call(arguments_p, *s_target, context) || success;

			context.popMixinCall();
		}
	}

	// if no functions matched, try it with 'default()' set to true.
	if (!success) {
		for (i = functionList.begin(); i != functionList.end(); i++) {
			function = *i;

			if (function->getPressSelector().needsArguments() ||
				!context.isInStack(*function)) {
				context.pushMixinCall(*function, false, isImportant());

				if (r_target != NULL)
					function->call(arguments_p, *r_target, context, true);
				else
					function->call(arguments_p, *s_target, context, true);

				context.popMixinCall();
			}
		}
	}

	return true;
}

void PressMixin::setImportant(bool b) {
	important = b;
}

bool PressMixin::isImportant() const {
	return important;
}

const PressStylesheet *PressMixin::getPressStylesheet() const {
	return pressStylesheet;
}

const PressRuleset *PressMixin::getPressRuleset() const {
	return pressRuleset;
}

void PressMixin::process(Stylesheet &s, void *context) const {
	call(*((ProcessingContext *) context), NULL, &s);
}

void PressMixin::process(Ruleset &r, void *context) const {
	call(*((ProcessingContext *) context), &r, NULL);
}

void PressMixin::write(Writer &writer) const {
	(void) writer;
}

}
