#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

#include <model.hh>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Model model;

    return a.exec();
}
