#!/bin/sh
find -regex ".*\(cpp\|h\)" | xargs perl -0 -pi -n -e 's$\n([\t ]*)VZR_MEMBER_M_T_CV_N_C\(([^,;\n]*?)\,([^;\n]+?)\,([^,;\n]*?)\,(\s*\w+)\,([^,;\n]*)\)(\s*);(\s*)//(\s*)VZRX_MEMBER[^,;\n]*$\n\1\2\3\4\5\7;\8//\9VZR_MEMBER\6$mg'
find -regex ".*\(cpp\|h\)" | xargs perl -0 -pi -n -e 's$VZRX_CLASS([^;]*;)$/* VZR_CLASS\1*/$mg'

