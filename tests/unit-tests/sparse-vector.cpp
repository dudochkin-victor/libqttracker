/*
 * Copyright (C) 2010 Nokia Corporation.
 *
 * Contact: Mark Doffman <mark.doffman@codethink.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QObject>
#include <QString>
#include <QStringList>

#include "unit-tests.h"
#include <../../src/sparsevector_p.h>

/*
 * Compares the SparseVector with a Vector.  Expected gaps should be
 * represented with empty QStringList()s in $b.
 */
static bool compare_basic (QSparseVector <QStringList> a, QVector <QStringList> b) {
        bool equal = true;
        QSparseVector<QStringList>::iterator ai;
        QVector<QStringList>::iterator bi;

        for (ai = a.begin(), bi = b.begin(); (ai != a.end() && bi != b.end()) && (equal == true); ai++, bi++) {
			if (ai.isGap()) {
				if (*bi != QStringList())
					equal = false;
			}
			else if (*ai != *bi)
				equal = false;
        }
        return equal;
}

void UnitTests::sparsevector_size () {
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();
        v.append (QStringList () << "A" << "B");
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QCOMPARE (v.size(), (unsigned) 3);
}

void UnitTests::sparsevector_append () {
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();
        v.append (QStringList () << "A" << "B");
        QCOMPARE (*v.index (0), QStringList () << "A" << "B");

        QSparseVector <QStringList> v1 = QSparseVector <QStringList> ();
        v1.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QVERIFY (compare_basic (v1, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));

        QSparseVector <QStringList> v2 = QSparseVector <QStringList> ();
        v2.append (QStringList () << "A" << "B");
        v2.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QVERIFY (compare_basic (v2,
                     QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
}

void UnitTests::sparsevector_index () {
        QSparseVector <QString> v = QSparseVector <QString> ();

        v.append (QVector <QString> () << QString ("0"));
        v.append (QVector <QString> () << QString ("1"));
        v.append (QVector <QString> () << QString ("2"));
        v.append (QVector <QString> () << QString ("3"));
        v.append (QVector <QString> () << QString ("4"));
        v.append (QVector <QString> () << QString ("5"));

        QCOMPARE (*v.index(0), QString ("0"));
        QCOMPARE (*v.index(1), QString ("1"));
        QCOMPARE (*v.index(2), QString ("2"));
        QCOMPARE (*v.index(3), QString ("3"));
        QCOMPARE (*v.index(4), QString ("4"));
        QCOMPARE (*v.index(5), QString ("5"));

        //TODO Out-of-bounds Index
}

void UnitTests::sparsevector_find () {
        QSparseVector <QString> v = QSparseVector <QString> ();

        v.append (QVector <QString> () << QString ("0"));
        v.append (QVector <QString> () << QString ("1"));
        v.append (QVector <QString> () << QString ("2"));
        v.append (QVector <QString> () << QString ("3"));
        v.append (QVector <QString> () << QString ("4"));
        v.append (QVector <QString> () << QString ("5"));

        QCOMPARE (*v.find(0), QString ("0"));
        QCOMPARE (*v.find(1), QString ("1"));
        QCOMPARE (*v.find(2), QString ("2"));
        QCOMPARE (*v.find(3), QString ("3"));
        QCOMPARE (*v.find(4), QString ("4"));
        QCOMPARE (*v.find(5), QString ("5"));

        //TODO Out-of-bounds Find
}

void UnitTests::sparsevector_replace () {
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();

        // [[A, B]]
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B"));
        QCOMPARE (*v.index(0), QStringList () << "A" << "B");
        // [[C, D]]
        // Basic Replace
        v.replace (0, QStringList () << "C" << "D");
        QCOMPARE (*v.index(0), QStringList () << "C" << "D");
        // [[A,B], [C,D]]
        // Vector Replace
        v.replace (0, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QVERIFY (compare_basic (v, QVector <QStringList> ()
                                         << (QStringList () << "A" << "B")
                                         << (QStringList () << "C" << "D")));
        // [[G, H], [I, K]]
        // Iterator Replace
        QSparseVector <QStringList> r = QSparseVector <QStringList> ();
        r.append (QVector <QStringList> () << (QStringList () << "G" << "H") << (QStringList () << "I" << "K"));
        QVERIFY (compare_basic (r, QVector <QStringList> ()
                                         << (QStringList () << "G" << "H")
                                         << (QStringList () << "I" << "K")));
        v.replace (0, r.begin(), r.end());
        QVERIFY (compare_basic (v, QVector <QStringList> ()
                                   << (QStringList () << "G" << "H")
                                   << (QStringList () << "I" << "K")));
        // [[G,H], [I,K], [L, M]]
        // Valid Iterator
        QSparseVector <QStringList>::iterator i = v.replace (2, QStringList () << "L" << "M"); 
        QCOMPARE (*i, QStringList () << "L" << "M");
        // [[G,H], [I,K], [L, M], _, [N,O]]
        // Replace Non-Existing
        v.replace (4, QStringList () << "N" << "O");
        QCOMPARE (v.find(3), v.end());
        QCOMPARE (*v.index (4), QStringList () << "N" << "O");
}

void UnitTests::sparsevector_insert () {
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();

        // [[A,B], [C,D]]
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QVERIFY (compare_basic (v, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
        // [[A,B], [E, F], [C,D]]
        // Basic insert
        v.insert (1, QStringList () << "E" << "F");
        QVERIFY (compare_basic (v, QVector <QStringList> ()
                                     << (QStringList () << "A" << "B")
                                     << (QStringList () << "E" << "F")
                                     << (QStringList () << "C" << "D")));
        // [[G, H], [I, K], [A,B], [E, F], [C,D]]
        // Vector Insert
        v.insert (0, QVector <QStringList> () << (QStringList () << "G" << "H") << (QStringList () << "I" << "K"));
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                       << (QStringList () << "G" << "H")
                                       << (QStringList () << "I" << "K")
                                       << (QStringList () << "A" << "B")
                                       << (QStringList () << "E" << "F")
                                       << (QStringList () << "C" << "D")));
        // [[G, H], [I, K], [A,B], [E, F], [L, M], [N, O], [C,D]]
        // Iterator Insert
        QSparseVector <QStringList> r = QSparseVector <QStringList> ();
        r.append (QVector <QStringList> () << (QStringList () << "L" << "M") << (QStringList () << "N" << "O"));
        QVERIFY (compare_basic (r, QVector <QStringList> ()
                                         << (QStringList () << "L" << "M")
                                         << (QStringList () << "N" << "O")));

        v.insert (4, r.begin(), r.end());
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                       << (QStringList () << "G" << "H")
                                       << (QStringList () << "I" << "K")
                                       << (QStringList () << "A" << "B")
                                       << (QStringList () << "E" << "F")
                                       << (QStringList () << "L" << "M")
                                       << (QStringList () << "N" << "O")
                                       << (QStringList () << "C" << "D")));
        // [[P, Q], [G, H], [I, K], [A,B], [E, F], [L, M], [N, O], [C,D]]
        // Valid Iterator
        QSparseVector <QStringList>::iterator i = v.insert (0, QStringList () << "P" << "Q"); 
        QCOMPARE (*i, QStringList () << "P" << "Q");
        // [[P, Q], [G, H], [I, K], [A,B], [E, F], [L, M], [N, O], [C,D], _, _, [R,S]]
        // Insert Non-Existing
        v.insert (10, QStringList () << "R" << "S");
        QCOMPARE (v.find(8), v.end());
        QCOMPARE (v.find(9), v.end());
        QCOMPARE (*v.index (10), QStringList () << "R" << "S");
}

void UnitTests::sparsevector_reset () {
        //Resets a given position and size with a gap
        //Create a vector.
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        v.append (QVector <QStringList> () << (QStringList () << "E" << "F") << (QStringList () << "G" << "H"));
        v.append (QVector <QStringList> () << (QStringList () << "I" << "J") << (QStringList () << "K" << "L"));
        v.append (QVector <QStringList> () << (QStringList () << "M" << "N") << (QStringList () << "O" << "P"));
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                    << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")
                                    << (QStringList () << "E" << "F") << (QStringList () << "G" << "H")
                                    << (QStringList () << "I" << "J") << (QStringList () << "K" << "L")
                                    << (QStringList () << "M" << "N") << (QStringList () << "O" << "P")));
        //Reset the middle.
        v.reset (2, 4);
        //Check the size is correct.
        QCOMPARE (v.size(), (unsigned) 8);
        //Check the new values.
        //TODO what should the new values be here? How do I iterate and check this list?
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                    << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")
								<< QStringList()
								<< QStringList()
								<< QStringList()
								<< QStringList()
                                    << (QStringList () << "M" << "N") << (QStringList () << "O" << "P")));
}

