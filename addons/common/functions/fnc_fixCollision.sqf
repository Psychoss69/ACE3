/*
 * Author: commy2
 * Attempt to fix physx collisions causing unreasonable impact forces and damage.
 *
 * Arguments:
 * 0: Object <OBJECT>
 *
 * Return Value:
 * Nothing
 *
 */
#include "script_component.hpp"

// allowDamage requires local object
if (!local _this) exitWith {};

// prevent collision damage, @todo allowDamage API
_this allowDamage false;

// re-allow damage after 2 seconds
[{_this allowDamage true}, _this, 2, 0] call EFUNC(common,waitAndExecute);
