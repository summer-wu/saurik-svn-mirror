-- Copyright & Warranty Disclaimer {{{
{- C++0x - C++ Parser w/ Awesome Extensions
 - Copyright (C) 2005  Jay Freeman (saurik)
-}

{-
 -        Redistribution and use in source and binary
 - forms, with or without modification, are permitted
 - provided that the following conditions are met:
 -
 - 1. Redistributions of source code must retain the
 -    above copyright notice, this list of conditions
 -    and the following disclaimer.
 - 2. Redistributions in binary form must reproduce the
 -    above copyright notice, this list of conditions
 -    and the following disclaimer in the documentation
 -    and/or other materials provided with the
 -    distribution.
 - 3. The name of the author may not be used to endorse
 -    or promote products derived from this software
 -    without specific prior written permission.
 -
 - THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 - AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 - BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 - MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 - ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 - LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 - EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 - NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 - SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 - INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 - LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 - TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 - ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 - ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-}
-- }}}
-- Import Statements {{{
import System.Environment
import System.IO
import System.IO.Unsafe
import Text.Regex
-- }}}

data TokenType =
    HeaderName String |
    Identifier String |
    PreNumber String |

    WhiteSpace |

    CharacterLiteral String | WideCharacterLiteral String |
    StringLiteral String | WideStringLiteral String |

    Ampersand | AmpersandAmpersand | AmpersandEqual | Carrot | CarrotEqual |
    Colon | ColonColon | Comma | Dot | DotDotDot | DotStar | EqualEqual |
    Equal | ExclamationEqual | Exclamation | LeftAngle | LeftAngleEqual |
    LeftAngleLeftAngle | LeftAngleLeftAngleEqual | LeftBrace | LeftBracket |
    LeftParen | Minus | MinusEqual | MinusMinus | MinusRightAngle | MinusRightAngleStar |
    Percent | PercentEqual | Pipe | PipeEqual | PipePipe | Plus | PlusEqual |
    PlusPlus | Pound | PoundPound | Question | RightAngle | RightBrace | RightBracket |
    RightParen | RightAngleEqual | RightAngleRightAngle | RightAngleRightAngleEqual |
    SemiColon | Slash | SlashEqual | Star | StarEqual | Tilde

data Range = Range {
    rg_file_ :: String,
    rg_line_ :: Int,
    rg_begin_ :: Int,
    rg_size_ :: Int
}

data Input = Input {
    ip_value_ :: Char,
    ip_range_ :: Range
}

data Token = Token {
    tk_type_ :: TokenType,
    tk_ranges_ :: [Range]
}

