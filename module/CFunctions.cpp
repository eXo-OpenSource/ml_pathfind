/*********************************************************
*
*  Multi Theft Auto: San Andreas - Deathmatch
*
*  ml_base, External lua add-on module
*
*  Copyright © 2003-2008 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 2002-2003 Rockstar North
*
*  THE FOLLOWING SOURCES ARE PART OF THE MULTI THEFT
*  AUTO SOFTWARE DEVELOPMENT KIT AND ARE RELEASED AS
*  OPEN SOURCE FILES. THESE FILES MAY BE USED AS LONG
*  AS THE DEVELOPER AGREES TO THE LICENSE THAT IS
*  PROVIDED WITH THIS PACKAGE.
*
*********************************************************/

#include "CFunctions.h"
#include "extra/CLuaArguments.h"
#include "WayFinderJobManager.h"

int CFunctions::calculateRouteBetweenPoints(lua_State* luaVM)
{
    if ( lua_type(luaVM, 1) != LUA_TNUMBER || lua_type(luaVM, 2) != LUA_TNUMBER || lua_type(luaVM, 3) != LUA_TNUMBER || 
        lua_type(luaVM, 4) != LUA_TNUMBER || lua_type(luaVM, 5) != LUA_TNUMBER || lua_type(luaVM, 6) != LUA_TNUMBER )
    {
        pModuleManager->ErrorPrintf("Bad argument @ calculateRouteBetweenPoints");
        lua_pushboolean(luaVM, false);
        return 1;
    }

    // run calculateRouteBetweenPoints(0, 0, 0, 10, 10, 10)

    // Create a new job
    WayFinderJob job;
    job.positionFrom.x = static_cast<float>(lua_tonumber(luaVM, 1));
    job.positionFrom.y = static_cast<float>(lua_tonumber(luaVM, 2));
    job.positionFrom.z = static_cast<float>(lua_tonumber(luaVM, 3));
    job.positionTo.x = static_cast<float>(lua_tonumber(luaVM, 4));
    job.positionTo.y = static_cast<float>(lua_tonumber(luaVM, 5));
    job.positionTo.z = static_cast<float>(lua_tonumber(luaVM, 6));

    // Add the job to the job queue
    JobId jobId = WayFinderJobManager::instance().addJob(job);

    // Return unique callback identifier
    lua_pushnumber(luaVM, jobId);
    return 1;
}

int CFunctions::processGPSEvents(lua_State* luaVM)
{
    // Process callbacks (required for thread synchronisation as we cannot call Lua stuff from a secondary thread)
    for (auto& result : WayFinderJobManager::instance().getResultCache())
    {
        // Push Lua function (triggerEvent) onto the stack (to be able to call it later)
        lua_getglobal(luaVM, "triggerEvent");

        // Push event name
        lua_pushstring(luaVM, "onGPSRouteCalculated");

        // Append job id
        JobId jobId = result.first;
        lua_pushnumber(luaVM, jobId);

        // Create a new table on the Lua stack
        lua_newtable(luaVM);
        size_t index = 0;

        auto& vectorList = result.second;
        for (auto& v : vectorList)
        {
            // Set table index
            lua_pushnumber(luaVM, ++index);

            // Create a table containing the vector components
            lua_newtable(luaVM);

            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, v.x);
            lua_settable(luaVM, -3);

            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, v.y);
            lua_settable(luaVM, -3);

            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, v.z);
            lua_settable(luaVM, -3);

            // Add vector table to result table
            lua_settable(luaVM, -3);
        }

        // Finally, call triggerEvent
        lua_call(luaVM, 2, 0);
    }

    lua_pushboolean(luaVM, true);
    return 1;
}