#ifndef AUTOCOMOBOX_H
#define AUTOCOMOBOX_H

#include <utils/utils_global.h>

#include <QComboBox>

namespace EmosWidgets
{

class AutoComoBoxPrivate;
class EMSTUDIO_UTILS_EXPORT AutoComoBox : public QComboBox
{
    Q_OBJECT
public:
    AutoComoBox(QWidget* parent = nullptr);
    ~AutoComoBox();

    void setCompleteList(const QStringList& list);
    QStringList getCompleteList() const;
private:
    QScopedPointer<AutoComoBoxPrivate> d;
};

}

#endif // FLATTABLE_H
