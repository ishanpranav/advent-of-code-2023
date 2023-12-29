grammar day19a;

KEY
    : [a-z]+
    ;

NEW_LINE
    : [\n]+
    ;

PROPERTY
    : 'a' 
    | 'm' 
    | 's' 
    | 'x'
    ;

RELATION
    : '<' 
    | '>'
    ;

NUMBER
    : [0-9]+
    ;

TRUE
    : 'A'
    ;

FALSE
    : 'R'
    ;

ENTRY_POINT
    : 'i' 'n'
    ;
    
function_dictionary
    : (function NEW_LINE)*
    ;

function
    : (KEY | ENTRY_POINT) '{' ranges '}'
    ;

ranges
    : (range ',')* range
    ;

range
    : (identifier RELATION comparand ':' action) 
    | action;
    ;

identifier
    : PROPERTY
    ;

comparand
    : NUMBER
    ;

action
    : TRUE 
    | FALSE 
    | KEY
    ;

dynamic 
    : '{' (pair ',')* pair '}'
    ;

pair
    : PROPERTY '=' NUMBER
    ;
    