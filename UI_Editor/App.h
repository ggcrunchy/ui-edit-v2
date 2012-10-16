#ifndef APP_H
#define APP_H

#include <SDL/SDL_types.h>
extern "C"
{
#include "../Lua/include/lua.h"
#include "../Lua/include/lualib.h"
#include "../Lua/include/lauxlib.h"
}

void luaopen_dirent (lua_State * L);
void luaopen_graphics (lua_State * L);
void luaopen_misc (lua_State * L);
void luaopen_sdl (lua_State * L);
void luaopen_ui (lua_State * L);

void Post (lua_State * L, char const * message);

Uint32 U (lua_State * L, int index);
Uint8 U8 (lua_State * L, int index);
Uint16 U16 (lua_State * L, int index);
Sint32 S32 (lua_State * L, int index);
Sint16 S16 (lua_State * L, int index);
int I (lua_State * L, int index);
long LI (lua_State * L, int index);
float F (lua_State * L, int index);
bool B (lua_State * L, int index);
char const * S (lua_State * L, int index);
void * UD (lua_State * L, int index);
void * UT (lua_State * L, int index);
Uint8 * PU8 (lua_State * L, int index);

template<typename T> T UDT (lua_State * L, int index)
{
	return static_cast<T>(UD(L, index));
}

int V_V (lua_State * L, void (*func)(void));
int V_U (lua_State * L, void (*func)(Uint32));
int I_V (lua_State * L, int (*func)(void));
int I_Ut (lua_State * L, int (*func)(void *));

typedef struct {
	Uint32 mOffset;
	char * mName;
	char * mUserType;
	enum {
		eUserType,
		ePointer,
		eU8, eU16, eU32,
		eS8, eS16, eS32,
		eUChar, eUShort, eULong, eUInt,
		eSChar, eSShort, eSLong, eSInt,
		eString,
		eBoolean,
		eFloat
	} mType;
} UserType_Reg;

void PushUserType (lua_State * L, void * data, char const * name);
void RegisterUserType (lua_State * L, char const * name, UserType_Reg * reg, Uint32 count, lua_CFunction GC);

void RegisterClass (lua_State * L, char const * name, char const * methods, lua_CFunction GC);

#endif // APP_H