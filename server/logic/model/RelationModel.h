#ifndef __RELATION_MODLE_H__
#define __RELATION_MODLE_H__

namespace marz {

class RelationModel {
public:
	~RelationModel();
	static RelationModel *GetInstance();

	uint32_t GetRelationId(uint32_t a_user_id, uint32_t b_user_id, bool add);
	bool UpdateRelation(uint32_t relation_id, uint32_t update_time);
	bool RemoveRelation(uint32_t relation_id);

private:
	RelationModel();

	uint32_t AddRelation(uint32_t small_id, uint32_t big_id);
	static RelationModel *instance_;
};

} // namespace marz

#endif // __RELATION_MODEL_H__
