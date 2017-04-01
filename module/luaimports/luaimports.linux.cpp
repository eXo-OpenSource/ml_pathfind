/*
 * Copyright (c) 2008, Alberto Alonso
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this
 *       list of conditions and the following disclaimer in the documentation and/or other
 *       materials provided with the distribution.
 *     * Neither the name of the mta-mysql nor the names of its contributors may be used
 *       to endorse or promote products derived from this software without specific prior
 *       written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <dlfcn.h>
#include <stdarg.h>
#include "ILuaModuleManager.h"
#include "lauxlib.h"
#include "luaconf.h"
#include "lua.h"
#include "lualib.h"

/* This file extracts the lua function addresses from the server core to avoid lua gc crashes */

extern ILuaModuleManager* pModuleManager;

/*
 ** state manipulation
 */
static void* plua_newstate = 0;
static void* plua_close = 0;
static void* plua_newthread = 0;
static void* plua_atpanic = 0;

/*
 ** basic stack manipulation
 */
static void* plua_gettop = 0;
static void* plua_settop = 0;
static void* plua_pushvalue = 0;
static void* plua_remove = 0;
static void* plua_insert = 0;
static void* plua_replace = 0;
static void* plua_checkstack = 0;
static void* plua_xmove = 0;

/*
 ** access functions (stack -> C)
 */
static void* plua_isnumber = 0;
static void* plua_isstring = 0;
static void* plua_iscfunction = 0;
static void* plua_isuserdata = 0;
static void* plua_type = 0;
static void* plua_typename = 0;
static void* plua_equal = 0;
static void* plua_rawequal = 0;
static void* plua_lessthan = 0;
static void* plua_tonumber = 0;
static void* plua_tointeger = 0;
static void* plua_toboolean = 0;
static void* plua_tolstring = 0;
static void* plua_objlen = 0;
static void* plua_tocfunction = 0;
static void* plua_touserdata = 0;
static void* plua_tothread = 0;
static void* plua_topointer = 0;

/*
 ** push functions (C -> stack)
 */
static void* plua_pushnil = 0;
static void* plua_pushnumber = 0;
static void* plua_pushinteger = 0;
static void* plua_pushlstring = 0;
static void* plua_pushstring = 0;
static void* plua_pushvfstring = 0;
static void* plua_pushfstring = 0;
static void* plua_pushcclosure = 0;
static void* plua_pushboolean = 0;
static void* plua_pushlightuserdata = 0;
static void* plua_pushthread = 0;

/*
 ** get functions (Lua -> stack)
 */
static void* plua_gettable = 0;
static void* plua_getfield = 0;
static void* plua_rawget = 0;
static void* plua_rawgeti = 0;
static void* plua_createtable = 0;
static void* plua_newuserdata = 0;
static void* plua_getmetatable = 0;
static void* plua_getfenv = 0;

/*
 ** set functions (stack -> Lua)
 */
static void* plua_settable = 0;
static void* plua_setfield = 0;
static void* plua_rawset = 0;
static void* plua_rawseti = 0;
static void* plua_setmetatable = 0;
static void* plua_setfenv = 0;

/*
 ** `load' and `call' functions (load and run Lua code)
 */
static void* plua_call = 0;
static void* plua_pcall = 0;
static void* plua_cpcall = 0;
static void* plua_load = 0;
static void* plua_dump = 0;

/*
 ** coroutine functions
 */
static void* plua_yield = 0;
static void* plua_resume = 0;
static void* plua_status = 0;

/*
 ** garbage-collection function and options
 */
static void* plua_gc = 0;


/*
 ** miscellaneous functions
 */
static void* plua_error = 0;
static void* plua_next = 0;
static void* plua_concat = 0;
static void* plua_getallocf = 0;
static void* plua_setallocf = 0;

/* Functions to be called by the debuger in specific events */
static void* plua_getstack = 0;
static void* plua_getinfo = 0;
static void* plua_getlocal = 0;
static void* plua_setlocal = 0;
static void* plua_getupvalue = 0;
static void* plua_setupvalue = 0;
static void* plua_sethook = 0;
static void* plua_gethook = 0;
static void* plua_gethookmask = 0;
static void* plua_gethookcount = 0;

/*
 ** Lua auxlib
 */
#if defined(LUA_COMPAT_GETN)
static void* pluaL_getn = 0;
static void* pluaL_setn = 0;
#endif
static void* pluaL_register = 0;
static void* pluaL_getmetafield = 0;
static void* pluaL_callmeta = 0;
static void* pluaL_typerror = 0;
static void* pluaL_argerror = 0;
static void* pluaL_checklstring = 0;
static void* pluaL_optlstring = 0;
static void* pluaL_checknumber = 0;
static void* pluaL_optnumber = 0;
static void* pluaL_checkinteger = 0;
static void* pluaL_optinteger = 0;
static void* pluaL_checkstack = 0;
static void* pluaL_checktype = 0;
static void* pluaL_checkany = 0;
static void* pluaL_newmetatable = 0;
static void* pluaL_checkudata = 0;
static void* pluaL_where = 0;
static void* pluaL_error = 0;
static void* pluaL_checkoption = 0;
static void* pluaL_ref = 0;
static void* pluaL_unref = 0;
static void* pluaL_loadfile = 0;
static void* pluaL_loadbuffer = 0;
static void* pluaL_loadstring = 0;
static void* pluaL_newstate = 0;
static void* pluaL_gsub = 0;
static void* pluaL_findtable = 0;
static void* pluaL_buffinit = 0;
static void* pluaL_prepbuffer = 0;
static void* pluaL_addlstring = 0;
static void* pluaL_addstring = 0;
static void* pluaL_addvalue = 0;
static void* pluaL_pushresult = 0;
static void* plua_getmainstate = 0;


