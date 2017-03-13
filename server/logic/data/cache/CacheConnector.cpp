#include "CacheConnector.h"
#include "Logger.h"

namespace marz {

CacheConnector::CacheConnector(CachePool* cache_pool):
cache_pool_(cache_pool),
redis_context_(NULL),
last_connect_time_(0)
{
}

CacheConnector::~CacheConnector()
{
	if (redis_context_ != NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
	}
}

int CacheConnector::Init() {
	if (redis_context_ != NULL) {
		return 0;
	}

	uint64_t current_time = (uint64_t) time(NULL);
	if (current_time < last_connect_time_ + 4) {
		return 1;
	}

	last_connect_time_ = current_time;

	struct timeval timeout = {0, 200000};
	DLOG << cache_pool_->GetServerIp() << " " << cache_pool_->GetServerPort();
	redis_context_ = redisConnectWithTimeout(cache_pool_->GetServerIp(), cache_pool_->GetServerPort(), timeout);
	if (redis_context_ == NULL || redis_context_->err) {
		if (redis_context_ != NULL) {
			WLOG << "Error : " << redis_context_->errstr;
			redisFree(redis_context_);
			redis_context_ = NULL;
		} else {
			WLOG << "Error : redisConnectWithTimeout failed";
		}

		return 1;
	}

	redisReply* reply = (redisReply*) redisCommand(redis_context_, "SELECT %d", cache_pool_->GetDbNumber());
	if (reply != NULL && (reply->type == REDIS_REPLY_STATUS) && (strncmp(reply->str, "OK", 2)) == 0) {
		freeReplyObject(reply);
		return 0;
	} else {
		WLOG << "cache connector init failed";
		return 2;
	}
}

const char* CacheConnector::GetPoolName() {
	return cache_pool_->GetPoolName();
}

std::string CacheConnector::Get(std::string key) {
	std::string value;

	if (Init() != 0) {
		return value;
	}

	redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(redis_context_, "GET %s", key.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return value;
	} 

	if (reply->type == REDIS_REPLY_STRING) {
		value.append(reply->str, reply->len);
	}

	freeReplyObject(reply);
	return value;
}

std::string CacheConnector::Setex(std::string key, int timeout, std::string value) {
	std::string ret_value;

	if (Init() != 0) {
		return value;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "SETEX %s %d %s", key.c_str(), timeout, value.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return ret_value;
	} 

	ret_value.append(reply->str, reply->len);
	freeReplyObject(reply);
	return ret_value;
}

std::string CacheConnector::Set(std::string key, std::string& value) {
	std::string ret_value;

	if (Init() != 0) {
		return ret_value;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "SET %s %s", key.c_str(), value.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return value;
	} 
	
	ret_value.append(reply->str, reply->len);
	freeReplyObject(reply);
	return ret_value;
}

bool CacheConnector::Mget(const std::vector<std::string>& keys, std::map<std::string, std::string>& ret_values) {
	
	if (Init() != 0) {
		return false;
	}

	if (keys.empty()) {
		return false;
	}

	std::string str_key;
	bool b_first = true;
	for (std::vector<std::string>::const_iterator it = keys.begin(); it != keys.end(); it ++) {
		if (b_first) {
			b_first = false;
			str_key = *it;
		} else {
			str_key += " " + *it;
		}
	}

	if (str_key.empty()) {
		return false;
	}

	str_key = "MGET " + str_key;
	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, str_key.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	} 

	if (reply->type == REDIS_REPLY_ARRAY) {
		for (size_t i = 0; i < reply->elements; i ++) {
			redisReply* child_reply = reply->element[i];
			if (child_reply->type == REDIS_REPLY_STRING) {
				ret_values[keys[i]] = child_reply->str;
			}
		}
	}
	
	freeReplyObject(reply);
	return true;
}

bool CacheConnector::IsExists(std::string& key) {
	
	if (Init() != 0) {
		return false;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "EXISTS %s", key.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	long value = reply->integer;

	freeReplyObject(reply);
	if (value == 0) {
		return false;
	} else {
		return true;
	}
}

// redis hash structure
long CacheConnector::Hdel(std::string key, std::string field) {
	
	if (Init() == 0) {
		return 0;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "HDEL %s %s", key.c_str(), field.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return 0;
	}

	long value = reply->integer;

	freeReplyObject(reply);
	return value;
}

std::string CacheConnector::Hget(std::string key, std::string field) {
	
	std::string value;
	if (Init() == 0) {
		return value;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "HGET %s %s", key.c_str(), field.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return value;
	}

	if (reply->type == REDIS_REPLY_STRING) {
		value.append(reply->str, reply->len);
	}

	freeReplyObject(reply);
	return value;
}

bool CacheConnector::HgetAll(std::string key, std::map<std::string, std::string>& ret_values) {
	
	if (Init() == 0) {
		return false;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "HGETALL %s", key.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	if ((reply->type == REDIS_REPLY_ARRAY) && (reply->elements % 2 == 0)) {
		for (size_t i = 0; i < reply->elements; i += 2) {
			redisReply* field_reply = reply->element[i];
			redisReply* value_reply = reply->element[i + 1];

			std::string field(field_reply->str, field_reply->len);
			std::string value(value_reply->str, value_reply->len);
			ret_values.insert(make_pair(field, value)); 
		}
	}

	freeReplyObject(reply);
	return true;
}

long CacheConnector::Hset(std::string key, std::string field, std::string value) {
	
	if (Init() != 0) {
		return -1;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return -1;
	}

	long ret_value = reply->integer;

	freeReplyObject(reply);
	return ret_value;
}

long CacheConnector::HincrBy(std::string key, std::string field, long value) {
	
	if (Init() != 0) {
		return -1;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "HINCRBY %s %s %d", key.c_str(), field.c_str(), value));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return -1;
	}

	long ret_value = reply->integer;

	freeReplyObject(reply);
	return ret_value;
}

