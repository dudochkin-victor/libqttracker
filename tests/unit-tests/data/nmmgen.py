
def rdf (s):
        return "rdf:" + s

def rdfs (s):
        return "rdfs:" + s

def xsd (s):
        return "xsd:" + s

def dc (s):
        return "dc:" + s

def nie (s):
        return "nie:" + s

def nco (s):
        return "nco:" + s

def nfo (s):
        return "nfo:" + s

def nmm (s):
        return "nmm:" + s

def nao (s):
        return "nao:" + s

def literal (s):
        return "\"%s\"" % (s)


URI_TEMPLATE = "codethink:%s"

import uuid
import string

def resourceURI ():
        return URI_TEMPLATE % (string.replace(str(uuid.uuid4()), '-', ''))

class Resource:
        def __init__ (self, uri=None):
                self._tups = []
                if uri:
                        self._uri = uri
                else:
                        self._uri = resourceURI ()

                self.atup ('a', rdfs('Resource'))

        def atup (self, predicate, object):
                self._tups.append((self._uri, predicate, object))

        @property
        def uri (self):
                return self._uri

        @property
        def tups (self):
                return self._tups

import random

class NIE (Resource):
        def __init__ (self, of=None, uri=None):
                Resource.__init__ (self, uri)

                self.atup ('a', nie ('InformationElement'))
                if (of):
                        self.atup (nie('isLogicalPartOf'), of.uri)


class NMMArtist (NIE):
        def __init__ (self, of=None):
                NIE.__init__ (self, of)

                self.atup ('a', nmm ('Artist'))
                self.atup (nmm('artistName'), literal(self.randomArtist()))

        def randomArtist (self):
                poss = ['Jane',
                        'Doe',
                        'Mark',
                        'Mike',
                        'Alberto',
                        'Roberto',
                        'Steve',
                        'David',
                        'Luke',
                        'Jeffrey',
                        'Dommer',
                        'Kilikstan',
                        'Homer',
                        'J',
                        'Simpson',
                        'John']

                artist = []
                [artist.append(random.choice(poss)) for i in range (0, 3)]
                return ' '.join (artist)

class NMMMusicAlbum (NIE):
        def __init__ (self, artists, of=None):
                NIE.__init__ (self, of)

                self.atup ('a', nmm ('MusicAlbum'))
                self.atup (nmm ('albumTitle'), literal (self.randomAlbum()))
                self.atup (nmm ('albumArtist'), random.choice (artists).uri)

        def randomAlbum (self):
                poss = ['Led',
                        'Zepplin',
                        'I',
                        'II',
                        'III',
                        'IV',
                        'Bat out of hell',
                        'White',
                        'Sgt Peppers',
                        'Nevermind',
                        'Lillies',
                        'Coming on strong',
                        'Whatever you say I am',
                        'Thats what I\'m not',
                        'Fool',
                        'Terry']

                album = []
                [album.append(random.choice(poss)) for i in range (0, 3)]
                return ' '.join (album)

class NFOMedia (NIE):
        def __init__ (self, of=None):
                NIE.__init__ (self, of)

                self.atup ('a', nfo ('Media'))

class NMMMusicPiece (NFOMedia):
        def __init__ (self, album, num, of=None):
                NFOMedia.__init__ (self, of)

                self.atup ('a', nmm ('MusicPiece'))
                self.atup (nie ('title'), literal(self.randomTitle()))
                self.atup (nmm ('musicAlbum'), album.uri)
                self.atup (nmm ('trackNumber'), str(num))
                if random.random() > 0.3:
                        self.atup (nmm ('length'), str(random.randrange (0, 10, 1)))

        def randomTitle (self):
                poss = ['foo',
                        'bar',
                        'baz',
                        'grate',
                        'loreum',
                        'ipsum',
                        'dolor',
                        'sit',
                        'amet',
                        'consectetuer',
                        'elit',
                        'tellus',
                        'vitae',
                        'morbi',
                        'dictum',
                        'eget',
                        'auctor',
                        'magna']

                tit = []
                [tit.append(random.choice(poss)) for i in range (0, 5)]
                return ' '.join (tit)

class TTLDoc:

        prefixes = \
"""
@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .
@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .
@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .
@prefix dc: <http://purl.org/dc/elements/1.1/> .
@prefix nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> .
@prefix nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> .
@prefix nfo: <http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#> .
@prefix nmm: <http://www.tracker-project.org/temp/nmm#> .
@prefix nao: <http://www.semanticdesktop.org/ontologies/2007/08/15/nao#> .
@prefix codethink: <http://www.codethink.co.uk/tracker/test/uri#> .
"""

        def __init__ (self):
                self.subjects = {}
                self.ordered = []

        def addtuple (self, tup):
                subject, predicate, object = tup
                if subject in self.subjects:
                        self.subjects[subject] += [tup]
                else:
                        self.subjects[subject] = [tup]
                        self.ordered.append (subject)

        def addresource (self, rdfresource):
                for tup in rdfresource.tups:
                        self.addtuple (tup)

        def addresources (self, resources):
                [self.addresource (res) for res in resources]

        def ttl (self):
                ttlstr = self.prefixes + '\n\n'

                def cstr (pred, obj):
                        return '\t' + ' '.join ([pred, obj]) + ' ;\n'
                def estr (pred, obj):
                        return '\t' + ' '.join ([pred, obj]) + ' .\n\n'

                for subject in self.ordered:
                        subjstr = subject + '\n'
                        tuples = self.subjects[subject]
                        for subject, predicate, object in tuples [0:-1]:
                                subjstr += cstr (predicate, object)
                        subject, predicate, object = tuples [-1]
                        subjstr += estr (predicate, object)

                        ttlstr += subjstr

                return ttlstr 

def main (argv):
        doc = TTLDoc()
        space = NIE(uri='codethink:nmm-test-graph-001')
        artists = [NMMArtist(of=space) for i in range (0, 10)]
        albums = [NMMMusicAlbum(artists, of=space) for i in range (0, 40)]
        songs = []
        for album in albums:
                songs += [NMMMusicPiece (album, i, of=space) for i in range (1, 12)]
        
        doc.addresource  (space)
        doc.addresources (artists)
        doc.addresources (albums)
        doc.addresources (songs)

        fname = argv[1]
        f = open (fname, 'w')
        f.write (doc.ttl())

import sys

if __name__ == "__main__":
        sys.exit (main (sys.argv))
