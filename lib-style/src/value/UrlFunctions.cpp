#include "UrlFunctions.h"
#include "FunctionLibrary.h"
#include "Value.h"
#include "UrlValue.h"
#include "NumberValue.h"
#include "Color.h"

namespace Press {

void UrlFunctions::loadFunctions(FunctionLibrary &lib) {
	lib.push("imgheight", "R", &UrlFunctions::imgheight);
	lib.push("imgwidth", "R", &UrlFunctions::imgwidth);
	lib.push("imgbackground", "R", &UrlFunctions::imgbackground);
	lib.push("image-width", "R", &UrlFunctions::imgwidth);
	lib.push("image-height", "R", &UrlFunctions::imgheight);
}

Value* UrlFunctions::imgheight(const vector<const Value*> &arguments) {
	const UrlValue* u;
	NumberValue* val;
	std::string px = "px";

	u = static_cast<const UrlValue* >(arguments[0]);

	val = new NumberValue(u->getImageHeight(), Token::DIMENSION, &px);
	return val;
}

Value* UrlFunctions::imgwidth(const vector<const Value*> &arguments) {
	const UrlValue* u;
	NumberValue* val;
	std::string px = "px";

	u = static_cast<const UrlValue* >(arguments[0]);
	val = new NumberValue(u->getImageWidth(), Token::DIMENSION, &px);
	return val;
}

Value* UrlFunctions::imgbackground(const vector<const Value*> &arguments) {
	const UrlValue* u = static_cast<const UrlValue* >(arguments[0]);
	return new Color(u->getImageBackground());
}

}
