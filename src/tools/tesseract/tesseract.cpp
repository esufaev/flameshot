// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "tesseract.h"
#include "flameshotdaemon.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

TesseractTool::TesseractTool(QObject* parent)
  : AbstractPathTool(parent)
{}

QIcon TesseractTool::icon(const QColor& background, bool inEditor) const
{
    Q_UNUSED(inEditor)
    return QIcon(iconPath(background) + "tesseract.svg");
}
QString TesseractTool::name() const
{
    return tr("Tesseract");
}

CaptureTool::Type TesseractTool::type() const
{
    return CaptureTool::TYPE_TESSERACT;
}

QString TesseractTool::description() const
{
    return tr("TESSERACT");
}

CaptureTool* TesseractTool::copy(QObject* parent)
{
    auto* tool = new TesseractTool(parent);
    copyParams(this, tool);
    return tool;
}

void TesseractTool::process(QPainter& painter, const QPixmap& pixmap)
{
}

void TesseractTool::paintMousePreview(QPainter& painter,
                                   const CaptureContext& context)
{
    
}

void TesseractTool::drawStart(const CaptureContext& context)
{

}

void TesseractTool::pressed(CaptureContext& context)
{
    auto capture = context.selectedScreenshotArea();
    QImage qimage = capture.toImage();
    QImage conv   = qimage.convertToFormat(QImage::Format_RGB888);

    tesseract::TessBaseAPI tess;

    tess.Init("/usr/share/tesseract-ocr/5/tessdata", "eng+rus");
    tess.SetImage(conv.bits(),
                    conv.width(),
                    conv.height(),
                    3,
                    conv.bytesPerLine());

    char* outText = tess.GetUTF8Text();
    if (outText) {
        FlameshotDaemon::copyToClipboard(QString::fromUtf8(outText));
        delete[] outText;
    }

    emit requestAction(REQ_CLOSE_GUI);
}
