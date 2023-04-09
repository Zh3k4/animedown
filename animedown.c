#include <assert.h>
#include <curl/curl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#define UTIL_IMPLEMENTATION
#include "util.h"
#define XML_IMPLEMENTATION
#include "xml.h"
#define ZSTRING_IMPLEMENTATION
#include "zstring.h"

typedef struct {
	String title;
	String link;
} Torrent;

Torrent *
torrent_list_add(Torrent *list, size_t *size, Torrent tor)
{
	list = realloc(list, (*size + 1) * sizeof(Torrent));
	if (list == NULL) {
		return NULL;
	}

	*size += 1;
	list[*size - 1] = tor;

	return list;
}

int
main(void)
{
	CURL *handle;
	CURLcode result;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	handle = curl_easy_init();

	if (!handle) {
		fprintf(stderr, "Error: could not initialize curl\n");
		return EXIT_FAILURE;
	}

	MemoryRegion m = {0};

	curl_easy_setopt(handle, CURLOPT_URL, "https://subsplease.org/rss/?r=720");
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&m.memory);

	result = curl_easy_perform(handle);

	if (result != CURLE_OK) {
		fprintf(stderr, "Error: %s\n", curl_easy_strerror(result));
	}/* else {
		printf("%s\n", m.memory);
	}*/

	XmlParser xml;
	xml.cursor = m.memory;

	Torrent *list = NULL;
	size_t listsz = 0;

	String title = {0};
	String link = {0};
	/* String filename = {0}; */
	int in_item = 0;

	for (;;) {
		xml_next(&xml);

		if (xml.kind == XML_EOF) {
			break;
		}

		if (xml.kind == XML_ELEMENTSTART && eq_to_cstr(xml.data, "item")) {
			in_item = 1;
			continue;
		}

		if (xml.kind == XML_ELEMENTEND && eq_to_cstr(xml.data, "item")
				&& in_item) {
			in_item = 0;
			Torrent *res = torrent_list_add(list, &listsz,
				(Torrent){
					.title = title,
					.link = link,
					/* .filename = filename */
				});
			if (res == NULL) {
				fprintf(stderr, "Error: could not append torrent list\n");
				goto defer;
			}
			list = res;
			continue;
		}

		if (xml.kind != XML_ELEMENTSTART) {
			continue;
		}

		if (eq_to_cstr(xml.data, "category")) {
			xml_next(&xml);

			if (xml.kind != XML_CHARDATA) {
				break;
			}

			xml.data.length -= 6; /* remove resolution suffix */
			title = xml.data;
		} else if (eq_to_cstr(xml.data, "link")) {
			xml_next(&xml);

			if (xml.kind != XML_CHARDATA) {
				break;
			}

			link = xml.data;
		}/* else if (eq_to_cstr(xml.data, "title")) {
			xml_next(&xml);

			if (xml.kind != XML_CHARDATA) {
				break;
			}

			filename = xml.data;
		}*/
	}


	/* get session id */

	MemoryRegion response = {0};

	curl_easy_reset(handle);
	curl_easy_setopt(handle, CURLOPT_URL, "http://127.0.0.1:9091/transmission/rpc");
	curl_easy_setopt(handle, CURLOPT_POST, 1);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "");
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&response.memory);

	result = curl_easy_perform(handle);

	if (result != CURLE_OK) {
		fprintf(stderr, "Error: could not perform request\n");
		free(response.memory);
		goto defer;
	}

	long code;
	curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &code);

	if (code != 409) {
		fprintf(stderr, "Error: wrong response code\n");
		free(response.memory);
		goto defer;
	}

	size_t offset = 0, start;
	while(strncmp(response.memory + offset,
			"X-Transmission-Session-Id: ", 27) != 0) {
		++offset;
	}
	offset += 27;
	start = offset;
	while(*(response.memory + offset) != '<') {
		++offset;
	}

	String session_id = {.data = (response.memory + start), .length = (offset - start)};
	struct curl_slist *headers = NULL;
	char header[1024] = {0};
	snprintf((char *)&header, 1024, "X-Transmission-Session-Id: %.*s",
			(int)session_id.length, session_id.data);
	headers = curl_slist_append(headers, (char *)&header);


	for (size_t i = 0; i < listsz; ++i) {
		for (size_t j = 0; filter[j] != NULL; ++j) {
			if (!eq_to_cstr(list[i].title, filter[j])) {
				continue;
			}

			char json[2048];
			snprintf((char *)&json, 2048,
					"{"
					"\"arguments\":{\"filename\":\"%.*s\"},"
					"\"method\": \"torrent-add\","
					"\"tag\": 8"
					"}", (int)list[i].link.length, list[i].link.data);
			
			curl_easy_reset(handle);
			curl_easy_setopt(handle, CURLOPT_URL, "http://127.0.0.1:9091/transmission/rpc");
			curl_easy_setopt(handle, CURLOPT_POST, 1);
			curl_easy_setopt(handle, CURLOPT_POSTFIELDS, json);
			curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

			curl_easy_perform(handle);

			if (result != CURLE_OK) {
				fprintf(stderr, "Error: could not upload magnet link\n");
			}
		}
	}

	curl_slist_free_all(headers);
	free(response.memory);

defer:
	if (list) free (list);
	if (m.memory) free(m.memory);
	if (handle) curl_easy_cleanup(handle);
	curl_global_cleanup();
	return EXIT_SUCCESS;
}

