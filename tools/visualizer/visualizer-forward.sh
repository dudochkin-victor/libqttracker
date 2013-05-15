#!/bin/sh
find -regex '.*\(cpp\|h\)' | xargs perl -0 -pi -n -e 's$([\t ]*mutable)?([\t ]*\w[^\n;]*?)(([\t ]*?const|[\t ]*?volatile)*)\b([\t ]*\w+)([\t ]*);([\t ]*)//([\t ]*)VZR_MEMBER([^,;\n\r]*)\n$VZR_MEMBER_M_T_CV_N_C(\1,\2,\3,\5,\9)\6;\7//\8VZRX_MEMBER\7\n$mg'
find -regex '.*\(cpp\|h\)' | xargs perl -0 -pi -n -e 's$\/\*(\s*)VZR_CLASS((.|\n)*?)(\s*)\*\/$\1VZRX_CLASS\2\4$mg'

