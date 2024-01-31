#ifndef XML_H_
#define XML_H_

#include "slice.h"

enum XmlKind {
	XML_EOF,
	XML_META,
	XML_ELEMENTSTART,
	XML_ELEMENTEND,
	XML_CHARDATA,
};

typedef struct {
	char *cursor;
	enum XmlKind kind;
	String data;
} XmlParser;

void xml_next(XmlParser xml[static 1]);

#endif /* XML_H_ */
