#!/usr/bin/python2.5

from time import time
from subprocess import call

query = '''
select ?id ?url ?mime ?width ?height ?cam ?exp ?flash ?wb {
  ?id a nmm:Photo ;
      nie:url ?url ;
      nie:mimeType ?mime ;
      nfo:width ?width ;
      nfo:height ?height ;
      nmm:camera ?cam ;
      nmm:exposureTime ?exp ;
      nmm:flash ?flash ;
      nmm:whiteBalance ?wb .
}
'''

devnull = open('/dev/null', 'w')

def measure(*cmdline):
	t0 = time()
	ret = call(cmdline, stdout=devnull, stderr=devnull)
	t1 = time()
	return (t1 - t0, ret)

# def print_results(results):
# 	print '      case     time'
# 	print '-----------------------'
# 	for (case, (t, rc)) in results:
#		print '%10s %10.2fms' % (case, t * 1000)

def print_results(results):
		for (case, (t, rc)) in results:
				print case, int(t*1000)

results = []
results.append(('basic_dbusSend', measure('dbus-send', '--session', '--type=method_call', '--print-reply',
										  '--dest=org.freedesktop.Tracker1',
										  '/org/freedesktop/Tracker1/Resources',
										  'org.freedesktop.Tracker1.Resources.SparqlQuery',
										  "string:" + query)))
results.append(('basic_qtDBus', measure('./qdbus-query', query)))

print_results(results)
