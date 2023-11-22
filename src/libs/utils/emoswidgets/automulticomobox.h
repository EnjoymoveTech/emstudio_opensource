#ifndef AUTOMULTICOMOBOX_H
#define AUTOMULTICOMOBOX_H

#include <utils/utils_global.h>

#include <QComboBox>

namespace EmosWidgets
{

class AutoMultiComoBoxPrivate;
class EMSTUDIO_UTILS_EXPORT AutoMultiComoBox : public QComboBox
{
    Q_OBJECT
public:
    AutoMultiComoBox(QWidget* parent = nullptr);
    ~AutoMultiComoBox();

    void setCompleteList(const QStringList& list);
    QStringList getCompleteList() const;

    void setSeparator(const QString& c);
protected:
    void keyPressEvent(QKeyEvent *e) override;
    virtual void showPopup() override;
    virtual void hidePopup() override;

private:
    QScopedPointer<AutoMultiComoBoxPrivate> d;

};

}

#endif // FLATTABLE_H
