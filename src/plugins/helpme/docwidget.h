#ifndef DOCWIDGET_H
#define DOCWIDGET_H

#include <QWidget>

namespace Helpme {
namespace Internal {

class DocWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DocWidget(QWidget *parent = nullptr);

signals:

};
}}
#endif // DOCWIDGET_H
