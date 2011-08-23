/* LuaDriver - I/O modules in Lua space.

TODO:
- Lazy load modules (is this feasible?)
*/

#ifndef LUA_DRIVER_H
#define LUA_DRIVER_H

/* a bunch of forward declarations */
struct lua_State;
class LuaReference;

class RageThread;
class InputHandler;
class LightsDriver;

class LuaDriver_Peripheral;

#include <map>

typedef void (*PushAPIHandleFn)(lua_State*);

class LuaDriver;

/* convenience typedef - maps a driver name to its path */
typedef map<RString,RString> DriverPathMap;

enum ModuleType
{
	ModuleType_Input,
	ModuleType_Lights,
	ModuleType_Peripheral,
	NUM_ModuleType,
	ModuleType_Invalid
};

/* XXX: we should probably separate the namespace and the base class. */
class LuaDriver
{
public:
	/* Loads modules from the given directory. Note that the modules
	 * aren't instantiated here; we just get module names, filter
	 * out the invalid ones, and save the file paths for later.
 	 * Drivers are instantiated in the following calls. */
	static void LoadModulesDir( const RString &sDir );

	/* Instantiate peripherals. Peripherals depend on MessageManager,
	 * so this must be called after MESSAGEMAN is instantiated. */
	static void LoadPeripherals();

	/* Updates all peripherals owned by LuaDriver. */
	static void Update( float fDeltaTime );

	/* If any loaded drivers match names in sDrivers, instantiate the
	 * corresponding handler, Init() it, and push it into AddTo. */
	static void AddInputModules( const RString &sDrivers, vector<InputHandler*> &AddTo );
	static void AddLightsModules( const RString &sDrivers, vector<LightsDriver*> &AddTo );

protected:
	static DriverPathMap s_mModulePaths[NUM_ModuleType];
	static vector<LuaDriver_Peripheral*> m_vpPeripherals;

public:
	LuaDriver( const RString &sName );
	~LuaDriver();

	bool IsRunning() const { return m_bRunning; }
	bool IsThreaded() const { return m_bThreaded; }
	RString GetName() const { return m_sName; }

	/* This must be set in InitModule or it will have no effect. */
	void SetThreaded( bool b ) { m_bThreaded = b; }

	/* Executes the appropriate function in the Lua driver script. */
	/* XXX: these are public for AddDrivers, but they shouldn't be public. */
	virtual bool ModuleInit( lua_State *L );
	virtual void ModuleExit( lua_State *L );
	virtual void ModuleUpdate( lua_State *L );

	bool LoadFromTable( lua_State *L, LuaReference *pTable );
	virtual bool LoadDerivedFromTable( lua_State *L, LuaReference *pTable ) { return true; }

protected:
	static int ModuleThread_Start( void *p ) { ((LuaDriver*)p)->ModuleThread(); return 0; }
	virtual void ModuleThread() = 0;

	bool m_bRunning, m_bThreaded;

	/* Used for logging, name matching, and GetDevicesAndDescriptions */
	RString m_sName, m_sDesc;

	/* m_pThread handles Update() if this driver is threaded */
	RageThread *m_pThread;

	/* Reference to the Lua table describing this driver */
	LuaReference *m_pDriver;

	/* References to the Lua functions that the driver requires */
	LuaReference *m_pInit, *m_pExit, *m_pUpdate;
};

#endif // LUA_DRIVER_H

/*
 * (c) 2011 Mark Cannon
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