-- instance Show Token {{{
instance Show Token where
    show Token {tk_type_ = PreNumber token} = "N:" ++ token
    show Token {tk_type_ = Identifier token} = "I:" ++ token
    show Token {tk_type_ = WhiteSpace} = "W:"

    show Token {tk_type_ = AmpersandAmpersand} = "O:&&"
    show Token {tk_type_ = AmpersandEqual} = "O:&="
    show Token {tk_type_ = Ampersand} = "O:&"
    show Token {tk_type_ = CarrotEqual} = "O:^="
    show Token {tk_type_ = Carrot} = "O:^"
    show Token {tk_type_ = ColonColon} = "O:::"
    show Token {tk_type_ = Colon} = "O::"
    show Token {tk_type_ = Comma} = "O:,"
    show Token {tk_type_ = DotStar} = "O:.*"
    show Token {tk_type_ = DotDotDot} = "O:..."
    show Token {tk_type_ = Dot} = "O:."
    show Token {tk_type_ = EqualEqual} = "O:=="
    show Token {tk_type_ = Equal} = "O:="
    show Token {tk_type_ = ExclamationEqual} = "O:!="
    show Token {tk_type_ = Exclamation} = "O:!"
    show Token {tk_type_ = LeftAngleEqual} = "O:<="
    show Token {tk_type_ = LeftAngleLeftAngleEqual} = "O:<<="
    show Token {tk_type_ = LeftAngleLeftAngle} = "O:<<"
    show Token {tk_type_ = LeftAngle} = "O:<"
    show Token {tk_type_ = LeftBrace} = "O:{"
    show Token {tk_type_ = LeftBracket} = "O:["
    show Token {tk_type_ = LeftParen} = "O:("
    show Token {tk_type_ = MinusEqual} = "O:-="
    show Token {tk_type_ = MinusMinus} = "O:--"
    show Token {tk_type_ = MinusRightAngleStar} = "O:->*"
    show Token {tk_type_ = MinusRightAngle} = "O:->"
    show Token {tk_type_ = Minus} = "O:-"
    show Token {tk_type_ = PercentEqual} = "O:%="
    show Token {tk_type_ = Percent} = "O:%"
    show Token {tk_type_ = PipeEqual} = "O:|="
    show Token {tk_type_ = PipePipe} = "O:||"
    show Token {tk_type_ = Pipe} = "O:|"
    show Token {tk_type_ = PlusEqual} = "O:+="
    show Token {tk_type_ = PlusPlus} = "O:++"
    show Token {tk_type_ = Plus} = "O:+"
    show Token {tk_type_ = PoundPound} = "O:##"
    show Token {tk_type_ = Pound} = "O:#"
    show Token {tk_type_ = Question} = "O:?"
    show Token {tk_type_ = RightAngleEqual} = "O:>="
    show Token {tk_type_ = RightAngleRightAngleEqual} = "O:>>="
    show Token {tk_type_ = RightAngleRightAngle} = "O:>>"
    show Token {tk_type_ = RightAngle} = "O:>"
    show Token {tk_type_ = RightBrace} = "O:}"
    show Token {tk_type_ = RightBracket} = "O:]"
    show Token {tk_type_ = RightParen} = "O:)"
    show Token {tk_type_ = SemiColon} = "O:;"
    show Token {tk_type_ = SlashEqual} = "O:/="
    show Token {tk_type_ = Slash} = "O:/"
    show Token {tk_type_ = StarEqual} = "O:*="
    show Token {tk_type_ = Star} = "O:*"
    show Token {tk_type_ = Tilde} = "O:~"
-- }}}
-- Character Sets {{{
isNonDigit c =
    (c >= 'a' && c <= 'z') ||
    (c >= 'A' && c <= 'Z') ||
    c == '_'

isDigit c =
    c >= '0' && c <= '9'

isWhite c =
    c == ' ' || c == '\t'
-- }}}

data SourceContext = SourceContext {
    sc_file_ :: String,
    sc_line_ :: Int,
    sc_byte_ :: Int,
    sc_offset_ :: Int
}

data TokenContext = TokenContext {
    tc_forMacro_ :: Bool,
    tc_forInclude_ :: Bool,
    tc_inIf_ :: Bool,
    tc_lineNumbers_ :: [Int],
    tc_source_ :: [Input]
}

-- TokenContext Manipulation {{{
getFirstCharacter :: TokenContext -> Char

getFirstCharacter context =
    ip_value_ $ head $ tc_source_ context

afterOneCharacter :: TokenContext -> TokenContext

afterOneCharacter context =
    context { tc_source_ = tail $ tc_source_ context }

afterWhiteSpace :: TokenContext -> TokenContext

afterWhiteSpace context
    | getFirstCharacter context == ' ' =
        afterWhiteSpace context

afterWhiteSpace context = context
-- }}}

unexpected context function =
    error $
        function ++
        ": (" ++
        (show $ head $ tc_lineNumbers_ context) ++
        "): unexpected '" ++
        [getFirstCharacter context] ++
        "'"

data Expression =
    Expression

data Catch =
    Catch {
        -- XXX: this is missing the argument
        ca_body_ :: Statement
    }

