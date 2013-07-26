#include "nexuscoordinatorgui.h"
#include "ui_nexuscoordinatorgui.h"

NexusCoordinatorGUI::NexusCoordinatorGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NexusCoordinatorGUI)
{
    ui->setupUi(this);
}

NexusCoordinatorGUI::~NexusCoordinatorGUI()
{
    delete ui;
}
