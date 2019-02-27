#include "StylerMixin.h"
#include "StylerRuleset.h"
#include "StylerStylesheet.h"
#include "StylerMixinException.h"

StylerMixin::StylerMixin(const TokenList &name, const StylerStylesheet &parent)
	: lessStylesheet(&parent), lessRuleset(NULL), important(false), name(name) {
}

StylerMixin::StylerMixin(const TokenList &name, const StylerRuleset &parent)
	: lessStylesheet(NULL), lessRuleset(&parent), important(false), name(name) {
}

StylerMixin::~StylerMixin() {
}


bool StylerMixin::call(ProcessingContext &context,
	Ruleset *r_target,
	Stylesheet *s_target) const {

	std::list<const StylerFunction *>::iterator i;
	std::list<const StylerFunction *> functionList;
	const StylerFunction *function;
	bool success = false;

	StylerMixinArguments arguments_p;

	context.getFunctions(functionList, *this);

	if (functionList.empty()) {
		throw new StylerMixinException(*this);
	}

	arguments_p = arguments;
	arguments_p.process(context);

	for (i = functionList.begin(); i != functionList.end(); i++) {
		function = *i;

		if (function->getLessSelector().needsArguments() ||
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

			if (function->getLessSelector().needsArguments() ||
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

void StylerMixin::setImportant(bool b) {
	important = b;
}

bool StylerMixin::isImportant() const {
	return important;
}

const StylerStylesheet *StylerMixin::getLessStylesheet() const {
	return lessStylesheet;
}

const StylerRuleset *StylerMixin::getLessRuleset() const {
	return lessRuleset;
}

void StylerMixin::process(Stylesheet &s, void *context) const {
	call(*((ProcessingContext *) context), NULL, &s);
}

void StylerMixin::process(Ruleset &r, void *context) const {
	call(*((ProcessingContext *) context), &r, NULL);
}

void StylerMixin::write(Writer &writer) const {
	(void) writer;
}
