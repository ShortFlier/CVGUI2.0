#include "sceneproject.h"
#include "log.h"



SceneProject::SceneProject(QObject *parent)
{
    addProjects();
}

SceneProject::~SceneProject()
{
    clear();
}

OperatorScene *SceneProject::addProjects(const QString &name)
{
    QString pname;
    if(name.isNull()){
        pname=QString("工程%1").arg(_nums);
    }else{
        pname=name;
    }

    OperatorScene* scene=nullptr;

    if(_subProjects.contains(pname)){
        QString str("子工程[%1]已存在！");
        str=str.arg(pname);
        tip_warn(str);
    }else{
        scene=new OperatorScene;
        _subProjects.insert(pname, scene);
        log_info("新建子工程：{0}", pname.toStdString());
    }

    ++_nums;

    return scene;
}

void SceneProject::clear()
{
    for(auto itr=_subProjects.begin(); itr!=_subProjects.end(); ++itr){
        itr.value()->deleteLater();
    }
    _subProjects.clear();
}

SceneProject &SceneProject::instance()
{
    static SceneProject pjt;
    return pjt;
}

SceneProject *SceneProject::instancePtr()
{
    return &(instance());
}