void UnitTests::sparsevector_erase () {
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();
        //There is both a single and a range erase.
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        v.append (QVector <QStringList> () << (QStringList () << "E" << "F") << (QStringList () << "G" << "H"));
        v.append (QVector <QStringList> () << (QStringList () << "I" << "J") << (QStringList () << "K" << "L"));
        v.append (QVector <QStringList> () << (QStringList () << "M" << "N") << (QStringList () << "O" << "P"));
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                    << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")
                                    << (QStringList () << "E" << "F") << (QStringList () << "G" << "H")
                                    << (QStringList () << "I" << "J") << (QStringList () << "K" << "L")
                                    << (QStringList () << "M" << "N") << (QStringList () << "O" << "P")));
        //Erase, Check
        v.erase (v.find(7));
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                    << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")
                                    << (QStringList () << "E" << "F") << (QStringList () << "G" << "H")
                                    << (QStringList () << "I" << "J") << (QStringList () << "K" << "L")
                                    << (QStringList () << "M" << "N")));
        //Erase, Check
        v.erase (v.find(6));
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                    << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")
                                    << (QStringList () << "E" << "F") << (QStringList () << "G" << "H")
                                    << (QStringList () << "I" << "J") << (QStringList () << "K" << "L")));
        //Erase, Check.
        v.erase (v.find(5));
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                    << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")
                                    << (QStringList () << "E" << "F") << (QStringList () << "G" << "H")
                                    << (QStringList () << "I" << "J")));
        //Erase, Vector
        QSparseVector <QStringList>::iterator i = v.erase (v.find(1), v.find(3));
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                    << (QStringList () << "A" << "B") << (QStringList () << "G" << "H")
                                    << (QStringList () << "I" << "J")));
        //Valid iterator.
        QCOMPARE (*i, QStringList () << "G" << "H");
        //Erase on invalid indicies
        v.erase (v.end());
        QVERIFY (compare_basic (v, QVector <QStringList> () 
                                    << (QStringList () << "A" << "B") << (QStringList () << "G" << "H")
                                    << (QStringList () << "I" << "J")));
}