long CacheConnector::IncrBy(std::string key, long value) {
	
	if (Init() != 0) {
		return -1;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "INCRBY %s %d", key.c_str(), value));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return -1;
	}

	long ret_value = reply->integer;

	freeReplyObject(reply);
	return ret_value;
}

std::string CacheConnector::Hmset(std::string key, std::map<std::string , std::string>& hash) {
	
	std::string value;

	if (Init() != 0) {
		return value;
	}

	int argc = hash.size() * 2 + 2;
	const char** argv = new const char* [argc];
	if (argv == NULL) {
		return value;
	}	

	argv[0] = "HMSET";
	argv[1] = key.c_str();

	int i = 2;
	for (std::map<std::string, std::string>::iterator it = hash.begin(); it != hash.end(); it ++) {
		argv[i ++] = it->first.c_str();
		argv[i ++] = it->second.c_str(); 
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommandArgv(redis_context_, argc, argv, NULL));
	if (reply == NULL) {
		delete[] argv;
		redisFree(redis_context_);
		redis_context_ = NULL;
		return value;
	}

	value.append(reply->str, reply->len);
	delete[] argv;
	freeReplyObject(reply);
	return value;
}

bool CacheConnector::Hmget(std::string key, std::list<std::string>& fields, std::list<std::string>& ret_values) {
	
	if (Init() != 0) {
		return false;
	}

	int argc = fields.size() + 2;
	const char** argv = new const char* [argc];
	if (argv == NULL) {
		return false;
	}	

	argv[0] = "HMGET";
	argv[1] = key.c_str();

	int i = 2;
	for (std::list<std::string>::iterator it = fields.begin(); it != fields.end(); it ++) {
		argv[i ++] = it->c_str();
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommandArgv(redis_context_, argc, argv, NULL));
	if (reply == NULL) {
		delete[] argv;
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	if (reply->type == REDIS_REPLY_ARRAY) {
		for (size_t i = 0; i < reply->elements; i ++) {
			redisReply* value_reply = reply->element[i];
			std::string value(value_reply->str, value_reply->len);
			ret_values.push_back(value);
		}
	}

	delete[] argv;
	freeReplyObject(reply);
	return true;
}

// atomic operation
long CacheConnector::Incr(std::string key) {
	
	if (Init() != 0) {
		return -1;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "INCR %s", key.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	long value = reply->integer;
	freeReplyObject(reply);
	return value;
}

long CacheConnector::Decr(std::string key) {
	
	if (Init() != 0) {
		return -1;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "DECR %s", key.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	long value = reply->integer;
	freeReplyObject(reply);
	return value;
}

// redis list structure
long CacheConnector::Lpush(std::string key, std::string value) {
	
	if (Init() != 0) {
		return -1;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "LPUSH %s %s", key.c_str(), value.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	long ret_value = reply->integer;
	freeReplyObject(reply);
	return ret_value;
}

long CacheConnector::Rpush(std::string key, std::string value) {
	
	if (Init() != 0) {
		return -1;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "RPUSH %s %s", key.c_str(), value.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	long ret_value = reply->integer;
	freeReplyObject(reply);
	return ret_value;
}

long CacheConnector::Llen(std::string key) {
	
	if (Init() != 0) {
		return -1;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "LLEN %s", key.c_str()));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	long value = reply->integer;
	freeReplyObject(reply);
	return value;
}

bool CacheConnector::Lrange(std::string key, long start, long end, std::list<std::string>& ret_values) {
	
	if (Init() != 0) {
		return false;
	}

	redisReply* reply = reinterpret_cast<redisReply*> 
		(redisCommand(redis_context_, "LRANGE %s %d %d", key.c_str(), start, end));
	if (reply == NULL) {
		redisFree(redis_context_);
		redis_context_ = NULL;
		return false;
	}

	if (reply->type == REDIS_REPLY_ARRAY) {
		for (size_t i = 0; i < reply->elements; i ++) {
			redisReply* value_reply = reply->element[i];
			std::string value(value_reply->str, value_reply->len);
			ret_values.push_back(value);
		}
	}

	freeReplyObject(reply);
	return true;
}


} // namespace marz