data Statement =
    Block {
        st_code_ :: [Statement]
    } | Do {
        st_body_ :: Statement,
        st_test_ :: Expression
    } | For {
        st_initialize_ :: Expression,
        st_test_ :: Expression,
        st_increment_ :: Expression,
        st_body_ :: Statement
    } | If {
        st_test_ :: Expression,
        st_then_ :: Statement,
        st_else_ :: Maybe Statement
    } | Try {
        st_body_ :: Statement,
        st_catches_ :: [Catch],
        st_finally_ :: Statement
    } | While {
        st_test_ :: Expression,
        st_body_ :: Statement
    }

type PreParser a = TokenContext -> (a, TokenContext)

-- Translation Phases #1-~3 (toSource) {{{
newLines :: Int -> String
newLines 0 = []
newLines lines =
    '\n' : newLines (lines - 1)

afterOldComment :: String -> (String, Int)

afterOldComment ('*' : '/' : rest) = (rest, 0)

afterOldComment ('\r' : '\n' : rest) =
    let (value, lines) = afterOldComment rest in
        (value, lines + 1)

afterOldComment ('\n' : rest) =
    let (value, lines) = afterOldComment rest in
        (value, lines + 1)

afterOldComment (first : rest) = afterOldComment rest

afterNewComment :: String -> (String, Int)

afterNewComment ('\\' : '\r' : '\n' : rest) =
    let (value, lines) = afterNewComment rest in
        (value, lines + 1)

afterNewComment ('\\' : '\n' : rest) =
    let (value, lines) = afterNewComment rest in
        (value, lines + 1)

afterNewComment ('\r' : '\n' : rest) = ('\n' : rest, 0)
afterNewComment ('\n' : rest) = ('\n' : rest, 0)

afterNewComment (first : rest) = afterNewComment rest
afterNewComment [] = ([], 0)

adjustSource :: SourceContext -> Int -> Bool -> SourceContext

adjustSource context characters newLine =
    context {
        sc_byte_ = sc_byte_ context + characters,
        sc_offset_ =
            if newLine then
                1
            else
                sc_offset_ context + characters
    }

mkInput :: SourceContext -> Char -> Int -> Input

mkInput context value size =
    Input {
        ip_value_ = value,
        ip_range_ = Range {
            rg_file_ = sc_file_ context,
            rg_line_ = sc_line_ context,
            rg_begin_ = sc_offset_ context,
            rg_size_ = size
        }
    }

addNewLine :: SourceContext -> [Input]

addNewLine context =
    [mkInput context '\n' 1]

toSource'' :: SourceContext -> Char -> Int -> String -> [Input]

toSource'' context first size rest =
    mkInput context first size : toSource' (adjustSource context size $ first == '\n') rest

toSource' :: SourceContext -> String -> [Input]
toSource' context ('?' : '?' : '='  : rest) = toSource'' context '#'  3 rest
toSource' context ('?' : '?' : '/'  : rest) = toSource'' context '\\' 3 rest
toSource' context ('?' : '?' : '\'' : rest) = toSource'' context '^'  3 rest
toSource' context ('?' : '?' : '('  : rest) = toSource'' context '['  3 rest
toSource' context ('?' : '?' : ')'  : rest) = toSource'' context ']'  3 rest
toSource' context ('?' : '?' : '!'  : rest) = toSource'' context '|'  3 rest
toSource' context ('?' : '?' : '<'  : rest) = toSource'' context '{'  3 rest
toSource' context ('?' : '?' : '>'  : rest) = toSource'' context '}'  3 rest
toSource' context ('?' : '?' : '-'  : rest) = toSource'' context '~'  3 rest

-- XXX: calculate offsets
toSource' context ('/' : '*' : rest) =
    let (rest', lines') = afterOldComment rest in
        toSource'' context ' ' 0 rest'

-- XXX: calculate offsets
toSource' context ('/' : '/' : rest) =
    let (rest', lines') = afterNewComment rest in
        toSource'' context ' ' 0 rest'

