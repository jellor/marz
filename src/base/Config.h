#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <map>
#include <string>

namespace marz {

class Config {
public:
	Config(const char* file_name);
	~Config();

	const char* GetValueByName(const char* name);
	bool  SetValueByName(const char* name, const char* value);

private:
	void LoadFile(const char* file_name);
	bool SaveFile(const char* file_name);
	void ParseLine(char* line);
	char* Trim(char* line) const;

	std::string file_name_;
	bool loaded_;
	std::map<std::string, std::string> configure_map_;

};

} // namespace marz

#endif // __CONFIG_H__
