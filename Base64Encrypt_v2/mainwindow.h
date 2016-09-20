#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QString>
#include <QFileDialog>
#include <QThread>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include <QDebug>

#include "codethread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:

    void oneFileFinished();

    void oneFileProcess(double i);

    void cleanUpThread();

private slots:
    void on_resourceButton_clicked();

    void on_targetButton_clicked();

    void on_EncodeButton_clicked();

    void on_DecodeButton_clicked();


private:
    Ui::MainWindow *ui;

    //属性
    QStringList files;
    QString targetDir = "";
    QString resourceDir = "";
    int finishedFileNum = 0;
    QThread* currentWorkingThread = nullptr;


    //成员函数
    QString openFileDialog(QString s);
    int getFileNum(QDir dir);
    bool checkDirEmpty();
    void encodeFiles();
    void decodeFiles();


};

#endif // MAINWINDOW_H
