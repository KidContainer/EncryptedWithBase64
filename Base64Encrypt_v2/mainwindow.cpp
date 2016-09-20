#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->totalProgress->setRange(0,1000);
    ui->FileProgress->setRange(0,1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//按钮
void MainWindow::on_resourceButton_clicked()
{
    resourceDir = this->openFileDialog(QString::fromLocal8Bit("选择资源文件"));
    ui->resourceDir->setText(QDir::toNativeSeparators(resourceDir));
    if(!resourceDir.isEmpty()){
        QDir dir(resourceDir);
        auto fileNum = getFileNum(dir);
        ui->infoDisplayArea->setText(QString::fromLocal8Bit("资源目录下有：").append(QString::number(fileNum))+QString::fromLocal8Bit("个文件"));
    }
}

void MainWindow::on_targetButton_clicked()
{
    this->targetDir = this->openFileDialog(QString::fromLocal8Bit("选择目录文件"));
    ui->targetDir->setText(QDir::toNativeSeparators(targetDir));

}

void MainWindow::on_EncodeButton_clicked()
{
    if(checkDirEmpty()){
        return;
    }

    MyThread* thread = new MyThread;
    currentWorkingThread = thread;
    thread->setParameter(encode_kind,this->files,this->targetDir,this->resourceDir);
    connect(thread,SIGNAL(oneFileHasFinished()),this,SLOT(oneFileFinished()));
    connect(thread,SIGNAL(progressBarChange(double)),this,SLOT(oneFileProcess(double)));
    connect(thread,SIGNAL(finished()),this,SLOT(cleanUpThread()));
    thread->start();

}

void MainWindow::on_DecodeButton_clicked()
{
    if(checkDirEmpty()){
        return;
    }
    MyThread* thread = new MyThread;
    currentWorkingThread = thread;
    thread->setParameter(decode_kind,this->files,this->targetDir,this->resourceDir);
    connect(thread,SIGNAL(oneFileHasFinished()),this,SLOT(oneFileFinished()));
    connect(thread,SIGNAL(progressBarChange(double)),this,SLOT(oneFileProcess(double)));
    connect(thread,SIGNAL(finished()),this,SLOT(cleanUpThread()));
    thread->start();
}


//工具

QString MainWindow::openFileDialog(QString s){

    QFileDialog dialog(this,s);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setFileMode(QFileDialog::Directory);
    QStringList fileNames;
    if(dialog.exec()){
        fileNames = dialog.selectedFiles();
    }
    if(!fileNames.empty()){
        return fileNames.at(0);
    }
    else return "";

}

int MainWindow::getFileNum(QDir dir)
{
    int fileNum = 0;

    for(QFileInfo fileInfo : dir.entryInfoList()){
        if(fileInfo.isFile()){
            fileNum++;
            this->files.append(fileInfo.absoluteFilePath());
        }
        else{
            if(fileInfo.fileName()=="." || fileInfo.fileName() == "..")continue;
            QDir dir(fileInfo.absoluteFilePath());
            fileNum += getFileNum(dir);
        }

    }
    return fileNum;
}


bool MainWindow::checkDirEmpty()
{
    ui->totalProgress->setValue(0);
    if(this->targetDir.isEmpty() || this->resourceDir.isEmpty()){
        ui->infoDisplayArea->setText( QString::fromLocal8Bit("请选择文件夹"));
        return true;
    }
    else return false;
}

void MainWindow::oneFileProcess(double i){
    i*=1000;
    ui->FileProgress->setValue(i);
}

void MainWindow::oneFileFinished(){
    ui->FileProgress->setValue(0);
    auto num = this->files.length();
    finishedFileNum++;
    double i = double(finishedFileNum)/num*1000;
    ui->totalProgress->setValue(i);
}

void MainWindow::cleanUpThread()
{
    delete currentWorkingThread;
    currentWorkingThread = nullptr;
}
