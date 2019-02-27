#include "Comment.h"

Comment::Comment() {
	comment = "";
}

Comment::Comment(const Token &comment) {
	this->comment = comment;
}

void Comment::setComment(const Token &comment) {
	this->comment = comment;
}

const Token &Comment::getComment() const {
	return comment;
}

void Comment::process(Ruleset &r, void *context) const {
	(void) context;
	Comment *c = r.createComment();
	c->setComment(comment);
}

void Comment::process(Stylesheet &s, void *context) const {
	(void) context;
	Comment *c = s.createComment();
	c->setComment(comment);
}

void Comment::write(Writer &writer) const {
	writer.writeComment(comment);
}
