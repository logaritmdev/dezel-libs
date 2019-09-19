#ifndef __PressExtension_h__
#define __PressExtension_h__

#include "Selector.h"

namespace Press {

class PressExtension {

private:
	Selector target;
	Selector extension;
	bool all;

public:
	PressExtension();
	virtual ~PressExtension();
	Selector &getTarget();
	Selector &getExtension();
	const Selector &getTarget() const;
	const Selector &getExtension() const;
	void setExtension(const Selector &extension);
	void setAll(bool b);
	bool isAll() const;
	void updateSelector(Selector &s) const;
	void replaceInSelector(Selector &s) const;
};

}

#endif 
