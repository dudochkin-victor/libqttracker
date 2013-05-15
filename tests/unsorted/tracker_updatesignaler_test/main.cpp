#ifndef MAIN_CPP
#define MAIN_CPP

#include <QApplication>

#include "tracker_updatesignaler_test.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TrackerUpdateSignalerTest test;

    app.exec();
}

#endif // MAIN_CPP
