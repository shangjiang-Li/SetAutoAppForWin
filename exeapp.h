#ifndef EXEAPP_H
#define EXEAPP_H

#include <QObject>

class EXEAPP : public QObject
{
    Q_OBJECT
public:
    explicit EXEAPP(QObject *parent = nullptr);
    ~EXEAPP();
    void exeappRun(QString app);
signals:
    void phpsmcFinished(bool complete);
};

#endif // EXEAPP_H
