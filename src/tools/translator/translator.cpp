#include "translator.h"
#include "translatorwindow.h"
#include "flameshotdaemon.h"
#include "src/utils/confighandler.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QApplication>
#include <QWidget>
#include <QMessageBox>

TranslatorTool::TranslatorTool(QObject* parent)
  : AbstractActionTool(parent)
{}

QIcon TranslatorTool::icon(const QColor& background, bool inEditor) const
{
    Q_UNUSED(inEditor)
    return QIcon(iconPath(background) + "translator.svg");
}

QString TranslatorTool::name() const
{
    return tr("Translator");
}

CaptureTool::Type TranslatorTool::type() const
{
    return CaptureTool::TYPE_TRANSLATOR;
}

QString TranslatorTool::description() const
{
    return tr("Google Translator");
}

CaptureTool* TranslatorTool::copy(QObject* parent)
{
    auto* tool = new TranslatorTool(parent);
    copyParams(this, tool);
    return tool;
}

bool TranslatorTool::closeOnButtonPressed() const
{
    return true;
}

void TranslatorTool::pressed(CaptureContext& context)
{
    auto capture = context.selectedScreenshotArea();
    QImage qimage = capture.toImage();
    QImage conv   = qimage.convertToFormat(QImage::Format_RGB888);
    tesseract::TessBaseAPI tess;
    
    if (tess.Init("/usr/share/tesseract-ocr/5/tessdata", "eng+rus") != 0) {
        QMessageBox::warning(nullptr,
                             tr("Translator"),
                             tr("Failed to initialize Tesseract OCR engine. "
                                "Make sure tesseract-ocr and language data "
                                "are installed."));
        emit requestAction(REQ_CLOSE_GUI);
        return;
    }

    tess.SetImage(conv.bits(),
                  conv.width(),
                  conv.height(),
                  3,
                  conv.bytesPerLine());

    char* outText = tess.GetUTF8Text();
    QString recognizedText;
    
    if (outText) {
        recognizedText = QString::fromUtf8(outText).trimmed();
        delete[] outText;
    }

    if (!recognizedText.isEmpty()) {
        // Визуально скрываем полупрозрачный фон и сетку Flameshot,
        // чтобы экран выглядел так, будто скриншотер закрылся.
        QVector<QWidget*> hiddenWidgets;
        const auto topLevelWidgets = QApplication::topLevelWidgets();
        for (QWidget* widget : topLevelWidgets) {
            if (widget->isVisible()) {
                widget->hide();
                hiddenWidgets.append(widget);
            }
        }

        // Создаем диалоговое окно на стеке
        TranslatorWindow window(recognizedText);

        // exec() блокирует дальнейшее выполнение кода на этой строке.
        // Окно будет спокойно висеть на экране и работать с сетью,
        // а Flameshot просто "подождет", не завершая процесс.
        window.exec();

        if (window.isRetry()) {
            if (!ConfigHandler().translatorRetryKeepSelection()) {
                // Очищаем выделение, чтобы пользователь мог
                // выделить новую область.
                emit requestAction(REQ_RESET_SELECTION);
            }
            // Показываем обратно все скрытые виджеты.
            for (QWidget* widget : hiddenWidgets) {
                widget->show();
            }
            return;
        }
    }

    // Как только вы закрыли окно переводчика, код идет дальше,
    // и Flameshot получает штатную команду завершить выделение.
    emit requestAction(REQ_CLOSE_GUI);
}
