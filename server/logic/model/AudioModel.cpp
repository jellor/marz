#include <json/json.h>
#include "AudioModel.h"
#include "Logger.h"
#include "Util.h"
#include "HttpClient.h"

#include "CacheManager.h"
#include "DatabaseManager.h"

namespace marz {

AudioModel* AudioModel::instance_ = nullptr;

AudioModel::AudioModel()
{
}

AudioModel::~AudioModel(){
}

AudioModel* AudioModel::GetInstance() {
	
	if (instance_ == nullptr) {
		instance_ = new AudioModel();
	}

	return instance_;
}

bool AudioModel::ReadAudios(std::list<Im::Base::MsgInfo>& msg_info_list) {
	
	if (msg_info_list.empty()) {
		WLOG << "read audios is empty";
		return true;
	}

	bool ret = false;
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return ret;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return ret;
	}

	for (auto it = msg_info_list.begin(); it != msg_info_list.end(); ) {
		Im::Base::MsgType msg_type = it->msg_type();
		if (msg_type == Im::Base::MSG_TYPE_SINGLE_AUDIO || msg_type == Im::Base::MSG_TYPE_GROUP_AUDIO) {
			std::string sql = "SELECT * FROM ImAudio WHERE id=" + it->msg_data();
			ResultSet* result = db_connector->Query(sql.c_str());
			if (result == nullptr) {
				WLOG << "result set is nullptr";
				it = msg_info_list.erase(it);
			} else {
				while (result->Next()) {
					uint32_t duration = result->GetInt("duration");
					uint32_t size = result->GetInt("size");
					std::string path = result->GetString("path");
					ReadAudioContent(duration, size, path, *it);
				}
				delete result;
				it ++;
			}
		} else {
			it ++;
		}
	}

	db_manager->ReleaseDatabaseConnector(db_connector);
	return true;
}

int AudioModel::SaveAudio(uint32_t from, uint32_t to, uint32_t create_time, 
	const char *audio_data, uint32_t audio_len) {
	
	DLOG << "from id => " << from << " to id => " << to << " create time => " << create_time;
	uint32_t duration = Util::StringToUint32(audio_data);
	char *data = (char*)audio_data + 4;
	uint32_t len = audio_len - 4;
	int audio_id = -1;

	HttpClient httpclient;
	std::string retstr = httpclient.UploadByteFile(file_site_, data, len);

	Json::Reader reader;
	Json::Value value;
	if (!reader.parse(retstr, value)) {
		WLOG << "Json::Reader parse failed";
		return audio_id;
	}

	if (value["error_code"].isNull()) {
		WLOG << " value[error_code'] failed ret => " << retstr;
		return audio_id;
	}
	
	uint32_t code = value["error_code"].asUInt();
	if (code != 0) {
		WLOG << "upload failed ret => " << retstr;
		return audio_id;
	}

	std::string path = value["url"].asString();

	if (!path.empty()) {
		// Get Batabase Connector. 
		DatabaseManager* db_manager = DatabaseManager::GetInstance();
		if (db_manager == nullptr) {
			WLOG << "db_manager is nullptr";
			return -1;
		}
		DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
		if (db_connector == nullptr) {
			WLOG << "db_connector is nullptr";
			return -1;
		}

		std::string sql = "INSERT INTO ImAudio (`from_id`, `to_id`, `path`, `size`, `duration`, `created`,) VALUES (" 
			+ Util::Uint32ToString(from) + ", " 
			+ Util::Uint32ToString(to) + ", " 
			+ path + ", " 
			+ Util::Uint32ToString(len) + ", " 
			+ Util::Uint32ToString(duration) +", " 
			+ Util::Uint32ToString(create_time) + " )";
			
		if (db_connector->Update(sql.c_str())) {
			audio_id = db_connector->GetInsertId();
		} else {
			WLOG << "insert into ImAudio failed";
		}

		db_manager->ReleaseDatabaseConnector(db_connector);

	} else {
		WLOG << "httpclent uploadByteFile failed";
	}

	return audio_id;
}

bool AudioModel::ReadAudioContent(uint32_t duration, uint32_t size, 
	const std::string& path, Im::Base::MsgInfo& msg_info) {
	
	if (duration == 0 || size == 0 || path.empty()) {
		return false;
	}

	audio_t audio;
	audio.data = new char [4 + size];
	Util::Uint32ToStr(audio.data, duration);
	audio.data_len = 4;
	audio.file_size = size;

	HttpClient httpclient;
	if (httpclient.DownloadByteFile(path, &audio)) {
		WLOG << "HttpClient downloadByteFile failed";
		delete[] audio.data;
		return false;
	}

	DLOG << "download path" << path << " data_len => " << audio.data_len;
	msg_info.set_msg_data((const char *)audio.data, audio.data_len);

	delete[] audio.data;
	return true;
}

} // namespace marz
