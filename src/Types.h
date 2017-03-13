#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

namespace marz {	

const int INVALID_VALUE = 0;

static const std::string GROUP_COUNTER_FIELD 						= "count";
static const std::string GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_PREFIX 	= "group_message_";
static const std::string GROUP_USER_MSG_COUNTER_REDIS_KEY_PREFIX 	= "user_group_";

typedef unsigned char		uchar_t;

enum IM_GROUP_SETTING {
	IM_GROUP_SETTING_PUSH = 1
};

typedef struct packet_s {
    int32_t length;
    int16_t version;
    int16_t service;
    int16_t command;
    int16_t sequence;
    int16_t reserve;
    char* data;
    int32_t checkSum;
} packet_t;

typedef struct audio_s {
	uint32_t audio_id;
	uint32_t file_size;
	uint32_t data_len;
	char *data;
	std::string path;
} audio_t;

typedef struct user_s {
	uint32_t user_id;
	uint8_t sex;
	std::string name;
	std::string domain;
	std::string nick;
	std::string phone;
	std::string email;
	std::string avatar;
	uint8_t status;

	struct user_s& operator=(const struct user_s& user) {
		if (this != &user) {
			this->user_id = user.user_id;
			this->sex = user.sex;
			this->name = user.name;
			this->domain = user.domain;
			this->nick = user.nick;
			this->phone = user.phone;
			this->email = user.email;
			this->avatar = user.avatar;
			this->status = user.status;
		}

		return *this;
	}

} user_info_t;

} // marz

#endif // __TYPES_H__
