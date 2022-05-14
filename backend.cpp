#include "backend.h"
#include <QFile>
#include <QDebug>
#include <conio.h>
#include <QDir>
#include <stdio.h>
#include <QDateTime>
#include <cstdlib>
#include "Windows.h"
#include "winuser.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <complex>
#include <string>
#include <cstring>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/sha.h>

using namespace std;

Backend::Backend(QObject *parent)
    : QObject{parent}
{
}

void Backend::get_sites()
{
    emit send_sites_to_qml(readJSON());
}

void Backend::send_site_to_cpp(bool is_pass, QString site)
{
    get_data(is_pass, site);
}

void Backend::get_backup_files()
{
    QString path = "C://Users//Alexander//Desktop//stud//Lab_2//history";
    QDir dir(path);
    QStringList filter;
    filter << "*.json";
    QStringList files = dir.entryList(filter, QDir::Files|QDir::Readable, QDir::Name | QDir::Reversed);

    emit send_backup_to_qml(files);
}


void Backend::upd_model_qml()
{
    get_sites();
}

void Backend::delete_account(QString site)
{
    json.remove(site);

    QJsonArray jsonArray = json["urls"].toArray();
    for (int i=0; i<jsonArray.size(); i++)
        if (jsonArray[i] == site)
            jsonArray.removeAt(i);

    json.insert("urls", jsonArray);
    backup();
}


void Backend::get_name_file(QString namefile)
{
    sourcefile = "C://Users//Alexander//Desktop//stud//Lab_2//history//" + namefile;
}

bool Backend::get_pass(QString pass)
{
    SHA256(reinterpret_cast<unsigned char *>(pass.toLatin1().data()), pass.length(), md);
    pass.clear();
    decrypt_file(md);
    data_to_json();

    if (all_data.substr(0,1) == "{")
        return true;
    else
        return false;
}

void Backend::backup()
{
    all_data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();
    encrypt_file(md);
}


void Backend::get_data(bool is_pass, QString site)
{
    QJsonObject object_site = json[site].toObject();
    QString string_;

    if (is_pass==0)
        string_ = object_site["login"].toString();

    else if (is_pass==1)
        string_ = object_site["password"].toString();

    unsigned char *cipher_login = qstr_to_unchar(string_),
            source_login[128];

    crypt_data(cipher_login, source_login, 0, md);

    string string__(reinterpret_cast<char*>(source_login));

    toClipboard(string__);
    string__.clear();
}


QString Backend::unchar_to_qstr(char* unchar)
{
    string str = string(unchar);
    auto arr = QByteArray::fromStdString(str);
    auto qstr = QString::fromLatin1(arr);

    return qstr;
}

unsigned char* Backend::qstr_to_unchar(QString qstr)
{
    auto arr = qstr.toLatin1();
    auto str = arr.toStdString();

    unsigned char* unchar;
    strcpy((char*)unchar, str.c_str());

    return unchar;
}


bool Backend::get_new_account_bool(QString data)
{
    QStringList arr_data = data.split(" ");

    if (arr_data.count() == 3)
        return 1;
    else
        return 0;
}

void Backend::get_new_account(QString data)
{
    QStringList arr_data = data.split(" ");
    QString site = arr_data[0];
    QString login = arr_data[1];
    QString pass = arr_data[2];

    unsigned char *source_login = reinterpret_cast<unsigned char *>(login.toLatin1().data()),
            cipher_login[128];
    crypt_data(source_login, cipher_login, 1, md);

    unsigned char *source_pass = reinterpret_cast<unsigned char *>(pass.toLatin1().data()),
            cipher_pass[128];
    crypt_data(source_pass, cipher_pass, 1, md);

    QJsonObject json_site = json[site].toObject();

    json_site.insert("login", unchar_to_qstr((char*)cipher_login));
    json_site.insert("password", unchar_to_qstr((char*)cipher_pass));

    json.insert(site, json_site);

    QJsonArray jsonArray = json["urls"].toArray();
    jsonArray.append(site);
    json.insert("urls", jsonArray);
    backup();

    get_sites();
}

void Backend::data_to_json()
{
    QString q_all_data = QString::fromStdString(all_data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(q_all_data.toUtf8(), &error);
    qDebug() << "Error: " << error.errorString() << error.offset << error.error;

    if (doc.isObject())
    {
        json = doc.object();
    }
}

QStringList Backend::readJSON()
{
    QStringList sites_array;
    QJsonArray jsonArray = json["urls"].toArray();

    foreach (const QJsonValue & value, jsonArray)
        sites_array << value.toString();

    return sites_array;
}

void Backend::toClipboard(const std::string &s)
{
    OpenClipboard(0);
    EmptyClipboard();
    HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE, s.size()+1);
    if (!hg){
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg),s.c_str(), s.size()+1);
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT,hg);
    CloseClipboard();
    GlobalFree(hg);
}