#define SAFE_IMPORT(x) \
  p ## x = dlsym(dl, #x); \
  if (p ## x == 0) \
  { \
    pModuleManager->Printf("Unable to import " #x ": %s\n", dlerror()); \
    return false; \
  }

#ifdef __cplusplus
extern "C"
#endif
bool ImportLua()
{
#ifdef __x86_64
  void* dl = dlopen("x64/deathmatch.so", RTLD_NOW | RTLD_NOLOAD);
#else
  void* dl = dlopen("mods/deathmatch/deathmatch.so", RTLD_NOW | RTLD_NOLOAD);
#endif
  if (!dl)
  {
    pModuleManager->ErrorPrintf("Unable to open deathmatch.so: %s\n", dlerror());
    return false;
  }

  /*
   ** state manipulation
   */
  SAFE_IMPORT(lua_newstate);
  SAFE_IMPORT(lua_close);
  SAFE_IMPORT(lua_newthread);
  SAFE_IMPORT(lua_atpanic);

  /*
   ** basic stack manipulation
   */
  SAFE_IMPORT(lua_gettop);
  SAFE_IMPORT(lua_settop);
  SAFE_IMPORT(lua_pushvalue);
  SAFE_IMPORT(lua_remove);
  SAFE_IMPORT(lua_insert);
  SAFE_IMPORT(lua_replace);
  SAFE_IMPORT(lua_checkstack);
  SAFE_IMPORT(lua_xmove);

  /*
   ** access functions (stack -> C)
   */
  SAFE_IMPORT(lua_isnumber);
  SAFE_IMPORT(lua_isstring);
  SAFE_IMPORT(lua_iscfunction);
  SAFE_IMPORT(lua_isuserdata);
  SAFE_IMPORT(lua_type);
  SAFE_IMPORT(lua_typename);
  SAFE_IMPORT(lua_equal);
  SAFE_IMPORT(lua_rawequal);
  SAFE_IMPORT(lua_lessthan);
  SAFE_IMPORT(lua_tonumber);
  SAFE_IMPORT(lua_tointeger);
  SAFE_IMPORT(lua_toboolean);
  SAFE_IMPORT(lua_tolstring);
  SAFE_IMPORT(lua_objlen);
  SAFE_IMPORT(lua_tocfunction);
  SAFE_IMPORT(lua_touserdata);
  SAFE_IMPORT(lua_tothread);
  SAFE_IMPORT(lua_topointer);

  /*
   ** push functions (C -> stack)
   */
  SAFE_IMPORT(lua_pushnil);
  SAFE_IMPORT(lua_pushnumber);
  SAFE_IMPORT(lua_pushinteger);
  SAFE_IMPORT(lua_pushlstring);
  SAFE_IMPORT(lua_pushstring);
  SAFE_IMPORT(lua_pushvfstring);
  SAFE_IMPORT(lua_pushfstring);
  SAFE_IMPORT(lua_pushcclosure);
  SAFE_IMPORT(lua_pushboolean);
  SAFE_IMPORT(lua_pushlightuserdata);
  SAFE_IMPORT(lua_pushthread);

  /*
   ** get functions (Lua -> stack)
   */
  SAFE_IMPORT(lua_gettable);
  SAFE_IMPORT(lua_getfield);
  SAFE_IMPORT(lua_rawget);
  SAFE_IMPORT(lua_rawgeti);
  SAFE_IMPORT(lua_createtable);
  SAFE_IMPORT(lua_newuserdata);
  SAFE_IMPORT(lua_getmetatable);
  SAFE_IMPORT(lua_getfenv);

  /*
   ** set functions (stack -> Lua)
   */
  SAFE_IMPORT(lua_settable);
  SAFE_IMPORT(lua_setfield);
  SAFE_IMPORT(lua_rawset);
  SAFE_IMPORT(lua_rawseti);
  SAFE_IMPORT(lua_setmetatable);
  SAFE_IMPORT(lua_setfenv);

  /*
   ** `load' and `call' functions (load and run Lua code)
   */
  SAFE_IMPORT(lua_call);
  SAFE_IMPORT(lua_pcall);
  SAFE_IMPORT(lua_cpcall);
  SAFE_IMPORT(lua_load);
  SAFE_IMPORT(lua_dump);

  /*
   ** coroutine functions
   */
  SAFE_IMPORT(lua_yield);
  SAFE_IMPORT(lua_resume);
  SAFE_IMPORT(lua_status);

  /*
   ** garbage-collection function and options
   */
  SAFE_IMPORT(lua_gc);


  /*
   ** miscellaneous functions
   */
  SAFE_IMPORT(lua_error);
  SAFE_IMPORT(lua_next);
  SAFE_IMPORT(lua_concat);
  SAFE_IMPORT(lua_getallocf);
  SAFE_IMPORT(lua_setallocf);

  /* Functions to be called by the debuger in specific events */
  SAFE_IMPORT(lua_getstack);
  SAFE_IMPORT(lua_getinfo);
  SAFE_IMPORT(lua_getlocal);
  SAFE_IMPORT(lua_setlocal);
  SAFE_IMPORT(lua_getupvalue);
  SAFE_IMPORT(lua_setupvalue);
  SAFE_IMPORT(lua_sethook);
  SAFE_IMPORT(lua_gethook);
  SAFE_IMPORT(lua_gethookmask);
  SAFE_IMPORT(lua_gethookcount);

  /*
   ** Lua auxlib
   */
#if defined(LUA_COMPAT_GETN)
  SAFE_IMPORT(luaL_getn);
  SAFE_IMPORT(luaL_setn);
#endif
  SAFE_IMPORT(luaL_register);
  SAFE_IMPORT(luaL_getmetafield);
  SAFE_IMPORT(luaL_callmeta);
  SAFE_IMPORT(luaL_typerror);
  SAFE_IMPORT(luaL_argerror);
  SAFE_IMPORT(luaL_checklstring);
  SAFE_IMPORT(luaL_optlstring);
  SAFE_IMPORT(luaL_checknumber);
  SAFE_IMPORT(luaL_optnumber);
  SAFE_IMPORT(luaL_checkinteger);
  SAFE_IMPORT(luaL_optinteger);
  SAFE_IMPORT(luaL_checkstack);
  SAFE_IMPORT(luaL_checktype);
  SAFE_IMPORT(luaL_checkany);
  SAFE_IMPORT(luaL_newmetatable);
  SAFE_IMPORT(luaL_checkudata);
  SAFE_IMPORT(luaL_where);
  SAFE_IMPORT(luaL_error);
  SAFE_IMPORT(luaL_checkoption);
  SAFE_IMPORT(luaL_ref);
  SAFE_IMPORT(luaL_unref);
  SAFE_IMPORT(luaL_loadfile);
  SAFE_IMPORT(luaL_loadbuffer);
  SAFE_IMPORT(luaL_loadstring);
  SAFE_IMPORT(luaL_newstate);
  SAFE_IMPORT(luaL_gsub);
  SAFE_IMPORT(luaL_findtable);
  SAFE_IMPORT(luaL_buffinit);
  SAFE_IMPORT(luaL_prepbuffer);
  SAFE_IMPORT(luaL_addlstring);
  SAFE_IMPORT(luaL_addstring);
  SAFE_IMPORT(luaL_addvalue);
  SAFE_IMPORT(luaL_pushresult);

  SAFE_IMPORT(lua_getmainstate);

    return true;
}
#undef SAFE_IMPORT

/** typedefs **/

/*
** state manipulation
*/
typedef lua_State *(*lua_newstate_t)(lua_Alloc f, void *ud);
typedef void       (*lua_close_t)(lua_State *ls);
typedef lua_State *(*lua_newthread_t)(lua_State *ls);

typedef lua_CFunction (*lua_atpanic_t)(lua_State *ls, lua_CFunction panicf);


/*
** basic stack manipulation
*/
typedef int   (*lua_gettop_t)(lua_State *ls);
typedef void  (*lua_settop_t)(lua_State *ls, int idx);
typedef void  (*lua_pushvalue_t)(lua_State *ls, int idx);
typedef void  (*lua_remove_t)(lua_State *ls, int idx);
typedef void  (*lua_insert_t)(lua_State *ls, int idx);
typedef void  (*lua_replace_t)(lua_State *ls, int idx);
typedef int   (*lua_checkstack_t)(lua_State *ls, int sz);

typedef void  (*lua_xmove_t)(lua_State *from, lua_State *to, int n);

/*
** access functions (stack -> C)
*/

typedef int             (*lua_isnumber_t)(lua_State *ls, int idx);
typedef int             (*lua_isstring_t)(lua_State *ls, int idx);
typedef int             (*lua_iscfunction_t)(lua_State *ls, int idx);
typedef int             (*lua_isuserdata_t)(lua_State *ls, int idx);
typedef int             (*lua_type_t)(lua_State *ls, int idx);
typedef const char     *(*lua_typename_t)(lua_State *ls, int tp);

typedef int            (*lua_equal_t)(lua_State *ls, int idx1, int idx2);
typedef int            (*lua_rawequal_t)(lua_State *ls, int idx1, int idx2);
typedef int            (*lua_lessthan_t)(lua_State *ls, int idx1, int idx2);

typedef lua_Number      (*lua_tonumber_t)(lua_State *ls, int idx);
typedef lua_Integer     (*lua_tointeger_t)(lua_State *ls, int idx);
typedef int             (*lua_toboolean_t)(lua_State *ls, int idx);
typedef const char     *(*lua_tolstring_t)(lua_State *ls, int idx, size_t *len);
typedef size_t          (*lua_objlen_t)(lua_State *ls, int idx);
typedef lua_CFunction   (*lua_tocfunction_t)(lua_State *ls, int idx);
typedef void         *(*lua_touserdata_t)(lua_State *ls, int idx);
typedef lua_State      *(*lua_tothread_t)(lua_State *ls, int idx);
typedef const void     *(*lua_topointer_t)(lua_State *ls, int idx);


/*
** push functions (C -> stack)
*/
typedef void  (*lua_pushnil_t)(lua_State *ls);
typedef void  (*lua_pushnumber_t)(lua_State *ls, lua_Number n);
typedef void  (*lua_pushinteger_t)(lua_State *ls, lua_Integer n);
typedef void  (*lua_pushlstring_t)(lua_State *ls, const char *s, size_t l);
typedef void  (*lua_pushstring_t)(lua_State *ls, const char *s);
typedef const char *(*lua_pushvfstring_t) (lua_State *ls, const char *fmt, va_list argp);
typedef const char *(*lua_pushfstring_t)(lua_State *ls, const char *fmt, ...);
typedef void  (*lua_pushcclosure_t)(lua_State *ls, lua_CFunction fn, int n);
typedef void  (*lua_pushboolean_t)(lua_State *ls, int b);
typedef void  (*lua_pushlightuserdata_t)(lua_State *ls, void *p);
typedef int   (*lua_pushthread_t)(lua_State *ls);


/*
** get functions (Lua -> stack)
*/
typedef void  (*lua_gettable_t)(lua_State *ls, int idx);
typedef void  (*lua_getfield_t)(lua_State *ls, int idx, const char *k);
typedef void  (*lua_rawget_t)(lua_State *ls, int idx);
typedef void  (*lua_rawgeti_t)(lua_State *ls, int idx, int n);
typedef void  (*lua_createtable_t)(lua_State *ls, int narr, int nrec);
typedef void *(*lua_newuserdata_t)(lua_State *ls, size_t sz);
typedef int   (*lua_getmetatable_t)(lua_State *ls, int objindex);
typedef void  (*lua_getfenv_t)(lua_State *ls, int idx);

/*
** set functions (stack -> Lua)
*/
typedef void  (*lua_settable_t)(lua_State *ls, int idx);
typedef void  (*lua_setfield_t)(lua_State *ls, int idx, const char *k);
typedef void  (*lua_rawset_t)(lua_State *ls, int idx);
typedef void  (*lua_rawseti_t)(lua_State *ls, int idx, int n);
typedef int   (*lua_setmetatable_t)(lua_State *ls, int objindex);
typedef int   (*lua_setfenv_t)(lua_State *ls, int idx);


/*
** `load' and `call' functions (load and run Lua code)
*/
typedef void  (*lua_call_t)(lua_State *ls, int nargs, int nresults);
typedef int   (*lua_pcall_t)(lua_State *ls, int nargs, int nresults, int errfunc);
typedef int   (*lua_cpcall_t)(lua_State *ls, lua_CFunction func, void *ud);
typedef int   (*lua_load_t) (lua_State *ls, lua_Reader reader, void *dt, const char *chunkname);
typedef int (*lua_dump_t)(lua_State *ls, lua_Writer writer, void *data);


/*
** coroutine functions
*/
typedef int  (*lua_yield_t)(lua_State *ls, int nresults);
typedef int  (*lua_resume_t)(lua_State *ls, int narg);
typedef int  (*lua_status_t)(lua_State *ls);

/*
** garbage-collection function and options
*/
typedef int (*lua_gc_t)(lua_State *ls, int what, int data);

/*
** miscellaneous functions
*/
typedef int   (*lua_error_t)(lua_State *ls);
typedef int   (*lua_next_t)(lua_State *ls, int idx);
typedef void  (*lua_concat_t)(lua_State *ls, int n);
typedef lua_Alloc (*lua_getallocf_t) (lua_State *ls, void **ud);
typedef void (*lua_setallocf_t) (lua_State *ls, lua_Alloc f, void *ud);

/* Functions to be called by the debuger in specific events */
typedef int (*lua_getstack_t) (lua_State *ls, int level, lua_Debug *ar);
typedef int (*lua_getinfo_t) (lua_State *ls, const char *what, lua_Debug *ar);
typedef const char *(*lua_getlocal_t) (lua_State *ls, const lua_Debug *ar, int n);
typedef const char *(*lua_setlocal_t) (lua_State *ls, const lua_Debug *ar, int n);
typedef const char *(*lua_getupvalue_t) (lua_State *ls, int funcindex, int n);
typedef const char *(*lua_setupvalue_t) (lua_State *ls, int funcindex, int n);

typedef int (*lua_sethook_t) (lua_State *ls, lua_Hook func, int mask, int count);
typedef lua_Hook (*lua_gethook_t) (lua_State *ls);
typedef int (*lua_gethookmask_t) (lua_State *ls);
typedef int (*lua_gethookcount_t) (lua_State *ls);

/*
** Lua auxlib
*/
#if defined(LUA_COMPAT_GETN)
typedef int (*luaL_getn_t) (lua_State *ls, int t);
typedef void (*luaL_setn_t) (lua_State *ls, int t, int n);
#endif
typedef void (*luaL_register_t)(lua_State *ls, const char *libname, const luaL_Reg *l);
typedef int (*luaL_getmetafield_t)(lua_State *ls, int obj, const char *e);
typedef int (*luaL_callmeta_t)(lua_State *ls, int obj, const char *e);
typedef int (*luaL_typerror_t)(lua_State *ls, int narg, const char *tname);
typedef int (*luaL_argerror_t)(lua_State *ls, int numarg, const char *extramsg);
typedef const char *(*luaL_checklstring_t)(lua_State *ls, int numArg, size_t *l);
typedef const char *(*luaL_optlstring_t)(lua_State *ls, int numArg, const char *def, size_t *l);
typedef lua_Number (*luaL_checknumber_t)(lua_State *ls, int numArg);
typedef lua_Number (*luaL_optnumber_t)(lua_State *ls, int nArg, lua_Number def);

typedef lua_Integer (*luaL_checkinteger_t)(lua_State *ls, int numArg);
typedef lua_Integer (*luaL_optinteger_t)(lua_State *ls, int nArg, lua_Integer def);

typedef void (*luaL_checkstack_t)(lua_State *ls, int sz, const char *msg);
typedef void (*luaL_checktype_t)(lua_State *ls, int narg, int t);
typedef void (*luaL_checkany_t)(lua_State *ls, int narg);

typedef int   (*luaL_newmetatable_t)(lua_State *ls, const char *tname);
typedef void *(*luaL_checkudata_t)(lua_State *ls, int ud, const char *tname);

typedef void (*luaL_where_t)(lua_State *ls, int lvl);
typedef int (*luaL_error_t)(lua_State *ls, const char *fmt, ...);

typedef int (*luaL_checkoption_t)(lua_State *ls, int narg, const char *def, const char *const lst[]);

typedef int (*luaL_ref_t)(lua_State *ls, int t);
typedef void (*luaL_unref_t)(lua_State *ls, int t, int ref);

typedef int (*luaL_loadfile_t)(lua_State *ls, const char *filename);
typedef int (*luaL_loadbuffer_t)(lua_State *ls, const char *buff, size_t sz, const char *name);
typedef int (*luaL_loadstring_t)(lua_State *ls, const char *s);

typedef lua_State *(*luaL_newstate_t)(void);

typedef const char *(*luaL_gsub_t)(lua_State *ls, const char *s, const char *p, const char *r);

typedef const char *(*luaL_findtable_t)(lua_State *ls, int idx, const char *fname, int szhint);
typedef void (*luaL_buffinit_t)(lua_State *ls, luaL_Buffer *B);
typedef char *(*luaL_prepbuffer_t)(luaL_Buffer *B);
typedef void (*luaL_addlstring_t)(luaL_Buffer *B, const char *s, size_t l);
typedef void (*luaL_addstring_t)(luaL_Buffer *B, const char *s);
typedef void (*luaL_addvalue_t)(luaL_Buffer *B);
typedef void (*luaL_pushresult_t)(luaL_Buffer *B);

typedef lua_State* (*lua_getmainstate_t)(lua_State* L);


/** functions **/

#ifdef __cplusplus
extern "C" {
#endif

/*
** state manipulation
*/
#define LRET(f, ...) return ((f ## _t)p ## f)(__VA_ARGS__)
#define LCALL(f, ...) ((f ## _t)p ## f)(__VA_ARGS__)
lua_State *(lua_newstate) (lua_Alloc f, void *ud)
{
  LRET(lua_newstate, f, ud);
}

void       (lua_close) (lua_State *ls)
{
  LRET(lua_close, ls);
}

lua_State *(lua_newthread) (lua_State *ls)
{
  LRET(lua_newthread, ls);
}


lua_CFunction (lua_atpanic) (lua_State *ls, lua_CFunction panicf)
{
  LRET(lua_atpanic, ls, panicf);
}



/*
** basic stack manipulation
*/
int   (lua_gettop) (lua_State *ls)
{
  LRET(lua_gettop, ls);
}

void  (lua_settop) (lua_State *ls, int idx)
{
  LCALL(lua_settop, ls, idx);
}

void  (lua_pushvalue) (lua_State *ls, int idx)
{
  LCALL(lua_pushvalue, ls, idx);
}

void  (lua_remove) (lua_State *ls, int idx)
{
  LCALL(lua_remove, ls, idx);
}

void  (lua_insert) (lua_State *ls, int idx)
{
  LCALL(lua_insert, ls, idx);
}

void  (lua_replace) (lua_State *ls, int idx)
{
  LCALL(lua_replace, ls, idx);
}

int   (lua_checkstack) (lua_State *ls, int sz)
{
  LRET(lua_checkstack, ls, sz);
}


void  (lua_xmove) (lua_State *from, lua_State *to, int n)
{
  LCALL(lua_xmove, from, to, n);
}


/*
** access functions (stack -> C)
*/

int             (lua_isnumber) (lua_State *ls, int idx)
{
  LRET(lua_isnumber, ls, idx);
}

int             (lua_isstring) (lua_State *ls, int idx)
{
  LRET(lua_isstring, ls, idx);
}

int             (lua_iscfunction) (lua_State *ls, int idx)
{
  LRET(lua_iscfunction, ls, idx);
}

int             (lua_isuserdata) (lua_State *ls, int idx)
{
  LRET(lua_isuserdata, ls, idx);
}

int             (lua_type) (lua_State *ls, int idx)
{
  LRET(lua_type, ls, idx);
}

const char     *(lua_typename) (lua_State *ls, int tp)
{
  LRET(lua_typename, ls, tp);
}


int            (lua_equal) (lua_State *ls, int idx1, int idx2)
{
  LRET(lua_equal, ls, idx1, idx2);
}

int            (lua_rawequal) (lua_State *ls, int idx1, int idx2)
{
  LRET(lua_rawequal, ls, idx1, idx2);
}

int            (lua_lessthan) (lua_State *ls, int idx1, int idx2)
{
  LRET(lua_lessthan, ls, idx1, idx2);
}


lua_Number      (lua_tonumber) (lua_State *ls, int idx)
{
  LRET(lua_tonumber, ls, idx);
}

lua_Integer     (lua_tointeger) (lua_State *ls, int idx)
{
  LRET(lua_tointeger, ls, idx);
}

int             (lua_toboolean) (lua_State *ls, int idx)
{
  LRET(lua_toboolean, ls, idx);
}

const char     *(lua_tolstring) (lua_State *ls, int idx, size_t *len)
{
  LRET(lua_tolstring, ls, idx, len);
}

size_t          (lua_objlen) (lua_State *ls, int idx)
{
  LRET(lua_objlen, ls, idx);
}

lua_CFunction   (lua_tocfunction) (lua_State *ls, int idx)
{
  LRET(lua_tocfunction, ls, idx);
}

void         *(lua_touserdata) (lua_State *ls, int idx)
{
  LRET(lua_touserdata, ls, idx);
}

lua_State      *(lua_tothread) (lua_State *ls, int idx)
{
  LRET(lua_tothread, ls, idx);
}

const void     *(lua_topointer) (lua_State *ls, int idx)
{
  LRET(lua_topointer, ls, idx);
}



/*
** push functions (C -> stack)
*/
void  (lua_pushnil) (lua_State *ls)
{
  LCALL(lua_pushnil, ls);
}

void  (lua_pushnumber) (lua_State *ls, lua_Number n)
{
  LCALL(lua_pushnumber, ls, n);
}

void  (lua_pushinteger) (lua_State *ls, lua_Integer n)
{
  LCALL(lua_pushinteger, ls, n);
}

void  (lua_pushlstring) (lua_State *ls, const char *s, size_t l)
{
  LCALL(lua_pushlstring, ls, s, l);
}

void  (lua_pushstring) (lua_State *ls, const char *s)
{
  LCALL(lua_pushstring, ls, s);
}

const char *(lua_pushvfstring) (lua_State *ls, const char *fmt, va_list argp)
{
  LRET(lua_pushvfstring, ls, fmt, argp);
}

const char *(lua_pushfstring) (lua_State *ls, const char *fmt, ...)
{
  char buffer[1024];
  va_list vl;

  va_start(vl, fmt);
  vsnprintf(buffer, 1024, fmt, vl);
  va_end(vl);

  LRET(lua_pushfstring, ls, buffer);
}

void  (lua_pushcclosure) (lua_State *ls, lua_CFunction fn, int n)
{
  LCALL(lua_pushcclosure, ls, fn, n);
}

void  (lua_pushboolean) (lua_State *ls, int b)
{
  LCALL(lua_pushboolean, ls, b);
}

void  (lua_pushlightuserdata) (lua_State *ls, void *p)
{
  LCALL(lua_pushlightuserdata, ls, p);
}

int   (lua_pushthread) (lua_State *ls)
{
  LRET(lua_pushthread, ls);
}



/*
** get functions (Lua -> stack)
*/
void  (lua_gettable) (lua_State *ls, int idx)
{
  LCALL(lua_gettable, ls, idx);
}

void  (lua_getfield) (lua_State *ls, int idx, const char *k)
{
  LCALL(lua_getfield, ls, idx, k);
}

void  (lua_rawget) (lua_State *ls, int idx)
{
  LCALL(lua_rawget, ls, idx);
}

void  (lua_rawgeti) (lua_State *ls, int idx, int n)
{
  LCALL(lua_rawgeti, ls, idx, n);
}

void  (lua_createtable) (lua_State *ls, int narr, int nrec)
{
  LCALL(lua_createtable, ls, narr, nrec);
}

void *(lua_newuserdata) (lua_State *ls, size_t sz)
{
  LRET(lua_newuserdata, ls, sz);
}

int   (lua_getmetatable) (lua_State *ls, int objindex)
{
  LRET(lua_getmetatable, ls, objindex);
}

void  (lua_getfenv) (lua_State *ls, int idx)
{
  LCALL(lua_getfenv, ls, idx);
}


/*
** set functions (stack -> Lua)
*/
void  (lua_settable) (lua_State *ls, int idx)
{
  LCALL(lua_settable, ls, idx);
}

void  (lua_setfield) (lua_State *ls, int idx, const char *k)
{
  LCALL(lua_setfield, ls, idx, k);
}

void  (lua_rawset) (lua_State *ls, int idx)
{
  LCALL(lua_rawset, ls, idx);
}

void  (lua_rawseti) (lua_State *ls, int idx, int n)
{
  LCALL(lua_rawseti, ls, idx, n);
}

int   (lua_setmetatable) (lua_State *ls, int objindex)
{
  LRET(lua_setmetatable, ls, objindex);
}

int   (lua_setfenv) (lua_State *ls, int idx)
{
  LRET(lua_setfenv, ls, idx);
}



/*
** `load' and `call' functions (load and run Lua code)
*/
void  (lua_call) (lua_State *ls, int nargs, int nresults)
{
  LCALL(lua_call, ls, nargs, nresults);
}

int   (lua_pcall) (lua_State *ls, int nargs, int nresults, int errfunc)
{
  LRET(lua_pcall, ls, nargs, nresults, errfunc);
}

int   (lua_cpcall) (lua_State *ls, lua_CFunction func, void *ud)
{
  LRET(lua_cpcall, ls, func, ud);
}

int   (lua_load) (lua_State *ls, lua_Reader reader, void *dt, const char *chunkname)
{
  LRET(lua_load, ls, reader, dt, chunkname);
}

int (lua_dump) (lua_State *ls, lua_Writer writer, void *data)
{
  LRET(lua_dump, ls, writer, data);
}



/*
** coroutine functions
*/
int  (lua_yield) (lua_State *ls, int nresults)
{
  LRET(lua_yield, ls, nresults);
}

int  (lua_resume) (lua_State *ls, int narg)
{
  LRET(lua_resume, ls, narg);
}

int  (lua_status) (lua_State *ls)
{
  LRET(lua_status, ls);
}


/*
** garbage-collection function and options
*/
int (lua_gc) (lua_State *ls, int what, int data)
{
  LRET(lua_gc, ls, what, data);
}


/*
** miscellaneous functions
*/

int   (lua_error) (lua_State *ls)
{
  LRET(lua_error, ls);
}


int   (lua_next) (lua_State *ls, int idx)
{
  LRET(lua_next, ls, idx);
}


void  (lua_concat) (lua_State *ls, int n)
{
  LCALL(lua_concat, ls, n);
}


lua_Alloc (lua_getallocf) (lua_State *ls, void **ud)
{
  LRET(lua_getallocf, ls, ud);
}

void lua_setallocf (lua_State *ls, lua_Alloc f, void *ud)
{
  LCALL(lua_setallocf, ls, f, ud);
}


/* Functions to be called by the debuger in specific events */
int lua_getstack (lua_State *ls, int level, lua_Debug *ar)
{
  LRET(lua_getstack, ls, level, ar);
}

int lua_getinfo (lua_State *ls, const char *what, lua_Debug *ar)
{
  LRET(lua_getinfo, ls, what, ar);
}

const char *lua_getlocal (lua_State *ls, const lua_Debug *ar, int n)
{
  LRET(lua_getlocal, ls, ar, n);
}

const char *lua_setlocal (lua_State *ls, const lua_Debug *ar, int n)
{
  LRET(lua_setlocal, ls, ar, n);
}

const char *lua_getupvalue (lua_State *ls, int funcindex, int n)
{
  LRET(lua_getupvalue, ls, funcindex, n);
}

const char *lua_setupvalue (lua_State *ls, int funcindex, int n)
{
  LRET(lua_setupvalue, ls, funcindex, n);
}


int lua_sethook (lua_State *ls, lua_Hook func, int mask, int count)
{
  LRET(lua_sethook, ls, func, mask, count);
}

lua_Hook lua_gethook (lua_State *ls)
{
  LRET(lua_gethook, ls);
}

int lua_gethookmask (lua_State *ls)
{
  LRET(lua_gethookmask, ls);
}

int lua_gethookcount (lua_State *ls)
{
  LRET(lua_gethookcount, ls);
}


/**
 ** Lua auxlib
 **/
#if defined(LUA_COMPAT_GETN)
int (luaL_getn) (lua_State *ls, int t)
{
  LRET(luaL_getn, ls, t);
}

void (luaL_setn) (lua_State *ls, int t, int n)
{
  LRET(luaL_setn, ls, t, n);
}
#endif

void (luaL_register) (lua_State *ls, const char *libname, const luaL_Reg *l)
{
  LCALL(luaL_register, ls, libname, l);
}

int (luaL_getmetafield) (lua_State *ls, int obj, const char *e)
{
  LRET(luaL_getmetafield, ls, obj, e);
}

int (luaL_callmeta) (lua_State *ls, int obj, const char *e)
{
  LRET(luaL_callmeta, ls, obj, e);
}

int (luaL_typerror) (lua_State *ls, int narg, const char *tname)
{
  LRET(luaL_typerror, ls, narg, tname);
}

int (luaL_argerror) (lua_State *ls, int numarg, const char *extramsg)
{
  LRET(luaL_argerror, ls, numarg, extramsg);
}

const char *(luaL_checklstring) (lua_State *ls, int numArg, size_t *l)
{
  LRET(luaL_checklstring, ls, numArg, l);
}

const char *(luaL_optlstring) (lua_State *ls, int numArg, const char *def, size_t *l)
{
  LRET(luaL_optlstring, ls, numArg, def, l);
}

lua_Number (luaL_checknumber) (lua_State *ls, int numArg)
{
  LRET(luaL_checknumber, ls, numArg);
}

lua_Number (luaL_optnumber) (lua_State *ls, int nArg, lua_Number def)
{
  LRET(luaL_optnumber, ls, nArg, def);
}


lua_Integer (luaL_checkinteger) (lua_State *ls, int numArg)
{
  LRET(luaL_checkinteger, ls, numArg);
}

lua_Integer (luaL_optinteger) (lua_State *ls, int nArg, lua_Integer def)
{
  LRET(luaL_optinteger, ls, nArg, def);
}


void (luaL_checkstack) (lua_State *ls, int sz, const char *msg)
{
  LRET(luaL_checkstack, ls, sz, msg);
}

void (luaL_checktype) (lua_State *ls, int narg, int t)
{
  LCALL(luaL_checktype, ls, narg, t);
}

void (luaL_checkany) (lua_State *ls, int narg)
{
  LCALL(luaL_checkany, ls, narg);
}


int   (luaL_newmetatable) (lua_State *ls, const char *tname)
{
  LRET(luaL_newmetatable, ls, tname);
}

void *(luaL_checkudata) (lua_State *ls, int ud, const char *tname)
{
  LRET(luaL_checkudata, ls, ud, tname);
}


void (luaL_where) (lua_State *ls, int lvl)
{
  LCALL(luaL_where, ls, lvl);
}

int (luaL_error) (lua_State *ls, const char *fmt, ...)
{
  char buffer[1024];
  va_list vl;

  va_start(vl, fmt);
  vsnprintf(buffer, 1024, fmt, vl);
  va_end(vl);

  LRET(luaL_error, ls, buffer);
}


int (luaL_checkoption) (lua_State *ls, int narg, const char *def, const char *const lst[])
{
  LRET(luaL_checkoption, ls, narg, def, lst);
}


int (luaL_ref) (lua_State *ls, int t)
{
  LRET(luaL_ref, ls, t);
}

void (luaL_unref) (lua_State *ls, int t, int ref)
{
  LCALL(luaL_unref, ls, t, ref);
}


int (luaL_loadfile) (lua_State *ls, const char *filename)
{
  LRET(luaL_loadfile, ls, filename);
}

int (luaL_loadbuffer) (lua_State *ls, const char *buff, size_t sz, const char *name)
{
  LRET(luaL_loadbuffer, ls, buff, sz, name);
}

int (luaL_loadstring) (lua_State *ls, const char *s)
{
  LRET(luaL_loadstring, ls, s);
}


lua_State *(luaL_newstate) (void)
{
  LRET(luaL_newstate);
}



const char *(luaL_gsub) (lua_State *ls, const char *s, const char *p, const char *r)
{
  LRET(luaL_gsub, ls, s, p, r);
}


const char *(luaL_findtable) (lua_State *ls, int idx, const char *fname, int szhint)
{
  LRET(luaL_findtable, ls, idx, fname, szhint);
}


void (luaL_buffinit) (lua_State *ls, luaL_Buffer *B)
{
  LCALL(luaL_buffinit, ls, B);
}

char *(luaL_prepbuffer) (luaL_Buffer *B)
{
  LRET(luaL_prepbuffer, B);
}

void (luaL_addlstring) (luaL_Buffer *B, const char *s, size_t l)
{
  LCALL(luaL_addlstring, B, s, l);
}

void (luaL_addstring) (luaL_Buffer *B, const char *s)
{
  LCALL(luaL_addstring, B, s);
}

void (luaL_addvalue) (luaL_Buffer *B)
{
  LCALL(luaL_addvalue, B);
}

void (luaL_pushresult) (luaL_Buffer *B)
{
  LCALL(luaL_pushresult, B);
}

lua_State* (lua_getmainstate) (lua_State* L)
{
	LRET(lua_getmainstate, L);
}

#ifdef __cplusplus
}
#endif

#undef LRET
#undef LCALL

