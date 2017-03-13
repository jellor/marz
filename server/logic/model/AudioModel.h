#ifndef __AUDIO_MODEL_H__
#define __AUDIO_MODEL_H__

#include <list>
#include "Im.Base.pb.h"

namespace marz {

class AudioModel {
public:
	virtual ~AudioModel();

	static AudioModel* GetInstance();

	void SetFileSite(const std::string& file_site) { file_site_ = file_site; }
	bool ReadAudios(std::list<Im::Base::MsgInfo>& msg_info_list);
	int SaveAudio(uint32_t from, uint32_t to, uint32_t create_time, const char *audio_data, uint32_t audio_len);

private:
	AudioModel();
	bool ReadAudioContent(uint32_t duration, uint32_t size, const std::string& path, Im::Base::MsgInfo& msg_info);

	static AudioModel *instance_;
	std::string file_site_;
};

} // namespace marz

#endif