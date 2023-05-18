#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageAuthenticationCode>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->comboBox->addItems({"MD5", "SHA256", "SHA512"});

    connect(ui->buttonGroupType, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
        [=](QAbstractButton *button){
        if(button == ui->radioButton1){
            m_hash = true;
            ui->labelKey->setText(tr("结果"));
            ui->pushButtonFile->show();
            ui->labelFile->show();
            ui->lineEditFile->show();
        }
        else if(button == ui->radioButton2){
            m_hash = false;
            ui->labelKey->setText(tr("key"));
            ui->pushButtonFile->hide();
            ui->labelFile->hide();
            ui->lineEditFile->hide();
        }
    });
    connect(ui->buttonGroupResult, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
        [=](QAbstractButton *button){
        if(button == ui->radioButtonHex){
            m_hex = true;
        }
        else if(button == ui->radioButtonBase64){
            m_hex = false;
        }
    });

    connect(ui->comboBox, &QComboBox::currentTextChanged, [=](QString text){
        if(text == "MD5"){
            m_algorithm = QCryptographicHash::Md5;
        }
        else if(text == "SHA256"){
            m_algorithm = QCryptographicHash::Sha256;
        }
        else if(text == "SHA512"){
            m_algorithm = QCryptographicHash::Sha512;
        }
    });
    connect(ui->pushButtonFile, &QPushButton::clicked, [=](){
        QString path = QFileDialog::getOpenFileName(this, tr("选择文件"),
                                     "~", tr("所有文件(*.*)"));
        ui->lineEditFile->setText(path);
    });

    connect(ui->pushButtonCalc, &QPushButton::clicked, [=](){
        if(m_hash){
            if(ui->plainTextEdit->toPlainText() != ""){
                auto result = QCryptographicHash::hash(ui->plainTextEdit->toPlainText().toUtf8(),
                                                 m_algorithm);
                QString str;
                if(m_hex){
                    str = result.toHex();
                }
                else{
                    str = result.toBase64();
                }
                ui->lineEditResultText->setText(str);
            }
            if(ui->lineEditFile->text() != ""){
                QCryptographicHash hash(m_algorithm);
                QFile file(ui->lineEditFile->text());

                if(!file.open(QIODevice::ReadOnly))
                    return;
                while(!file.atEnd()){
                    // 每次读1MByte
                    hash.addData(file.read(1024*1024));
                }
                QString str;
                if(m_hex){
                    str = hash.result().toHex();
                }
                else{
                    str = hash.result().toBase64();
                }
                ui->lineEditResultFile->setText(str);
            }
        }
        else{
            if((ui->plainTextEdit->toPlainText() != "")
                    && (ui->lineEditResultText->text() != "")){
                QString info = ui->plainTextEdit->toPlainText();
                QString key = ui->lineEditResultText->text();
                auto result = QMessageAuthenticationCode::hash(info.toUtf8(),
                                                               key.toUtf8(),
                                                               m_algorithm);
                QString str;
                if(m_hex){
                    str = result.toHex();
                }
                else{
                    str = result.toBase64();
                }
                ui->lineEditResultFile->setText(str);
            }
        }
    });
}

Dialog::~Dialog()
{
    delete ui;
}