toSource' context ('\\' : '\r' : '\n' : rest) =
    toSource' (adjustSource context 3 True) rest

toSource' context ('\\' : '\n' : rest) =
    toSource' (adjustSource context 2 True) rest

toSource' context ['\\'] = addNewLine context
toSource' context ['\n'] = addNewLine context
toSource' context [] = addNewLine context

toSource' context ('\t' : rest) =
    toSource'' context ' ' 1 rest

toSource' context ('\r' : '\n' : rest) =
    toSource'' context '\n' 2 rest

toSource' context ('\n' : rest) =
    toSource'' context '\n' 1 rest

-- XXX: universal character encoding
toSource' context (first : rest) =
    toSource'' context first 1 rest

toSource :: String -> String -> [Input]
toSource file = toSource' SourceContext {
    sc_file_ = file,
    sc_line_ = 0,
    sc_byte_ = 0,
    sc_offset_ = 0
}
-- }}}
-- Translation Phases #~3-4 (toTokens) {{{

mkToken :: TokenContext -> TokenType -> Token

-- XXX: this is missing a _lot_ of stuff
mkToken context _type =
    Token {
        tk_type_ = _type,
        tk_ranges_ = []
    }

-- Numeric Parsing {{{
getPreNumber :: PreParser String

getPreNumber context@(TokenContext {tc_source_ = ('e' : sign : rest)})
    | sign == '+' || sign == '-' =
        let (value, context') = (getPreNumber context {tc_source_ = rest}) in
            ('e' : sign : value, context')

getPreNumber context@(TokenContext {tc_source_ = (first : rest)})
    | isDigit first || isNonDigit first || first == '.' =
        let (value, context') = (getPreNumber context {tc_source_ = rest}) in
            (first : value, context')

getPreNumber context = ("", context)
-- }}}
-- Identifier Parsing {{{
getIdentifier'' :: TokenContext -> (String, TokenContext)
getIdentifier'' context@(TokenContext {tc_source_ = (first : rest)})
    | isNonDigit first || isDigit first =
        getIdentifier' context

getIdentifier'' context = ("", context)

getIdentifier' :: TokenContext -> (String, TokenContext)
getIdentifier' context@(TokenContext {tc_source_ = (first : rest)}) =
    let (value, context') = getIdentifier'' context {tc_source_ = rest} in
        (first : value, context')

getIdentifier :: TokenContext -> (String, TokenContext)

getIdentifier context@(TokenContext {tc_source_ = (first : rest)})
    | isNonDigit first =
        getIdentifier' context

getIdentifier context =
    unexpected context "getIdentifier"
-- }}}

getToken :: PreParser (Maybe Token)

getToken context@(TokenContext {tc_source_ = all@('.' : first : _)})
    | isDigit first =
        let (value, context') = getPreNumber context in
            (Just $ mkToken context $ PreNumber value, context')

-- Operator Parsing {{{
getToken context@(TokenContext {tc_source_ = ('<' : '%' : rest)}) = (Just $ mkToken context LeftBrace, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('%' : '>' : rest)}) = (Just $ mkToken context RightBrace, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('<' : ':' : rest)}) = (Just $ mkToken context LeftBracket, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = (':' : '>' : rest)}) = (Just $ mkToken context RightBracket, context {tc_source_ = rest})

getToken context@(TokenContext {tc_source_ = ('%' : ':' : '%' : ':' : rest)}) = (Just $ mkToken context PoundPound, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('%' : ':' : rest)}) = (Just $ mkToken context Pound, context {tc_source_ = rest})

getToken context@(TokenContext {tc_source_ = ('&' : '&' : rest)}) = (Just $ mkToken context AmpersandAmpersand, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('&' : '=' : rest)}) = (Just $ mkToken context AmpersandEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('&' : rest)}) = (Just $ mkToken context Ampersand, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('^' : '=' : rest)}) = (Just $ mkToken context CarrotEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('^' : rest)}) = (Just $ mkToken context Carrot, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = (':' : ':' : rest)}) = (Just $ mkToken context ColonColon, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = (':' : rest)}) = (Just $ mkToken context Colon, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = (',' : rest)}) = (Just $ mkToken context Comma, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('.' : '*' : rest)}) = (Just $ mkToken context DotStar, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('.' : '.' : '.' : rest)}) = (Just $ mkToken context DotDotDot, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('.' : rest)}) = (Just $ mkToken context Dot, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('=' : '=' : rest)}) = (Just $ mkToken context EqualEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('=' : rest)}) = (Just $ mkToken context Equal, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('!' : '=' : rest)}) = (Just $ mkToken context ExclamationEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('!' : rest)}) = (Just $ mkToken context Exclamation, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('<' : '=' : rest)}) = (Just $ mkToken context LeftAngleEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('<' : '<' : '=' : rest)}) = (Just $ mkToken context LeftAngleLeftAngleEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('<' : '<' : rest)}) = (Just $ mkToken context LeftAngleLeftAngle, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('<' : rest)}) = (Just $ mkToken context LeftAngle, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('{' : rest)}) = (Just $ mkToken context LeftBrace, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('[' : rest)}) = (Just $ mkToken context LeftBracket, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('(' : rest)}) = (Just $ mkToken context LeftParen, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('-' : '=' : rest)}) = (Just $ mkToken context MinusEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('-' : '-' : rest)}) = (Just $ mkToken context MinusMinus, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('-' : '>' : '*' : rest)}) = (Just $ mkToken context MinusRightAngleStar, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('-' : '>' : rest)}) = (Just $ mkToken context MinusRightAngle, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('-' : rest)}) = (Just $ mkToken context Minus, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('%' : '=' : rest)}) = (Just $ mkToken context PercentEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('%' : rest)}) = (Just $ mkToken context Percent, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('|' : '=' : rest)}) = (Just $ mkToken context PipeEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('|' : '|' : rest)}) = (Just $ mkToken context PipePipe, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('|' : rest)}) = (Just $ mkToken context Pipe, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('+' : '=' : rest)}) = (Just $ mkToken context PlusEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('+' : '+' : rest)}) = (Just $ mkToken context PlusPlus, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('+' : rest)}) = (Just $ mkToken context Plus, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('#' : '#' : rest)}) = (Just $ mkToken context PoundPound, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('#' : rest)}) = (Just $ mkToken context Pound, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('?' : rest)}) = (Just $ mkToken context Question, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('>' : '=' : rest)}) = (Just $ mkToken context RightAngleEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('>' : '>' : '=' : rest)}) = (Just $ mkToken context RightAngleRightAngleEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('>' : '>' : rest)}) = (Just $ mkToken context RightAngleRightAngle, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('>' : rest)}) = (Just $ mkToken context RightAngle, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('}' : rest)}) = (Just $ mkToken context RightBrace, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = (']' : rest)}) = (Just $ mkToken context RightBracket, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = (')' : rest)}) = (Just $ mkToken context RightParen, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = (';' : rest)}) = (Just $ mkToken context SemiColon, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('/' : '=' : rest)}) = (Just $ mkToken context SlashEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('/' : rest)}) = (Just $ mkToken context Slash, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('*' : '=' : rest)}) = (Just $ mkToken context StarEqual, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('*' : rest)}) = (Just $ mkToken context Star, context {tc_source_ = rest})
getToken context@(TokenContext {tc_source_ = ('~' : rest)}) = (Just $ mkToken context Tilde, context {tc_source_ = rest})
-- }}}

