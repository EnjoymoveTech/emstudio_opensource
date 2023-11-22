#ifndef PROJECTMERGE_H
#define PROJECTMERGE_H

#include <QObject>
#include <QFileDialog>
#include "mergewizards/mergewizard.h"

class ProjectMerge : public QObject
{
    Q_OBJECT
public:
    explicit ProjectMerge(QObject *parent = nullptr);
    //void setMergeProjectPath(QString path);
    bool merge(QString projectPath);
private:
    QString getMergedProjectPath();

    bool typeMerge();
    bool apiMerge();
    bool idlMerge();
    bool cfvMerge();
signals:

public slots:
    void slotMerge(const QString& projectPath);
private:
    //QString m_projectPath;
    //QString m_mergedProjectPath;

    MergeWizard* m_wizard;
};

#endif // PROJECTMERGE_H