bool Backend::crypt_data(unsigned char *sourcetext, unsigned char *ciphertext, int do_encrypt, unsigned char* key_user)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    int sourcetext_len = strlen((char *)sourcetext),
            len,
            ciphertext_len;

    unsigned char * iv = (unsigned char *)("%QkI0j2XwDjpf$644yfm0xY0wBsWkM1h"),
            *key = (unsigned char *) (char*) key_user;

    if(!EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL, NULL, do_encrypt))
    {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if(!EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, do_encrypt))
    {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if(!EVP_CipherUpdate(ctx, ciphertext, &len, sourcetext, sourcetext_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    ciphertext_len = len;

    if(!EVP_CipherFinal_ex(ctx, ciphertext + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext[ciphertext_len] = '\0';

    return ciphertext_len;
}

bool Backend::encrypt_file(unsigned char* key_user)
{
    QString file_name = "C://Users//Alexander//Desktop//stud//Lab_2//Data.json";
    QFile file_modified(file_name);
    file_modified.open(QIODevice::ReadWrite | QIODevice::Truncate);

    int position;
    position = file_name.lastIndexOf("Data.json");
    QString file_backup_name = file_name.left(position) + "history//Data.json";
    position = file_backup_name.lastIndexOf(".");
    QString file_extension = file_backup_name.mid(position);
    file_backup_name = file_backup_name.left(position) + QDateTime::currentDateTime().toString("_dd.MM.yyyy_hh.mm.ss") + file_extension;
    QFile file_backup(file_backup_name);
    file_backup.open(QIODevice::ReadWrite | QIODevice::Truncate);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    unsigned char * iv = (unsigned char *)("%QkI0j2XwDjpf$644yfm0xY0wBsWkM1h"),
            *key = (unsigned char *) (char*) key_user;

    if(!EVP_EncryptInit_ex(ctx, EVP_aes_256_ofb(), NULL, key, iv))
        return false;

    unsigned char ciphertext[256];
    unsigned char plaintext[256];

    int sourcetext_len = strlen((char *)all_data.substr(0, 256).c_str()),
            len,
            ciphertext_len;

    while (sourcetext_len > 0)
    {
        strcpy((char*)plaintext, (char*)all_data.substr(0, 256).c_str());

        if(!EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, sourcetext_len))
            return false;

        all_data.erase(0, 256);
        sourcetext_len = strlen((char *)all_data.substr(0, 256).c_str());

        file_modified.write((char*)ciphertext, len);
        file_backup.write((char*)ciphertext, len);
    }

    ciphertext_len = len;

    if(!EVP_CipherFinal_ex(ctx, ciphertext + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext[ciphertext_len] = '\0';

    file_modified.write((char*)ciphertext, len);
    file_backup.write((char*)ciphertext, len);

    file_modified.close();
    file_backup.close();
}

bool Backend::decrypt_file(unsigned char* key_user){
    all_data = "";

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    unsigned char * iv = (unsigned char *)("%QkI0j2XwDjpf$644yfm0xY0wBsWkM1h"),
            *key = (unsigned char *) (char*) key_user;

    if(!EVP_DecryptInit_ex(ctx, EVP_aes_256_ofb(), NULL, key, iv))
        return false;

    QFile source_file(sourcefile);
    source_file.open(QIODevice::ReadOnly);

    unsigned char ciphertext[256];
    unsigned char plaintext[256];
    int plaintext_len = source_file.read((char *)plaintext, 256);
    int len;

    while (plaintext_len > 0)
    {
        if(!EVP_DecryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
            return false;
        string all_data_(ciphertext, ciphertext+len);
        all_data += all_data_;

        plaintext_len = source_file.read((char *)plaintext, 256);
    }

    if(!EVP_DecryptFinal_ex(ctx, ciphertext + len, &len))
        return false;

    string all_data_(ciphertext, ciphertext+len);
    all_data += all_data_;
    EVP_CIPHER_CTX_free(ctx);

    source_file.close();
}

bool Backend::SHA256(unsigned char* input, unsigned long length, unsigned char* md)
{
    SHA256_CTX context;
    if(!SHA256_Init(&context))
        return false;

    if(!SHA256_Update(&context, (unsigned char*)input, length))
        return false;

    if(!SHA256_Final(md, &context))
        return false;

    return true;
}









//bool Backend::encrypt_file(unsigned char* key_user){

//    all_data = "";
//    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

//    unsigned char * iv = (unsigned char *)("%QkI0j2XwDjpf$644yfm0xY0wBsWkM1h"),
//            *key = (unsigned char *) (char*) key_user;

//    if(!EVP_EncryptInit_ex(ctx, EVP_aes_256_ofb(), NULL, key, iv))
//        return false;

//    QString sourcefile = "C://Users//Alexander//Desktop//stud//Lab_2//Data_2.json";
//    QFile source_file(sourcefile);
//    source_file.open(QIODevice::ReadOnly);

//    QFile file_modified("C://Users//Alexander//Desktop//stud//Lab_2//Data.json");
//    file_modified.open(QIODevice::ReadWrite | QIODevice::Truncate);

//    unsigned char ciphertext[256];
//    unsigned char plaintext[256];
//    int plaintext_len = source_file.read((char *)plaintext, 256);
//    int len;

//    while (plaintext_len > 0)
//    {

//        if(!EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
//            return false;

//        string all_data_(ciphertext, ciphertext+len);
//        all_data += all_data_;


//        file_modified.write((char*)ciphertext, len);
//        plaintext_len = source_file.read((char *)plaintext, 256);
//    }

//    if(!EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
//        return false;


//    file_modified.write((char*)ciphertext, len);
//    string all_data_(ciphertext, ciphertext+len);
//    all_data += all_data_;
//    EVP_CIPHER_CTX_free(ctx);


//    source_file.close();
//    file_modified.close();

//}


//bool Backend::crypt_file(QString key_user, bool mode){
//    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
//    unsigned char md[SHA256_DIGEST_LENGTH]; // 32 bytes
//    SHA256(reinterpret_cast<unsigned char *>(key_user.toLatin1().data()), key_user.length(), md);


//    unsigned char * iv = (unsigned char *)("%QkI0j2XwDjpf$644yfm0xY0wBsWkM1h"),
//            *key = (unsigned char *) (char*) md;

//    if (mode == 1)
//    {
//        if(!EVP_EncryptInit_ex(ctx, EVP_aes_256_ofb(), NULL, key, iv))
//            return false;
//    }

//    else if (mode == 0)
//        if(!EVP_DecryptInit_ex(ctx, EVP_aes_256_ofb(), NULL, key, iv))
//            return false;

//    QString sourcefile = "C://Users//Alexander//Desktop//stud//Lab_2//Data_encrypt.json";
//    QFile source_file(sourcefile);
//    source_file.open(QIODevice::ReadOnly);

//    int position;
//    position = sourcefile.lastIndexOf(".");
//    QString file_extension = sourcefile.mid(position);
//    QString sourcefile_for_if;

//    if (mode == 1)
//    {
//        position = sourcefile.lastIndexOf(".");
//        sourcefile_for_if = sourcefile.left(position) + "_encrypt" + file_extension;
//    }

//    else if (mode == 0)
//    {
//        position = sourcefile.lastIndexOf("_encrypt");
//        sourcefile_for_if = sourcefile.left(position) + "_decrypt" + file_extension;
//    }

//    QFile file_modified(sourcefile_for_if);
//    file_modified.open(QIODevice::ReadWrite | QIODevice::Truncate);

//    unsigned char ciphertext[256];
//    unsigned char plaintext[256];
//    int plaintext_len = source_file.read((char *)plaintext, 256);
//    int len;

//    while (plaintext_len > 0)
//    {
//        if (mode == 1)
//        {
//            if(!EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
//                return false;
//        }

//        else if (mode == 0)
//            if(!EVP_DecryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
//                return false;
//        string all_data_(ciphertext, ciphertext+len);
//        all_data += all_data_;


//        file_modified.write((char*)ciphertext, len);
//        plaintext_len = source_file.read((char *)plaintext, 256);
//    }


//    if (mode == 1)
//    {
//        if(!EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
//            return false;
//    }

//    else if (mode == 0)
//        if(!EVP_DecryptFinal_ex(ctx, ciphertext + len, &len))
//            return false;

//    file_modified.write((char*)ciphertext, len);
//    string all_data_(ciphertext, ciphertext+len);
//    all_data += all_data_;
//    EVP_CIPHER_CTX_free(ctx);


//    source_file.close();
//    file_modified.close();
//}

