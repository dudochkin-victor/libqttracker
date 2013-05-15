#!/usr/bin/python

import sys
import subprocess
import string

from StringIO import StringIO
from xml.dom.minidom import parseString, parse
from string import strip

#----- Extraction of the tracker results

class TestResult(object):
        __slots__ = [
                        'test',
                        'description',
                        'expectedvalue',
                        'actualvalue',
                        'numitems',
                    ]

        def __init__(self, data):
                self.test, self.description, self.expectedvalue, self.actualvalue, self.numitems = data


        def __str__(self):
                return (str(self.test) + '\n' +
                        str(self.description) + '\n' +
                        str(self.expectedvalue) + '\n' +
                        str(self.actualvalue) + '\n' +
                        str(self.numitems))

def getText (node):
        textnode = node.childNodes[0]
        return strip(textnode.data)

def parseRow (tr):
        tds = tr.getElementsByTagName ('td')

        if len (tds) != 7:
                return (None, None)

        testnode = tds[0]
        test = getText (testnode)

        descriptionnode = tds[1]
        description = getText (descriptionnode)

        expectedvaluenode = tds[2]
        expectedvalue = getText (expectedvaluenode)

        actualvaluenode = tds[3]
        actualvalue = eval(getText (actualvaluenode))
        avf = [float (v) for v in actualvalue]
        actualmean = sum (avf) / len (avf)
        actualmeanms = actualmean * 1000

        numitemsnode = tds[5]
        numitems = getText (numitemsnode)

        return (test, TestResult ((test, description, expectedvalue, actualmeanms, numitems)))

def parseTrackerResults (fname):

        doc = parse (fname)
        trs = doc.getElementsByTagName ('tr')

        parsed = {}
        for tr in trs:
                test, testresult = parseRow (tr)
                if test:
                        parsed [test] = testresult

        return parsed

# ------------------- Creation of the html report


null_tracker_result = TestResult (("", "", "0", 0, "0"))

def percentDifference (old, new):
        if old == 0:
                return 0
        diff = ((new - old) / old) * 100
        return int (diff)

def parseTestSuite (nodes, extra={}):
        htmlhead = \
"""
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
   <head>
      <title>LibQtTracker performance benchmarks</title>
   </head>
   <style type="text/css">
   body {
        font-family: "Helvetica Neue", "Lucida Grande", Helvetica, Arial, Verdana, sans-serif;
        margin-top: .5em; color: #444;
   }
   </style>
   <body>
"""

        htmlfoot = \
"""
   </body>
</html>
"""
        output = StringIO ()

        output.write (htmlhead)
        parseTestCase (nodes, output, extra)
        output.write (htmlfoot)

        return output.getvalue ()

def parseTestCase (nodes, output, extra={}):
        tchead = string.Template ( \
"""
        <div class="qttrackertestcase">
                <h1> $testname </h1>
                <table cellpadding="4" width="100%" style="border: 1px solid #000000; border-collapse: collapse;" border="1">
                        <tr>
                                <th>Function</th>
                                <th>Description</th>
                                <th>LibQtTracker</th>
                                <th>Tracker</th>
                                <th>% Difference</th>
                                <th>Expected</th>
                                <th>Iterations</th>
                        </tr>
"""
        )

        tcfoot = \
"""
                </table>
        </div>
"""
        tcs = nodes.getElementsByTagName ("TestCase")
        for tc in tcs:
                output.write (tchead.substitute ({'testname': tc.attributes['name'].value}))
                parseTestFunction (tc, output, extra)
                output.write (tcfoot)

def parseTestFunction (nodes, output, extra={}):
        tfs = nodes.getElementsByTagName ("TestFunction")
        for tf in tfs:
                parseBenchmarkResult (tf, output, extra)

def parseBenchmarkResult (tf, output, extra={}):
        tfbody = string.Template ( \
"""
                        <tr class="$result" style="background: $background">
                                <td> $function </td>
                                <td> $description </td>
                                <td> $qttime </td>
                                <td> $rawtime </td>
                                <td> $pdiff </td>
                                <td> $etime </td>
                                <td> $iterations </td>
                        </tr>
"""
        )
        bmr = tf.getElementsByTagName ('BenchmarkResult')
        if bmr:
                bmr = bmr[0]
                function = tf.attributes['name'].value
                exv = extra.get(function, null_tracker_result)

                description = exv.description
                time = bmr.attributes['value'].value
                etime = exv.expectedvalue
                iterations = bmr.attributes['iterations'].value

                result = "qttrackerfail"
                background = "#FF9999"
                if (etime == 0 or (etime >= int (time))):
                        result = "qttrackerpass"
                        background = "#FFFFFF"

                pdiff = percentDifference (exv.actualvalue, int(time))
                pdiffstr = "%d %%" % (pdiff)
                rawtime = exv.actualvalue

                bodyattr = {'function': function,
                            'description': description,
                            'qttime': time,
                            'rawtime': rawtime,
                            'etime': etime,
                            'pdiff': pdiffstr,
                            'iterations': iterations,
                            'result': result,
                            'background': background}

                output.write (tfbody.substitute (bodyattr))

#-----------------------------

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
        libqttrackerres = parse (argv[1])
        trackerres = parseTrackerResults (argv[2])

        print parseTestSuite (libqttrackerres, extra=trackerres)

if __name__ == "__main__":
        sys.exit (main (sys.argv))
