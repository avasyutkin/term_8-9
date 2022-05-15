#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <openssl/sha.h>



using namespace std;

class Backend : public QObject
{
    Q_OBJECT


public:
    explicit Backend(QObject *parent = nullptr);
    QStringList readJSON();
    void toClipboard(const std::string &s);
    void get_data(bool is_pass, QString site);
    bool crypt_data(unsigned char *sourcetext, unsigned char *ciphertext, int do_encrypt, unsigned char* key_user);
    bool encrypt_file(unsigned char* key_user);
    bool SHA256(unsigned char* input, unsigned long length, unsigned char* md);
    bool decrypt_file(unsigned char* key_user);
    //bool crypt_file(QString key_user, bool mode);
    QString unchar_to_qstr(char* unchar);
    unsigned char* qstr_to_unchar(QString qstr);
    void data_to_json();
    void backup();

private:
    QJsonObject json;
    string all_data;
    QString sourcefile = "C://Users//Alexander//Desktop//stud//Lab_2//Data.json";
    unsigned char md[SHA256_DIGEST_LENGTH]; // 32 bytes

signals:
    // для передачи данных в qml-интерфейс
    void send_sites_to_qml(QStringList count);
    void send_backup_to_qml(QStringList count);


public slots:
    // для приёма данных из qml-интерфейса
    void get_sites();
    void send_site_to_cpp(bool is_pass, QString site);
    bool get_pass(QString pass);
    bool get_new_account_bool(QString data);
    void get_new_account(QString data);
    void delete_account(QString site);
    void upd_model_qml();
    void get_backup_files();
    void get_name_file(QString namefile);

};







#endif // BACKEND_H
