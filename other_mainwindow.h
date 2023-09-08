#ifndef OTHER_MAINWINDOW_H
#define OTHER_MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class Other_MainWindow;
}

class Other_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Other_MainWindow(QWidget *parent = nullptr);
    ~Other_MainWindow();
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
private:
    Ui::Other_MainWindow *ui;
};
#endif // OTHER_MAINWINDOW_H
