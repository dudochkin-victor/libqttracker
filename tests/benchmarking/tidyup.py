import tidy
import sys

def tidy2xhtml(instream, outstream):
    options = dict(output_xhtml=1,
                   add_xml_decl=1,
                   indent=1
                   )
    tidied = tidy.parseString(instream.read(), **options)
    tidied.write(outstream)
    return

doc = open(sys.argv[1])

tidy2xhtml(doc, sys.stdout)
