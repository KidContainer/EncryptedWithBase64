#ifndef CODETHREAD_H
#define CODETHREAD_H

#include <QThread>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>


#include "base64.h"
#define encode_kind 0
#define decode_kind 1

#define nameInfo 256*1024
class MyThread : public QThread {
    Q_OBJECT
public:

    virtual ~MyThread(){
        QMessageBox msgBox;
        msgBox.setText("任务已完成，请去目标目录查看");
        msgBox.exec();
    }

    virtual void run(){
        if(codeKind==encode_kind) encodeFiles();
        else decodeFiles();

    }
    void setParameter(int kind,QStringList li, QString targetDir_str,QString resourceDir_str){
        this->codeKind = kind;
        files=li;
        this->targetDir_str = targetDir_str;
        this->resourceDir_str = resourceDir_str;
    }

signals:
    void progressBarChange(double i);//文件内完成度，发送一次，ProgressBar更新一次
    void oneFileHasFinished();//每完成一个文件，则发送一次


private:
    int codeKind;
    QStringList files;
    QString targetDir_str="";
    QString resourceDir_str = "";


    void decodeFiles(){
        for(QString file_str :files){
            decodeFile(file_str);
            emit oneFileHasFinished();
        }
    }

    void encodeFiles(){
        for(QString file_str : files){
            encodeFile(file_str);
            emit oneFileHasFinished();
        }
    }

    void decodeFile(QString& file_str){
        QFile resourceFile(file_str);
        double resourceFileSize = resourceFile.size(); //文件的大小
        if(!resourceFile.open(QIODevice::ReadOnly)){
            return;
        }
        QDataStream in(&resourceFile);
        //获取名字
        QString name;
        in>>name;

        QDir targetDir(this->targetDir_str);
        QFile targetFile(targetDir.absoluteFilePath(name));

        QFileInfo fileInfo(targetFile);
        if(!targetDir.mkpath(fileInfo.absolutePath())){
            return;
        }
        if(!targetFile.open(QIODevice::WriteOnly)){
            return;
        }

        //QDataStream out(&targetFile);
        //对文件进行解密，注意发送信号
        /*
        QByteArray encryptedData = resourceFile.readAll();
        auto data = decode(encryptedData);
        targetFile.write(data);
        */
        /*
        double workedSize = 0;
        while(!resourceFile.atEnd()){
            auto data = resourceFile.readLine();
            workedSize+= data.size();
            emit progressBarChange(workedSize/resourceFileSize);
            //data = decode(data);
            targetFile.write(data);
        }
        */
        double workedSize = 0;
        while(true){
            auto line = resourceFile.read(20*1024*1024);
            if(line.isEmpty()) break;
            workedSize+=line.size();
            emit progressBarChange(workedSize/resourceFileSize);
            auto s = QByteArray::fromBase64(line);
            targetFile.write(s);
        }
        resourceFile.close();
        targetFile.close();


    }

    void encodeFile(QString& file_str){

        QFile resourceFile(file_str);
        double resourceFileSize = resourceFile.size();//文件的大小
        if(!resourceFile.open(QIODevice::ReadOnly)){
            return;
        }
        //QDataStream in(&resourceFile);

        QDir resourceDir(resourceDir_str);

        QDir targetDir(targetDir_str);
        targetDir.mkdir("result");
        targetDir.cd("result");
        QFile targetFile(targetDir.path()+QString("\\")+QString::number(QDateTime::currentMSecsSinceEpoch()));
        if(!targetFile.open(QIODevice::WriteOnly)){
            return;
        }
        QDataStream out(&targetFile);

        QString name(resourceDir.relativeFilePath(file_str));
        out<<name;
        //完成对数据的加密操作,注意每步都要发送一个范围在0~1之间的数来更新progressBar


        //临时使用大内存读写
        /*
        QByteArray data = resourceFile.readAll();
        auto encryptData = encode(data);
        targetFile.write(encryptData);
        */
        /*
        double workedSize = 0;
        while(!resourceFile.atEnd()){
            auto data = resourceFile.readLine();
            workedSize += data.size();
            emit progressBarChange(workedSize/resourceFileSize);
            //data = encode(data);
            targetFile.write(data);
        }
        */



        static const int MAXBINSIZE = int(76/4)*3;
        double workSize = 0;
        while(true){
            auto s = resourceFile.read(MAXBINSIZE);
            if(s.isEmpty()){
                break;
            }
            while(s.length()<MAXBINSIZE){
                auto ns = resourceFile.read(MAXBINSIZE-s.length());
                if(ns.isEmpty()){
                    break;
                }
                s+=ns;
            }

            workSize+= s.size();
            emit progressBarChange(workSize/resourceFileSize);

            auto line = s.toBase64();
            targetFile.write(line);
        }


        resourceFile.close();
        targetFile.close();
    }


};

#endif // CODETHREAD_H
