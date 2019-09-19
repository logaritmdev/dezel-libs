#ifndef __Writable_h__
#define __Writable_h__

#include "Writer.h"

namespace Press {

class Writable {
public:
	virtual void write(Writer &css) const = 0;
};

}

#endif
