header {
    package xxc;
}

class PlusPlusCParser extends Parser;

options {
    k = 2;
    exportVocab = PlusPlusC;
    buildAST = true;
    defaultErrorHandler = false;
}

tokens {
    Node;
}

th1s^:
    "this";

constant^:
    HexInteger |
    DecInteger;

application^:
    LeftRound! (aexpression (Comma! aexpression)*)? RightRound!;

indirectOf^:
    At!;

addressOf^:
    Caret!;

memberOf^:
    Period! step;

applyTo^:
    application;

elementOf^:
    LeftSquare! sexpression RightSquare!;

newOperator^:
    "new"! (application)?;

n3w^:
    newOperator instantiation application;

atomicExpression^:
    (
        n3w |
        string |
        constant |
        name |
        auto
    ) (
        applyTo |
        indirectOf |
        addressOf |
        memberOf |
        elementOf
    )*;

multiplicativeOperator^:
    Star |
    Slash |
    Percent;

multiplicativeOperand^:
    multiplicativeOperator atomicExpression;

multiplicativeExpression^:
    atomicExpression (multiplicativeOperand)*;

additiveOperator^:
    Plus |
    Dash;

additiveOperand^:
    additiveOperator multiplicativeExpression;

additiveExpression^:
    multiplicativeExpression (additiveOperand)*;

shiftOperator^:
    LeftShift |
    RightShift;

shiftOperand^:
    shiftOperator additiveExpression;

shiftExpression^:
    additiveExpression (shiftOperand)*;

comparisonOperator^:
    LeftAngle |
    LeftAngleEqual |
    RightAngle |
    RightAngleEqual;

comparisonOperand^:
    comparisonOperator shiftExpression;

comparisonExpression^:
    shiftExpression (comparisonOperand)*;

equalityOperator^:
    EqualEqual |
    NotEqual;

equalityOperand^:
    equalityOperator comparisonExpression;

equalityExpression^:
    comparisonExpression (equalityOperand)*;

bitwiseAndOperator^:
    Ampersand;

bitwiseAndOperand^:
    bitwiseAndOperator equalityExpression;

bitwiseAndExpression^:
    equalityExpression (bitwiseAndOperand)*;

bitwiseXorOperator^:
    Bang;

bitwiseXorOperand^:
    bitwiseXorOperator bitwiseAndExpression;

bitwiseXorExpression^:
    bitwiseAndExpression (bitwiseXorOperand)*;

bitwiseOrOperator^:
    Pipe;

bitwiseOrOperand^:
    bitwiseOrOperator bitwiseXorExpression;

bitwiseOrExpression^:
    bitwiseXorExpression (bitwiseOrOperand)*;

logicalAndOperator^:
    AmpersandAmpersand;

logicalAndOperand^:
    logicalAndOperator bitwiseOrExpression;

logicalAndExpression^:
    bitwiseOrExpression (logicalAndOperand)*;

logicalOrOperator^:
    PipePipe;

logicalOrOperand^:
    logicalOrOperator logicalAndExpression;

logicalOrExpression^:
    logicalAndExpression (logicalOrOperand)*;

conditionOperand^:
    Question! logicalOrExpression Colon! logicalOrExpression;

conditionExpression^:
    logicalOrExpression (conditionOperand)*;

assignmentOperator^:
    Equal |
    PlusEqual |
    DashEqual |
    StarEqual |
    SlashEqual |
    PercentEqual |
    LeftShiftEqual |
    RightShiftEqual |
    AmpersandEqual |
    PipeEqual |
    CaretEqual;

assignmentOperand^:
    assignmentOperator conditionExpression;

assignmentExpression^:
    conditionExpression (assignmentOperand)*;

sequenceOperator^:
    Semicolon;

sequenceOperand^:
    sequenceOperator assignmentExpression;

sequenceExpression^:
    assignmentExpression (sequenceOperand)*;

aexpression:
    assignmentExpression;

texpression:
    Equal assignmentExpression |
    type;

sexpression:
    sequenceExpression;

instantiation^:
    LeftAngleSquare! (texpression (Comma! texpression)*)? RightAngleSquare!;

caster^:
    "cast"! LeftAngleSquare! type RightAngleSquare!;

pseudo^:
    Tilde! (id | caster) | "this";

step^:
    (id | pseudo) (instantiation)?;

name^:
    step (ColonColon! step)*;

parameter^:
    (id (initializer)? Colon!)? type;

id^:
    Identifier;

parameters:
    (parameter (Comma! parameter)*)?;

signature^:
    LeftRound! parameters RightRound! type;

pointerTo^:
    Caret! type_;

referenceTo^:
    Ampersand! type_;

arrayOf^:
    LeftSquare! (sexpression)? RightSquare! type_;

typename^:
    "typename"!;

concrete^:
    name | typename;

terminator^:
    Period!;

c0nst^:
    "const"! type_;

v0latile^:
    "volatile"! type_;

type_:
    concrete |
    pointerTo |
    referenceTo |
    arrayOf |
    c0nst |
    v0latile |
    templater type_ |
    signature |
    terminator;

type^:
    type_;

semantic^:
    ("static" | "const" | "virtual")?;

