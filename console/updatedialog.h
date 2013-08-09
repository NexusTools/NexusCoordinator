#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <cursesdialog.h>

class CoordinatorUpdateDialog : public CursesDialog
{
    Q_OBJECT
public:
    explicit CoordinatorUpdateDialog(CursesMainWindow *main = 0);

protected:
    virtual void showImpl();
    
};

#endif // UPDATEDIALOG_H
