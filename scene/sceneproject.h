#ifndef SCENEPROJECT_H
#define SCENEPROJECT_H

#include "operatorscene.h"
#include <QObject>


/*
 * 一个工程可以包含许多的子工程，这些子工程可以访问工程的公共变量
 */

class SceneProject : public QObject
{
    Q_OBJECT
public:
    ~SceneProject();

    OperatorScene* addProjects(const QString& name=QString());
    QMap<QString, OperatorScene*>& subProjects();

    //清理所有的OperatorScene
    void clear();

    static SceneProject& instance();
    static SceneProject* instancePtr();

private:
    explicit SceneProject(QObject *parent = nullptr);

    QMap<QString, OperatorScene*> _subProjects;

    int _nums=0;
signals:
};

#endif // SCENEPROJECT_H
