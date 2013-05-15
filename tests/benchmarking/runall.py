#!/usr/bin/python

import sys
import subprocess

from StringIO import StringIO
from xml.dom.minidom import parseString

def getResults (testExec):
        #r = subprocess.Popen ([testExec + ' -xml' + ' -iterations 5'], shell=True, stdout=subprocess.PIPE)
        r = subprocess.Popen ([testExec + ' -xml' + ' -iterations 2'], shell=True, stdout=subprocess.PIPE)
        return r.stdout.read()

benchmarks = ["rtcom",
              "gallery",
              "music",
              "email",
              "calls",
              "im",
              "location",
              "contacts",]
              #"content",

def main (argv):
        xmlset = []
        for name in benchmarks:
                sys.stderr.write("Running the %s tests\n" % (name))
                xmlset.append (getResults ("%s/%s-benchmark" % (name, name)))

        merged = StringIO()
        merged.write ("<TestSuite>")
        for xmlstr in xmlset:
                doc = parseString (xmlstr)
                tc = doc.getElementsByTagName ('TestCase')[0]
                ostr = tc.toxml().strip()
                merged.write (ostr)
        merged.write ("</TestSuite>")

        print merged.getvalue()

if __name__ == "__main__":
        sys.exit (main (sys.argv))
