#ifndef WINDOWS_FUNCTION_H
#define WINDOWS_FUNCTION_H

#include <QObject>

class Windows_function:public  QObject
{
    Q_OBJECT
public:
    Windows_function(QObject *parent = nullptr);
    ~Windows_function();
    void WindowsSetting();
    void DevicCheck();
    void advancesharing(bool info);
signals:
     void setting_complemt(QList <QString >*);
public slots:
     void setting_work();
private:
    QList <QString> info_result_list;
};
#endif // WINDOWS_FUNCTION_H
