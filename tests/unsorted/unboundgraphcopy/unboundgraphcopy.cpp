#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>

using namespace SopranoLive;

int main (int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    RDFVariable subj, pred, obj;
    subj.property(pred) = obj;

    RDFVariableList copied = RDFVariable::deepCopy(RDFVariableList() << subj << pred << obj);

    subj = QUrl("http://test.com");

    RDFSelect sel;
    sel.addColumn("subj", subj);
    sel.addColumn("pred", pred);
    sel.addColumn("obj", obj);

    qDebug() << sel.getQuery();

    qDebug() << "Should differ from";

    RDFSelect sel2;
    sel2.addColumn("subj", copied[0]);
    sel2.addColumn("pred", copied[1]);
    sel2.addColumn("obj", copied[2]);

    qDebug() << sel2.getQuery();

    return 0;
}
