#ifndef NEXUSCOORDINATORGUI_H
#define NEXUSCOORDINATORGUI_H

#include <QMainWindow>

namespace Ui {
class NexusCoordinatorGUI;
}

class NexusCoordinatorGUI : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit NexusCoordinatorGUI(QWidget *parent = 0);
    ~NexusCoordinatorGUI();
    
private:
    Ui::NexusCoordinatorGUI *ui;
};

#endif // NEXUSCOORDINATORGUI_H
