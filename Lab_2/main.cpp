#include <QApplication>
#include <QQmlApplicationEngine>
#include "backend.h"
#include <QQmlContext>
#include <QDebug>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QProcess>
#include <iostream>


#include "windows.h"


using namespace std;


typedef unsigned long long QWORD;

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);


//        QProcess *satelliteProcess = new QProcess();
//        int pid = QApplication::applicationPid();
//        //qDebug() << pid;
//        QStringList arguments = {QString::number(pid)};
//        //qDebug() << arguments;
//        satelliteProcess -> start("C:/Users/Alexander/Desktop/stud/build-DebugProtect-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/debug/DebugProtect.exe", arguments);
//        bool protectorStarted = satelliteProcess -> waitForStarted(1000);
//        qDebug() << protectorStarted;


    QWORD moduleBase = (QWORD)GetModuleHandle(NULL); // начальный адрес, с которого приложение разм. в вирт. памяти
    QWORD text_segment_start = moduleBase + 0x1000; // адрес сегмента .text
    //qDebug() << "text_segment_start:" << Qt::hex << text_segment_start << "\n";

    PIMAGE_DOS_HEADER pIDH = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase); // расположение и размер первого заголовка
    PIMAGE_NT_HEADERS pINH = reinterpret_cast<PIMAGE_NT_HEADERS>(moduleBase + pIDH -> e_lfanew); // расположение и размер вторых двух заголовков
    QWORD size_of_text = pINH -> OptionalHeader.SizeOfCode; // величина .text
    //qDebug() << "size_of_text:" << size_of_text << "\n";

    QByteArray text_segment_contents = QByteArray((char*)text_segment_start, size_of_text);
    QByteArray current_hash = QCryptographicHash::hash(text_segment_contents, QCryptographicHash::Sha256);
    QByteArray current_hash_base64 = current_hash.toBase64();
    //qDebug() << "text_segment_contents:" << Qt::hex << text_segment_contents << "\n";
    qDebug() << "current_hash_base64:" << current_hash_base64 << "\n";

    const QByteArray hash0_base64 = QByteArray("Uq2xwUrbPBVG9EptX0SO2eBywB9QvPA7bLm+N/Y8Ph4=");

    bool checkresult = (current_hash_base64 == hash0_base64);
    qDebug() << "checkresult:" << checkresult << "\n";
        if (!checkresult){
            int result = QMessageBox::critical(nullptr, "Внимание!", "Приложение модифицировано");
            return -1;
        }




    qmlRegisterType<Backend>("Backend", 1, 0, "Backend"); //

    QQmlApplicationEngine engine;

    Backend Backend;    // Создаём ядро приложения
    QQmlContext *context = engine.rootContext();    // Создаём корневой контекст
    /* Загружаем объект в контекст для установки соединения,
        * а также определяем имя, по которому будет происходить соединение
        * */
    context->setContextProperty("Backend", &Backend);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

//    bool debuggerFound = IsDebuggerPresent();
//    if(debuggerFound){
//        qDebug() << "FOUND!!!";
//        cout << "FOUND!!!";
//        return -1;
//    }

    return app.exec();
}
