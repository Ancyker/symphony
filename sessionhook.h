#ifndef SYMPHONY4_SESSIONHOOK_H
#define SYMPHONY4_SESSIONHOOK_H

#include <boost/signal.hpp>

struct CSession;

class SessionHook
{
public:
	typedef boost::signal<void (CSession*)> created_signal_t;

	void Init();
	void Dispose();

	bool IsCreated() { return session_ != 0; }
	int GetJob() { return get_job_pointer_(session_); }
	int GetSex() { return get_sex_pointer_(session_); }
	char* GetJobActName(int job, int sex, char *buffer) { return get_job_act_name_pointer_(session_, job, sex, buffer); }

	boost::signals::connection ConnectCreatedSlot(created_signal_t::slot_function_type slot, int priority = 0) { return created_signal_.connect(priority, slot); };

protected:
	CSession *session_;

	static int __thiscall CreateHook(CSession *session);
	int (__thiscall *create_pointer_)(CSession*);
	int (__thiscall *get_job_pointer_)(CSession*);
	int (__thiscall *get_sex_pointer_)(CSession*);
	char* (__thiscall *get_job_act_name_pointer_)(CSession*,int,int,char*);

	created_signal_t created_signal_;
};

extern SessionHook session_hook;

#endif