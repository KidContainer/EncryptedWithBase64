#ifndef BASE64_H
#define BASE64_H
#include <QFile>
#include <QByteArray>
#include <QDebug>

#ifdef BASE64_LIB
#define BASE64_API __declspec(dllexport)
#else
#define BASE64_API __declspec(dllimport)
#endif


class BASE64_API base64
{
public:
    base64();
    static const int MAXBINSIZE = int(76/4)*3;

    static void encode(QFile& input, QFile& output){
        while(true){
            auto s = input.read(MAXBINSIZE);
            if(s.isEmpty()){
                break;
            }
            while(s.length()<MAXBINSIZE){
                auto ns = input.read(MAXBINSIZE-s.length());
                if(ns.isEmpty()){
                    break;
                }
                s+=ns;
            }
            auto line = s.toBase64();
            output.write(line);
        }
    }

static void decode(QFile& input, QFile& output){
        while(true){
            auto line = input.readLine();
            if(line.isEmpty()) break;
            auto s = QByteArray::fromBase64(line);
            output.write(s);
        }
    }


};

#endif // BASE64_H
