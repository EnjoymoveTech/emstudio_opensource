#ifndef ADVANCEMETHOD_H
#define ADVANCEMETHOD_H

#include <QDialog>
#include <emostools/api_description_parser.h>
#include <utils/emoswidgets/flattable.h>

namespace DescriptionEditor {

class AdvanceMethod : public QDialog
{
    Q_OBJECT
public:
    explicit AdvanceMethod(QDialog *parent = nullptr);

    void setTypes(const QStringList& types);
    void setParams(const QList<EmosTools::tParam>& params);
    void showDialog();
    QList<EmosTools::tParam> getParams() const;
signals:
public slots:
    void slotAddRow();
    void slotRemoveRow();

private slots:
    void slotTypeCombo(QString);
    void slotDirCombo(QString);
protected:
    virtual void closeEvent(QCloseEvent *event) override;
    QSize minimumSizeHint() const override;

    QList<EmosTools::tParam> m_params;
    QStringList m_types;
    QStringList m_typesPointer;

    void appendComboBox(const QStringList &items, int row, int col, QString currentText, bool editorEnable);
    QString direction2String(EmosTools::tParamDirection direction);
    EmosTools::tParamDirection string2Direction(const QString& str);
    EmosWidgets::FlatTable* m_table;
};
}
#endif // ADVANCEMETHOD_H