void UnitTests::sparsevector_clear () {
        //Clear removes everything from the sparse vector.
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();

        // [[A,B], [C,D]]
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QVERIFY (compare_basic (v, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
        // []
        v.clear ();
        QVERIFY (compare_basic (v, QVector <QStringList> ()));
}

void UnitTests::sparsevector_resize () {
        // Create a SparseVector.
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();

        // [[A,B], [C,D]]
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QVERIFY (compare_basic (v, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
        // Expand the size of it.
        v.resize (10);
        // Check that the size is correct.
        QCOMPARE (v.size(), (unsigned) 10);
        // Check for correct values
        // TODO How do I check the correct values here? Including the gaps that have been introduced?
        QVERIFY (compare_basic (v, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
        // Fill at the end.
        // Check size
        // Check correct values
        // Create large vector
        // Reduce size.
        // Check results are correct.
        // Check size.
}

void UnitTests::sparsevector_constructor () {
        //Create a vector
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();
        //Construct the sparse vector using the vector.
        QSparseVector <QStringList> r = QSparseVector <QStringList> (QVector <QStringList> () 
                                                                        << (QStringList () << "A" << "B")
                                                                        << (QStringList () << "C" << "D"));
        //Check that the sparsevector contains the correct information.
        QVERIFY (compare_basic (r, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
}

void UnitTests::sparsevector_copy_constructor () {
        //Create a new SparseVector.
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QVERIFY (compare_basic (v, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
        //Assign it to a new value.
        QSparseVector <QStringList> r (v);
        //Check that the second contains the correct data.
        QVERIFY (compare_basic (r, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
}

void UnitTests::sparsevector_assignment_op () {
        //Create a new SparseVector.
        QSparseVector <QStringList> v = QSparseVector <QStringList> ();
        QSparseVector <QStringList> r;

        // [[A,B], [C,D]]
        v.append (QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D"));
        QVERIFY (compare_basic (v, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
        //Assign it to a new value.
        r = v;
        //Check that the second contains the correct data.
        QVERIFY (compare_basic (r, QVector <QStringList> () << (QStringList () << "A" << "B") << (QStringList () << "C" << "D")));
}

void UnitTests::sparsevector_index_op () {
        QSparseVector <QString> v = QSparseVector <QString> ();

        v.append (QVector <QString> () << QString ("0"));
        v.append (QVector <QString> () << QString ("1"));
        v.append (QVector <QString> () << QString ("2"));
        v.append (QVector <QString> () << QString ("3"));
        v.append (QVector <QString> () << QString ("4"));
        v.append (QVector <QString> () << QString ("5"));

        QCOMPARE (v[0], QString ("0"));
        QCOMPARE (v[1], QString ("1"));
        QCOMPARE (v[2], QString ("2"));
        QCOMPARE (v[3], QString ("3"));
        QCOMPARE (v[4], QString ("4"));
        QCOMPARE (v[5], QString ("5"));

        //TODO Out-of-bounds Index
}
