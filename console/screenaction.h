#ifndef SCREENACTION_H
#define SCREENACTION_H

#include <cursesaction.h>

class ScreenAction : public CursesAction
{
public:
    explicit ScreenAction(int id, QString name, GUIContainer* par) : CursesAction(name, par) {_id=id;}
    inline QString name() const{return text();}

protected:
    bool processEvent(QEvent *e);

private:
    int _id;
};

#endif // SCREENACTION_H
