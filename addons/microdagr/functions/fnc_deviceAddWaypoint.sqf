/*
 * Author: PabstMirror
 * Takes some arguments and returns something or other.
 *
 * Arguments:
 * 0: The first argument <STRING>
 * 1: The second argument <OBJECT>
 * 2: Third Optional Argument <BOOL><OPTIONAL>
 *
 * Return Value:
 * The return value <BOOL>
 *
 * Example:
 * _bool = ["something", player] call ace_common_fnc_imanexample
 *
 * Public: Yes
 */
#include "script_component.hpp"

PARAMS_2(_waypointName,_waypointPosASL);

_waypoints = ace_player getVariable [QGVAR(waypoints), []];
_waypoints pushBack [_waypointName, _waypointPosASL];
ace_player setVariable [QGVAR(waypoints), _waypoints];
