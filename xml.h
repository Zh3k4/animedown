#ifndef XML_H_
#define XML_H_

#include "zstring.h"

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
#ifdef XML_IMPLEMENTATION

void
xml_next(XmlParser xml[static 1])
{
	if (strncmp(xml->cursor, "</rss>", 6) == 0) {
		xml->kind = XML_EOF;
		return;
	}

	if (strncmp(xml->cursor, "<rss", 4) == 0) {
		char *start = xml->cursor + 1;
		while (*xml->cursor != '>') {
			xml->cursor += 1;
		}

		xml->kind = XML_META;
		xml->data = (String){
			.data = start,
			.length = (size_t)(xml->cursor - start),
		};

		xml->cursor += 1;
		return;
	}

	if (*xml->cursor == '<') {
		xml->cursor += 1;
		if (*xml->cursor == '/') {
			xml->kind = XML_ELEMENTEND;
			xml->cursor += 1;
		} else {
			xml->kind = XML_ELEMENTSTART;
		}

		char *start = xml->cursor;
		while (*xml->cursor != '>') {
			xml->cursor += 1;
		}

		xml->data = (String){
			.data = start,
			.length = (size_t)(xml->cursor - start)
		};

		xml->cursor += 1;
	} else {
		char *start = xml->cursor;
		while (*xml->cursor != '<' && *xml->cursor != '\0') {
			xml->cursor += 1;
		}

		xml->kind = XML_CHARDATA;
		xml->data = (String){
			.data = start,
			.length = (size_t)(xml->cursor - start)
		};
	}
}

#endif /* XML_IMPLEMENTATION */
