-- Copyright & Warranty Disclaimer {{{
{- C++0x - C++ Parser w/ Awesome Extensions
 - Copyright (C) 2001-2005  Jay Freeman (saurik)
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
    StringLiteral String | WideStringLiteral String

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

parseCpp0x a = return ()

-- instance Show Token {{{
instance Show Token where
    show Token {tk_type_ = PreNumber token} = "N:" ++ token
    show Token {tk_type_ = Identifier token} = "I:" ++ token
    show Token {tk_type_ = WhiteSpace} = "W:"
-- }}}
-- Input/Output {{{
getFileContents ::String -> String

getFileContents path =
    unsafePerformIO $ openBinaryFile path ReadMode >>= hGetContents
-- }}}
-- Haskell Entrypoint {{{
main :: IO ()

main = do {
    args <- getArgs;
    case args of
        [cpp] ->
            parseCpp0x $ getFileContents cpp
}
-- }}}
