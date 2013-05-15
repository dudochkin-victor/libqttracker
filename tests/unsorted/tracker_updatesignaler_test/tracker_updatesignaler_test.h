#ifndef TRACKER_UPDATESIGNALER_H
#define TRACKER_UPDATESIGNALER_H

#include <QObject>

class TrackerUpdateSignalerTest : public QObject
{
    Q_OBJECT
public:
    TrackerUpdateSignalerTest();
    ~TrackerUpdateSignalerTest();

public slots:
    void contentChanged(const QStringList &);
};

#endif //TRACKER_UPDATESIGNALER_H
