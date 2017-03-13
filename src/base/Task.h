#ifndef __TASK_H__
#define __TASK_H__

namespace marz {

typedef std::function<void(void)> TaskCallback;

class Task {
public:
	Task(const TaskCallback& cb = nullptr): task_callback_(cb) {}
	Task(const Task& rhs) {
		task_callback_ = rhs.task_callback_;
	}
	virtual ~Task() = default;

	void Start() { if (task_callback_ != nullptr) task_callback_(); Run(); }
	virtual void Run() {}

private:
	TaskCallback task_callback_;

};

} // namespace marz

#endif // __TASK_H__
