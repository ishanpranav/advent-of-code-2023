grammar day19a;

KEY: [a-z]+;
NEW_LINE: [\n]+;
PROPERTY: 'a' | 'm' | 's' | 'x';
RELATION_OPERATOR: '<' | '>';
NUMBER: [0-9]+;
ACCEPT: 'A';
REJECT: 'R';

function_dictionary: (function NEW_LINE)*;
function: (KEY | entry_point) '{' (range ',')* range '}';
entry_point: 'i' 'n';
range: (identifier relation comparand ':' action) | action;
identifier: PROPERTY;
relation: RELATION_OPERATOR;
comparand: NUMBER;
action: ACCEPT | REJECT | KEY;