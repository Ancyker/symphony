#ifndef SYMPHONY4_MODEMGRHOOK_H
#define SYMPHONY4_MODEMGRHOOK_H

#include <boost/signal.hpp>

struct CModeMgr;

class ModeMgrHook
{
public:
	typedef boost::signal<void (CModeMgr*,int,char*)> switched_signal_t;

	void Init();
	void Dispose();

	int GetMode() { return mode_; }

	boost::signals::connection ConnectSwitchedSlot(switched_signal_t::slot_function_type slot, int priority = 0) { return switched_signal_.connect(priority, slot); };

	enum { LOGIN_MODE = 0, GAME_MODE = 1 };

protected:
	static int __thiscall RunHook(CModeMgr *mode_mgr, int mode, char *rsw);
	int (__thiscall *run_pointer_)(CModeMgr *mode_mgr, int mode, char *rsw);

	static int __thiscall SwitchHook(CModeMgr *mode_mgr, int mode, char *rsw);
	int (__thiscall *switch_pointer_)(CModeMgr *mode_mgr, int mode, char *rsw);

	int mode_;

	switched_signal_t switched_signal_;
};

extern ModeMgrHook mode_mgr_hook;

#endif