#include "docwidget.h"

#include <QLabel>
#include <QVBoxLayout>

namespace Helpme {
namespace Internal {

DocWidget::DocWidget(QWidget *parent)
    : QWidget{parent}
{
    const QString placeholderText = tr("<html><body style=\"color:#606060; font-size:16px\">"
          "<div align='center'>"
          "<div style=\"font-size:22px\">EmStudio</div>"
          "<table><tr><td>"
          "<hr/>"
          "<div style=\"margin-top: 5px\">&bull; A Tool For Emos</div>"
          "<div style=\"margin-top: 5px\">&bull; <a href='http://x.x.x.x:xxxx/docsify'>Document</a> </div>"
          "</table>"
          "</div>"
          "</body></html>");
    QLabel* label = new QLabel(placeholderText, this);
    label->setOpenExternalLinks(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(label);
}
}}
