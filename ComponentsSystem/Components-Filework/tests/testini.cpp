#include <string_view>

// AI-generated test
const std::string_view TEST_INI_DATA = R"(
; Test ini file

globval = myval ; Test if it's saved as a global section value

[COMMON]
emptyv =                                ; NULL value
nospacev=value with no spaces           ; No spaces between '='
  manyspace   =  surrounded by spaces   ; Many random spaces
equ_in_value = E=mc^2                   ; Equal sign in value
fake_section = [not a section]          ; Section-like value

[REGULAR]
key1 = value1
key2 = value2

[COMMENTS]
value = data # This must be a comment (not quoted)

[] ; NULL section 1
[ ] ; NULL section 2
[  ] ; NULL section 3
[ SPACES NAME 1 ]
[   SPACES NAME 2     ]
[DOT.NAME.SECTION]
[unclosed invalid section
[EXTRA BRACKETS]invalid]parse]
[ЮНИКОД]
[NEWLINE
PARSE INVALID]

; Expected - owerwrite previous or error
[DUBLICATE]
value = 0
[DUBLICATE]
value = 1

[INVALID]
= empty key
value with spaces = 123
just_key
true = false
false = true
on = off
off = on
unclosed_quote_single_1 = 'my value is not quoted!
unclosed_quote_single_2 = my value is not quoted!'
unclosed_quote_double_1 = "my value is not quoted!
unclosed_quote_double_2 = my value is not quoted!"

[QUOTES]
single_quote  = ' Single value '
single_quote_esc  = ' Single \'ESCAPED\' value '
double_quoted = " Double value "
double_quoted_esc = " Double \"ESCAPED\" value "
mixed_quote   = 'Example "value" of doubled'
single_comment = 'This ; is not a comment'
double_comment = "This ; is not a comment"

[MULTILINE]
multiline_default = My string
    value is
    multi-lined
    and supported
    cuz many lines read
)";


const std::string_view TEST_INI_EXPECTED_DATA = {
R"(globval = myval ; Test if it's saved as a global section value
[COMMENTS]
value = data ; This must be a comment (not quoted)
[COMMON]
emptyv =  ; NULL value
equ_in_value = E=mc^2 ; Equal sign in value
fake_section = [not a section] ; Section-like value
manyspace = surrounded by spaces ; Many random spaces
nospacev = value with no spaces ; No spaces between '='
[DUBLICATE]
value = 1
[INVALID]
false = true
off = on
on = off
true = false
unclosed_quote_double_1 = "my value is not quoted!
unclosed_quote_double_2 = my value is not quoted!"
unclosed_quote_single_1 = 'my value is not quoted!
unclosed_quote_single_2 = my value is not quoted!'
[MULTILINE]
multiline_default = My string
[QUOTES]
double_comment = "This ; is not a comment"
double_quoted = " Double value "
double_quoted_esc = " Double \"ESCAPED\" value "
mixed_quote = 'Example "value" of doubled'
single_comment = 'This ; is not a comment'
single_quote = ' Single value '
single_quote_esc = ' Single \'ESCAPED\' value '
[REGULAR]
key1 = value1
key2 = value2
)"
};