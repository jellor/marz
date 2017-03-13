#include "Config.h"
#include "Logger.h"

namespace marz {

Config::Config(const char* file_name):
file_name_(file_name),
loaded_(false)
{
	LoadFile(file_name);
}

Config::~Config() {
}

const char* Config::GetValueByName(const char* name) {
	if (! loaded_) {
        return NULL;
	}

	const char* ret = NULL;

	std::map<std::string, std::string>::iterator it = configure_map_.find(name);
	if (it != configure_map_.end()) {
		ret = static_cast<const char*>(it->second.c_str());
	}

    // should return type std::string.
	return ret;
}

bool Config::SetValueByName(const char* name, const char* value) {
	if (! loaded_) {
        return false;
	}

	std::map<std::string, std::string>::iterator it = configure_map_.find(name);
	if (it != configure_map_.end()) {
		it->second = value;
	} else {
		configure_map_.insert(std::make_pair(name, value));
	}

	return SaveFile(file_name_.c_str());
}

void Config::LoadFile(const char* file_name) {
	file_name_ = file_name;

	FILE* file = fopen(file_name, "r");
	if (file == NULL) {
		WLOG << "Can not open the file " << file_name << " erron " << Logger::GetError();
		return;
	}

	char buf[512];
	while (fgets(buf, sizeof(buf), file) != NULL) {
		int len = strlen(buf);

		if (buf[len - 1] == '\n') {
			buf[len - 1] = 0;
		}
		char* p = strchr(buf, '#');
		if(p != NULL) {
            *p = 0;
		}
		if (strlen(buf) == 0) {
            continue;
		}

		ParseLine(buf);
	}

	fclose(file);
	loaded_ = true;
}

bool Config::SaveFile(const char* file_name) {
	FILE* file = fopen(file_name, "w");
	if (file == NULL) {
		return false;
	}
	char buf[512];
	for (std::map<std::string, std::string>::iterator it = configure_map_.begin(); it != configure_map_.end(); it ++) {
		memset(buf, 0, sizeof(buf)); // ? can remove this operation
		snprintf(buf, sizeof(buf), "%s = %s\n", it->first.c_str(), it->second.c_str());
		int ret = fwrite(buf, strlen(buf), 1, file);
		if (ret != 1) {
			fclose(file);
			return false;
		}
	}
	fclose(file);
	return true;
}

void Config::ParseLine(char* line) {
	char* p = strchr(line, '=');
	*p = 0;
	char* key = Trim(line);
	char* value = Trim(p + 1);
	if (key != NULL && value != NULL) {
		configure_map_.insert(std::make_pair(key, value));
	}
}

char* Config::Trim(char* str) const {
	char* begin = str;
	char* end   = str + strlen(str) - 1;
	while (*begin == ' ' || *begin == '\t') {
		begin ++;
	}
	if (strlen(begin) == 0) return NULL;

	while (*end == ' ' || *end == '\t') {
		*end = 0;
		end --;
	}
	return begin;
}

} // namespace marz
