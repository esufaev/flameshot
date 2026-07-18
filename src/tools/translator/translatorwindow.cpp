// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Your Name & Contributors

#include "translatorwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QApplication>
#include <QClipboard>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFont>

TranslatorWindow::TranslatorWindow(const QString& initialText, QWidget* parent)
    : QDialog(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_retry(false)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    setWindowTitle(tr("Flameshot Translator"));
    resize(850, 550);

    setupUi();
    loadLanguages();
    applyModernStyle();

    updateFonts(m_fontSizeSpinBox->value());

    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &TranslatorWindow::onTranslationFinished);

    if (!initialText.isEmpty()) {
        m_sourceTextEdit->setPlainText(initialText);
        onTranslateClicked();
    }
}

TranslatorWindow::~TranslatorWindow()
{
}

bool TranslatorWindow::isRetry() const
{
    return m_retry;
}

void TranslatorWindow::onRetryClicked()
{
    m_retry = true;
    accept();
}

void TranslatorWindow::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setSpacing(10);
    
    QLabel* fromLabel = new QLabel(tr("From:"), this);
    m_sourceLangCombo = new QComboBox(this);
    
    QLabel* toLabel = new QLabel(tr("To:"), this);
    m_targetLangCombo = new QComboBox(this);

    QLabel* sizeLabel = new QLabel(tr("Font:"), this);
    m_fontSizeSpinBox = new QSpinBox(this);
    m_fontSizeSpinBox->setRange(10, 48);
    m_fontSizeSpinBox->setValue(16);
    
    m_translateBtn = new QPushButton(tr("Translate"), this);

    m_retryBtn = new QPushButton(tr("Again"), this);
    m_retryBtn->setObjectName("retryBtn");

    topLayout->addWidget(fromLabel);
    topLayout->addWidget(m_sourceLangCombo);
    topLayout->addSpacing(10);
    topLayout->addWidget(toLabel);
    topLayout->addWidget(m_targetLangCombo);
    topLayout->addSpacing(10);
    topLayout->addWidget(sizeLabel);
    topLayout->addWidget(m_fontSizeSpinBox);
    topLayout->addStretch();
    topLayout->addWidget(m_retryBtn);
    topLayout->addWidget(m_translateBtn);

    mainLayout->addLayout(topLayout);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    
    QWidget* leftWidget = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    m_sourceTextEdit = new QTextEdit(this);
    m_sourceTextEdit->setPlaceholderText(tr("Original text..."));
    
    m_copySourceBtn = new QPushButton(tr("Copy Source"), this);
    m_copySourceBtn->setObjectName("secondaryBtn");
    
    QHBoxLayout* leftBottomLayout = new QHBoxLayout();
    leftBottomLayout->addStretch();
    leftBottomLayout->addWidget(m_copySourceBtn);

    leftLayout->addWidget(m_sourceTextEdit);
    leftLayout->addLayout(leftBottomLayout);

    QWidget* rightWidget = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    
    m_targetTextEdit = new QTextEdit(this);
    m_targetTextEdit->setPlaceholderText(tr("Translation will appear here..."));
    
    m_copyTargetBtn = new QPushButton(tr("Copy Translation"), this);
    m_copyTargetBtn->setObjectName("secondaryBtn");

    QHBoxLayout* rightBottomLayout = new QHBoxLayout();
    rightBottomLayout->addStretch();
    rightBottomLayout->addWidget(m_copyTargetBtn);

    rightLayout->addWidget(m_targetTextEdit);
    rightLayout->addLayout(rightBottomLayout);

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    mainLayout->addWidget(splitter);

    connect(m_translateBtn, &QPushButton::clicked, this, &TranslatorWindow::onTranslateClicked);
    connect(m_copySourceBtn, &QPushButton::clicked, this, &TranslatorWindow::onCopySourceClicked);
    connect(m_copyTargetBtn, &QPushButton::clicked, this, &TranslatorWindow::onCopyTargetClicked);
    connect(m_retryBtn, &QPushButton::clicked, this, &TranslatorWindow::onRetryClicked);
    connect(m_fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &TranslatorWindow::onFontSizeChanged);
}

void TranslatorWindow::updateFonts(int size)
{
    QFont font = m_sourceTextEdit->font();
    font.setPointSize(size);
    m_sourceTextEdit->setFont(font);
    m_targetTextEdit->setFont(font);
}

void TranslatorWindow::onFontSizeChanged(int size)
{
    updateFonts(size);
}

