#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include <pb.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "_resetprop.h"
#include "utils.h"
#include "array.h"

/* ***********************************************************************
 * Auto generated header and constant definitions compiled from
 * android/platform/system/core/master/init/persistent_properties.proto
 * using Nanopb's protoc
 * Nanopb: https://github.com/nanopb/nanopb
 * ***********************************************************************/

/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.9.1 at Sun Apr 22 14:36:22 2018. */

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _PersistentProperties {
    pb_callback_t properties;
/* @@protoc_insertion_point(struct:PersistentProperties) */
} PersistentProperties;

typedef struct _PersistentProperties_PersistentPropertyRecord {
    pb_callback_t name;
    bool has_value;
    char value[92];
/* @@protoc_insertion_point(struct:PersistentProperties_PersistentPropertyRecord) */
} PersistentProperties_PersistentPropertyRecord;

/* Default values for struct fields */

/* Initializer values for message structs */
#define PersistentProperties_init_default        {{{NULL}, NULL}}
#define PersistentProperties_PersistentPropertyRecord_init_default {{{NULL}, NULL}, false, ""}
#define PersistentProperties_init_zero           {{{NULL}, NULL}}
#define PersistentProperties_PersistentPropertyRecord_init_zero {{{NULL}, NULL}, false, ""}

/* Field tags (for use in manual encoding/decoding) */
#define PersistentProperties_properties_tag      1
#define PersistentProperties_PersistentPropertyRecord_name_tag 1
#define PersistentProperties_PersistentPropertyRecord_value_tag 2

/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.9.1 at Sun Apr 22 14:36:22 2018. */

/* Struct field encoding specification for nanopb */
const pb_field_t PersistentProperties_PersistentPropertyRecord_fields[3] = {
		PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, PersistentProperties_PersistentPropertyRecord, name, name, 0),
		PB_FIELD(  2, STRING  , OPTIONAL, STATIC  , OTHER, PersistentProperties_PersistentPropertyRecord, value, name, 0),
		PB_LAST_FIELD
};

const pb_field_t PersistentProperties_fields[2] = {
		PB_FIELD(  1, MESSAGE , REPEATED, CALLBACK, FIRST, PersistentProperties, properties, properties, &PersistentProperties_PersistentPropertyRecord_fields),
		PB_LAST_FIELD
};

/* Maximum encoded size of messages (where known) */
/* PersistentProperties_size depends on runtime parameters */
/* PersistentProperties_PersistentPropertyRecord_size depends on runtime parameters */

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define PROPS_MESSAGES \

#endif
/* @@protoc_insertion_point(eof) */


/* ***************************
 * End of auto generated code
 * ***************************/

static bool name_decode(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	auto name = new pb_byte_t[stream->bytes_left + 1];
	name[stream->bytes_left] = '\0';
	if (!pb_read(stream, name, stream->bytes_left))
		return false;
	*arg = name;
	return true;
}

static bool name_encode(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
	return pb_encode_tag_for_field(stream, field) &&
		   pb_encode_string(stream, (const pb_byte_t *) *arg, strlen((const char *) *arg));
}

static bool prop_decode(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	PersistentProperties_PersistentPropertyRecord prop = {};
	prop.name.funcs.decode = name_decode;
	if (!pb_decode(stream, PersistentProperties_PersistentPropertyRecord_fields, &prop))
		return false;
	((read_cb_t *) *arg)->exec((const char *) prop.name.arg, prop.value);
	delete[] (pb_byte_t *) prop.name.arg;
	return true;
}

static bool prop_encode(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
	PersistentProperties_PersistentPropertyRecord prop = {};
	prop.name.funcs.encode = name_encode;
	prop.has_value = true;
	Array<prop_t> &prop_list = *(Array<prop_t> *) *arg;
	for (auto &p : prop_list) {
		if (!pb_encode_tag_for_field(stream, field))
			return false;
		prop.name.arg = p.name;
		strcpy(prop.value, p.value);
		if (!pb_encode_submessage(stream, PersistentProperties_PersistentPropertyRecord_fields, &prop))
			return false;
	}
	return true;
}

static bool write_callback(pb_ostream_t *stream, const uint8_t *buf, size_t count) {
	int fd = (intptr_t)stream->state;
	return xwrite(fd, buf, count) == count;
}

