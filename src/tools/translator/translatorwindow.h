// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 Your Name & Contributors

#pragma once

#include <QDialog>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class TranslatorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TranslatorWindow(const QString& initialText, QWidget* parent = nullptr);
    ~TranslatorWindow();
    bool isRetry() const;

private slots:
    void onTranslateClicked();
    void onTranslationFinished(QNetworkReply* reply);
    void onCopySourceClicked();
    void onCopyTargetClicked();
    void onFontSizeChanged(int size);
    void onRetryClicked();

private:
    void setupUi();
    void loadLanguages();
    void applyModernStyle();
    void updateFonts(int size);

    QTextEdit* m_sourceTextEdit;
    QTextEdit* m_targetTextEdit;
    QComboBox* m_sourceLangCombo;
    QComboBox* m_targetLangCombo;
    QSpinBox*  m_fontSizeSpinBox;
    QPushButton* m_translateBtn;
    QPushButton* m_copySourceBtn;
    QPushButton* m_copyTargetBtn;
    QPushButton* m_retryBtn;

    QNetworkAccessManager* m_networkManager;
    bool m_retry;
};