getToken context@(TokenContext {tc_source_ = (' ' : ' ' : rest)}) =
    getToken context {tc_source_ = (' ' : rest)}

getToken context@(TokenContext {tc_source_ = (' ' : rest)})
    | tc_forMacro_ context = (Just $ mkToken context WhiteSpace, context')
    | True = getToken context'
    where context' = context {tc_source_ = rest}

getToken context@(TokenContext {tc_source_ = (first : _)})
    | isNonDigit first =
        let (value, context') = getIdentifier' context in
            (Just $ mkToken context $ Identifier value, context')
    | isDigit first =
        let (value, context') = getPreNumber context in
            (Just $ mkToken context $ PreNumber value, context')

getToken context@(TokenContext {tc_source_ = []}) =
    (Nothing, context)

getToken context =
    unexpected context "getToken"

toTokens' :: TokenContext -> [Token]

-- XXX: count line numbers (like we used to!)
toTokens' context
    | getFirstCharacter context == '\n' =
        toTokens $ afterOneCharacter context

toTokens' context =
    let (value, context') = getToken context in
        case value of
            Nothing -> []
            Just token -> token : toTokens' context'

toTokens :: TokenContext -> [Token]

toTokens context
    | getFirstCharacter context == '#' =
        let context' = afterWhiteSpace context in
            if getFirstCharacter context' == '\n' then
                toTokens' context'
            else let
                (name, context'') = getIdentifier context'
                context''' = afterWhiteSpace context''
            in case name of
                "define" -> preDefine context'''
                "elif" -> preElif context'''
                "else" -> preElse context'''
                "endif" -> preEndIf context'''
                "error" -> preError context'''
                "if" -> preIf context'''
                "ifdef" -> preIfDef context'''
                "ifndef" -> preIfNdef context'''
                "include" -> preInclude context'''
                "line" -> preLine context'''
                "pragma" -> prePragma context'''
                "undef" -> preUndef context'''

toTokens context =
    toTokens' context
-- }}}
-- Token Debug Output {{{
putTokens :: [Token] -> IO ()

putTokens [token] = do {
    putStr (show token);
    return ()
}

putTokens (first : rest) = do {
    putStr (show first);
    putChar ' ';
    putTokens rest;
    return ()
}

putTokens [] = return ()
-- }}}
-- Pre-processing Directives {{{
preDefine :: TokenContext -> [Token]
preDefine context = []
    {-let (name, after) = getIdentifier context (afterWhiteSpace rest) in
        case after of
            ('(' : _) ->
                error "function-like macro"
            _ -> preDefine' Nothing after-}

preElif :: TokenContext -> [Token]
preElif context = error "#elif"

preElse :: TokenContext -> [Token]
preElse context = error "#else"

preEndIf :: TokenContext -> [Token]
preEndIf context = error "#endif"

preError :: TokenContext -> [Token]
preError context = error "#error"

preIf :: TokenContext -> [Token]
preIf context = error "#if"

preIfDef :: TokenContext -> [Token]
preIfDef context = error "#ifdef"

preIfNdef :: TokenContext -> [Token]
preIfNdef context = error "#ifndef"

preInclude :: TokenContext -> [Token]
preInclude context =
    toTokens context {tc_source_ = getSourceSet "wow.h" ++ tc_source_ context}

preLine :: TokenContext -> [Token]
preLine context = error "#line"

prePragma :: TokenContext -> [Token]
prePragma context = error "#pragma"

preUndef :: TokenContext -> [Token]
preUndef context = error "#undef"
-- }}}

getSourceSet :: String -> [Input]

getSourceSet path =
    toSource path $ unsafePerformIO (openBinaryFile path ReadMode >>= hGetContents) ++ ['\n']

main :: IO ()

main = do {
    args <- getArgs;
    case args of
        [cpp] ->
            let
                source = getSourceSet cpp
                tokens = toTokens TokenContext {
                    tc_forMacro_ = False,
                    tc_forInclude_ = False,
                    tc_inIf_ = False,
                    tc_lineNumbers_ = [1],
                    tc_source_ = source
                }
            in do {
                putStrLn source;
                putTokens tokens;
                putStrLn ""
            }
}