static pb_ostream_t create_ostream(const char *filename) {
	int fd = creat(filename, 0644);
	pb_ostream_t o = {
		.callback = write_callback,
		.state = (void*)(intptr_t)fd,
		.max_size = SIZE_MAX,
		.bytes_written = 0,
	};
	return o;
}

static void pb_getprop_cb(const char *name, const char *value, void *v) {
	struct prop_t *prop = static_cast<prop_t *>(v);
	if (prop->name && strcmp(name, prop->name) == 0)
		strcpy(prop->value, value);
}

static void pb_getprop(read_cb_t *read_cb) {
	LOGD("resetprop: decode with protobuf [" PERSISTENT_PROPERTY_DIR "/persistent_properties]\n");
	PersistentProperties props = {};
	props.properties.funcs.decode = prop_decode;
	props.properties.arg = read_cb;
	pb_byte_t *buf;
	size_t size;
	mmap_ro(PERSISTENT_PROPERTY_DIR "/persistent_properties", (void **) &buf, &size);
	pb_istream_t stream = pb_istream_from_buffer(buf, size);
	pb_decode(&stream, PersistentProperties_fields, &props);
	munmap(buf, size);
}

static void file_getprop(const char *name, char *value) {
	value[0] = '\0';
	char path[PATH_MAX];
	snprintf(path, sizeof(path), PERSISTENT_PROPERTY_DIR "/%s", name);
	int fd = open(path, O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		return;
	LOGD("resetprop: read prop from [%s]\n", path);
	value[read(fd, value, sizeof(PROP_VALUE_MAX))] = '\0';  // Null terminate the read value
	close(fd);
}

void persist_getprop(read_cb_t *read_cb) {
	if (use_pb) {
		pb_getprop(read_cb);
	} else {
		DIR *dir = opendir(PERSISTENT_PROPERTY_DIR);
		struct dirent *entry;
		while ((entry = readdir(dir))) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )
				continue;
			char value[PROP_VALUE_MAX];
			file_getprop(entry->d_name, value);
			if (value[0])
				read_cb->exec(entry->d_name, value);
		}
	}
}

char *persist_getprop(const char *name) {
	prop_t prop(name);
	if (use_pb) {
		read_cb_t read_cb(pb_getprop_cb, &prop);
		pb_getprop(&read_cb);
		if (prop.value[0])
			return strdup(prop.value);
	} else {
		// Try to read from file
		char value[PROP_VALUE_MAX];
		file_getprop(name, value);
		if (value[0])
			return strdup(value);
	}
	return nullptr;
}

bool persist_deleteprop(const char *name) {
	if (use_pb) {
		Array<prop_t> prop_list;
		read_cb_t read_cb(collect_props, &prop_list);
		persist_getprop(&read_cb);

		for (auto it = prop_list.begin(); it != prop_list.end(); ++it) {
			if (strcmp((*it).name, name) == 0) {
				prop_list.erase(it);
				// Dump the props back
				PersistentProperties props = PersistentProperties_init_zero;
				pb_ostream_t ostream = create_ostream(PERSISTENT_PROPERTY_DIR
						"/persistent_properties.tmp");
				props.properties.funcs.encode = prop_encode;
				props.properties.arg = &prop_list;
				LOGD("resetprop: encode with protobuf [" PERSISTENT_PROPERTY_DIR
							 "/persistent_properties.tmp]\n");
				if (!pb_encode(&ostream, PersistentProperties_fields, &props))
					return false;
				clone_attr(PERSISTENT_PROPERTY_DIR "/persistent_properties",
						   PERSISTENT_PROPERTY_DIR "/persistent_properties.tmp");
				rename(PERSISTENT_PROPERTY_DIR "/persistent_properties.tmp",
					   PERSISTENT_PROPERTY_DIR "/persistent_properties");
				return true;
			}
		}

		return false;
	} else {
		char path[PATH_MAX];
		snprintf(path, sizeof(path), PERSISTENT_PROPERTY_DIR "/%s", name);
		if (unlink(path) == 0) {
			LOGD("resetprop: unlink [%s]\n", path);
			return true;
		}
	}
	return false;
}
