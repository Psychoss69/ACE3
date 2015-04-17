/*
 * Author: jaynus
 *
 * Call and propegate a synced event
 *
 * Argument:
 * 0: Name (String)
 * 1: Arguments (Array)
 * 2: TTL (Number or Code) [Optional]
 * 
 * Return value:
 * Boolean of success
 */
#define DEBUG_MODE_FULL
#include "script_component.hpp"
PARAMS_3(_name,_args,_ttl);
private["_internalData", "_eventLog", "_eventCode"];

if(!HASH_HASKEY(GVAR(syncedEvents),_name)) exitWith {
    diag_log text format["[ACE] Error, synced event key not found."];
    false
};

_internalData = HASH_GET(GVAR(syncedEvents),_name);

if(isServer) then {
    // Server needs to internally log it for synchronization
    if(_ttl > -1) then {
        _internalData = HASH_GET(GVAR(syncedEvents),_name);
        _eventLog = _internalData select 1;
        _eventLog pushback [diag_tickTime, _args, _ttl];
    };
};

_eventCode = _internalData select 0;
_args call _eventCode;