members^:
    (definition)*;

visibility^:
    "private" | "protected" | "public";

access^:
    "friend" | visibility;

region^:
    access semantic Colon! members;

extend^:
    visibility concrete;

tag^:
    "class" | "struct" | "union";

cl4ss^:
    tag (
        Colon! extend (Comma! extend)*
    )? (
        LeftCurly! (region)* RightCurly! |
        Semicolon!
    );

en2m^:
    "enum"! (LeftCurly!
        (declarator Semicolon!)*
    RightCurly!);

test^:
    sexpression;

empty^:
    Semicolon!;

body^:
    statement;

then^:
    statement;

e1se^:
    statement;

stated^:
    aexpression;

r3turn^:
    "return"! (aexpression)?;

c0ntinue^:
    "continue"!;

br3ak^:
    "break"!;

statement:
    typedef |
    block |
    empty |
    "while"^ LeftRound! test RightRound! body |
    "if"^ LeftRound! test RightRound! then (
        options { warnWhenFollowAmbig = false; }:
        "else"! e1se
    )? | (
        stated |
        r3turn |
        br3ak |
        c0ntinue
    ) Semicolon!;

statements:
    (statement)*;

block^:
    LeftCurly!
        statements
    RightCurly!;

modifier^:
    Identifier |
    "const" |
    "virtual";

attributer^:
    LeftSquare! name (application)? RightSquare!;

attributed^:
    definition_;

attribute^:
    attributer attributed;

modifiers:
    LeftRound! (modifier)* RightRound!;

function^:
    "function"! semantic (modifiers)? name Colon! type (
        (Colon! construction (Comma! construction)?)? block |
        Semicolon!
    );

typedef^:
    "typedef"! name Colon! (
        cl4ss |
        en2m |
        type Semicolon!
    );

initializer^:
    Equal! aexpression;

declarator^:
    name (initializer | application)? | application;

construction^:
    name application;

storage^:
    "auto" | "static";

auto^:
    storage declarator (Comma! declarator)* Colon! type;

templater^:
    "template"! LeftAngleSquare! parameters RightAngleSquare!;

templated^:
    definition;

template^:
    templater templated;

definition_:
    attribute |
    auto Semicolon! |
    function |
    template |
    typedef;

definition^:
    definition_;

string^:
    String;

include^:
    "#include"! String;

preproc:
    include;

hierarchy^:
    (definition | namespace | preproc)*;

namespace^:
    "namespace"! (name)? LeftCurly! hierarchy RightCurly!;

file:
    hierarchy EOF!;

class PlusPlusCLexer extends Lexer;

options {
    k = 3;
    exportVocab = PlusPlusC;
    testLiterals = false;
}

Caret: '^';
Star: '*';

LeftRound: '(';
RightRound: ')';

LeftCurly: '{';
RightCurly: '}';

LeftSquare: '[';
RightSquare: ']';

LeftAngle: '<';
RightAngle: '>';

LeftAngleSquare: "<[";
RightAngleSquare: "]>";

LeftShift: "<<";
RightShift: ">>";

Colon: ':';
ColonColon: "::";

Equal: '=';
EqualEqual: "==";
ColonEqual: ":=";

Semicolon: ';';
Period: '.';
Question: '?';

Comma: ',';
Ampersand: '&';

Plus: '+';
Dash: '-';

At: '@';
Bang: '!';

Tick: '`';
Tilde: '~';

PlusEqual: "+=";
DashEqual: "-=";
StarEqual: "*=";
SlashEqual: "/=";
PercentEqual: "%=";
LeftShiftEqual: "<<=";
RightShiftEqual: ">>=";
AmpersandEqual: "&=";
PipeEqual: "|=";
CaretEqual: "^=";

protected NewLine: ("\r\n" | '\n') { newline(); };
WhiteSpace: (' ' | '\t' | '\f' | NewLine) { $setType(Token.SKIP); };
SingleComment: "//" (~('\n' | '\r'))* NewLine { $setType(Token.SKIP); };

MultiComment:
    "/*" (
        options { generateAmbigWarnings=false; }:
        { LA(2) != '/' }? '*' |
        NewLine |
        ~('*' | '\n' | '\r')
    )* "*/" { $setType(Token.SKIP); };

protected AsciiLetter: 'a'..'z' | 'A'..'Z';
protected NonZeroDecDigit: '1'..'9';
protected DecDigit: '0' | NonZeroDecDigit;
protected HexDigit: '0'..'9' | 'a'..'f' | 'A'..'F';

DecInteger: NonZeroDecDigit (DecDigit)+ | DecDigit;
HexInteger: "0x" (HexDigit)+;

protected IdentifierCharacter: AsciiLetter | '_';
protected IdentifierStarter: IdentifierCharacter;
protected IdentifierContinuer: IdentifierCharacter | DecDigit;

String: '"' (Escape | ~('"' | '\\' | '\n' | '\r'))* '"';

Escape: '\\' '\\' /*XXX*/;

PreProc options {
    testLiterals = true;
}: '#' ('a'..'z')*;

Identifier options {
    testLiterals = true;
}: IdentifierStarter (IdentifierContinuer)*;