void TranslatorWindow::loadLanguages()
{
    m_sourceLangCombo->addItem(tr("Auto Detect"), "auto");
    m_sourceLangCombo->addItem(tr("English"), "en");
    m_sourceLangCombo->addItem(tr("Russian"), "ru");
    m_sourceLangCombo->addItem(tr("Spanish"), "es");
    m_sourceLangCombo->addItem(tr("German"), "de");
    m_sourceLangCombo->addItem(tr("French"), "fr");

    m_targetLangCombo->addItem(tr("Russian"), "ru");
    m_targetLangCombo->addItem(tr("English"), "en");
    m_targetLangCombo->addItem(tr("Spanish"), "es");
    m_targetLangCombo->addItem(tr("German"), "de");
    m_targetLangCombo->addItem(tr("French"), "fr");
}

void TranslatorWindow::applyModernStyle()
{
    QString style = R"(
        QDialog {
            background-color: #f7f7f7;
            color: #333333;
            font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
        }
        QTextEdit {
            background-color: #ffffff;
            border: 1px solid #d1d1d1;
            border-radius: 8px;
            padding: 10px;
        }
        QTextEdit:focus {
            border: 1px solid #0078d4;
            background-color: #ffffff;
        }
        QPushButton {
            background-color: #0078d4;
            color: #ffffff;
            border: none;
            border-radius: 6px;
            padding: 8px 18px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #106ebe;
        }
        QPushButton:pressed {
            background-color: #005a9e;
        }
        QPushButton:disabled {
            background-color: #a0a0a0;
        }
        QPushButton#secondaryBtn {
            background-color: #e1dfdd;
            color: #333333;
        }
        QPushButton#secondaryBtn:hover {
            background-color: #d2d0ce;
        }
        QPushButton#secondaryBtn:pressed {
            background-color: #c8c6c4;
        }
        QComboBox, QSpinBox {
            border: 1px solid #d1d1d1;
            border-radius: 6px;
            padding: 6px 12px;
            background-color: #ffffff;
            font-size: 14px;
        }
        QComboBox { min-width: 120px; }
        QSpinBox { min-width: 50px; }
        QComboBox::drop-down, QSpinBox::up-button, QSpinBox::down-button {
            border: none;
        }
        QLabel {
            font-weight: 600;
            color: #555555;
            font-size: 14px;
        }
        QSplitter::handle {
            background: #e1dfdd;
            width: 2px;
        }
        QPushButton#retryBtn {
            background-color: #107c10;
            color: #ffffff;
        }
        QPushButton#retryBtn:hover {
            background-color: #0e6e0e;
        }
        QPushButton#retryBtn:pressed {
            background-color: #0b5c0b;
        }
    )";
    this->setStyleSheet(style);
}

void TranslatorWindow::onTranslateClicked()
{
    QString text = m_sourceTextEdit->toPlainText().trimmed();
    if (text.isEmpty()) return;

    m_translateBtn->setEnabled(false);
    m_translateBtn->setText(tr("Translating..."));

    QUrl url("https://translate.googleapis.com/translate_a/single");
    QUrlQuery query;
    query.addQueryItem("client", "gtx");
    query.addQueryItem("sl", m_sourceLangCombo->currentData().toString());
    query.addQueryItem("tl", m_targetLangCombo->currentData().toString());
    query.addQueryItem("dt", "t");
    query.addQueryItem("q", text);
    url.setQuery(query);

    QNetworkRequest request(url);
    m_networkManager->get(request);
}

void TranslatorWindow::onTranslationFinished(QNetworkReply* reply)
{
    m_translateBtn->setEnabled(true);
    m_translateBtn->setText(tr("Translate"));

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QString translatedText;

        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            if (!arr.isEmpty() && arr[0].isArray()) {
                QJsonArray sentences = arr[0].toArray();
                for (int i = 0; i < sentences.size(); ++i) {
                    if (sentences[i].isArray()) {
                        translatedText += sentences[i].toArray()[0].toString();
                    }
                }
            }
        }
        m_targetTextEdit->setPlainText(translatedText);
    } else {
        m_targetTextEdit->setPlainText(tr("Translation Error: ") + reply->errorString());
    }
    reply->deleteLater();
}

void TranslatorWindow::onCopySourceClicked()
{
    QString text = m_sourceTextEdit->toPlainText();
    if (!text.isEmpty()) QApplication::clipboard()->setText(text);
}

void TranslatorWindow::onCopyTargetClicked()
{
    QString text = m_targetTextEdit->toPlainText();
    if (!text.isEmpty()) QApplication::clipboard()->setText(text);
}
