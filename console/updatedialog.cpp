#include "updatedialog.h"
#include "coordinatorconsole.h"

#include <QDir>
#include <QDateTime>

#include <unistd.h>

#include <curseslabel.h>
#include <cursesbuttonbox.h>
#include <curseslineedit.h>

CoordinatorUpdateDialog::CoordinatorUpdateDialog(CursesMainWindow *main) :
    CursesDialog("Update NexusCoordinator", main)
{
    setLayout(VerticalLayout);

    CursesVBox* vBox = new CursesVBox(this);
    new CursesLabel("This will download the source code for", vBox);
    new CursesLabel("NexusCoordinator to compile and install it.", vBox);

    CursesButtonBox* btnBox = new CursesButtonBox(this);
    CursesButton* btn = new CursesButton("U_pdate", Normal, btnBox);
    connect(btn, SIGNAL(selected(QVariant)), this, SLOT(answer(QVariant)));
    btn = new CursesButton("_Development Build", Normal, btnBox);
    connect(btn, SIGNAL(selected(QVariant)), this, SLOT(answer(QVariant)));
    btn = new CursesButton("Clos_e", Normal, btnBox);
    connect(btn, SIGNAL(selected(QVariant)), this, SLOT(answer(QVariant)));


}

void CoordinatorUpdateDialog::showImpl()  {
    CursesDialog::showImpl();
    QString repoUrl;

    QEventLoop eventLoop;
    connect(this, SIGNAL(answered()), &eventLoop, SLOT(quit()));

    clearValue();
    while(!hasValue())
        eventLoop.processEvents(QEventLoop::WaitForMoreEvents);

    QString val = value<QString>();
    if(val == "Development Build") {
        if(!CursesDialog::ensure("Development builds are often unstable, are you sure?", "Install Development Build"))
            return;

        CursesDialog::alert("Development builds are currently unavailable...", "Cannot Continue");
    } else if(val == "Update")
        repoUrl = "https://github.com/NexusTools/NexusCoordinator.git";
    else
        return;

    if(!repoUrl.isEmpty()) {
        CoordinatorConsole* console = (CoordinatorConsole*)CoordinatorConsole::current();
        if(!CursesDialog::continu("The source code will now be checked out and compiled. This may take a while.", "Update NexusCoordinator", &console->_config))
            return;

        int ret;
        QDir dir = QDir::temp();
        QString tempName = "NexusCoordinator-" + QDateTime::currentDateTime().toString(Qt::ISODate);
        QString tempPath = dir.path() + QDir::separator() + tempName + QDir::separator();

        if(QDir(tempPath).exists() || dir.mkdir(tempName)) {
            console->_upgraded = true;
            if(!console->startShell(QStringList() << "git" << "clone" << "--recursive" << repoUrl, "", "Download Update", "", tempPath)) {
                CursesDialog::alert("Failed to checkout source...", "Git Failed");
                goto cleanup;
            }
            tempPath += "NexusCoordinator";
            tempPath += QDir::separator();
            if(!QDir(tempPath).exists()) {
                CursesDialog::alert("Git returned okay but, source is missing...", "Source Missing");
                goto cleanup;
            }
            if(!console->startShell(QStringList() << "qmake" << "CONFIG+=RELEASE", "", "Configuring Update", "", tempPath)) {
                CursesDialog::alert("Failed to configure source...", "QMake Failed");
                goto cleanup;
            }
            if(!console->startShell(QStringList() << "make", "", "Building Update", "", tempPath)) {
                CursesDialog::alert("Failed to build source...", "Build Failed");
                goto cleanup;
            }
            if(!console->startShell(QStringList() << "sudo" << "make" << "install", "", "Installing Update", "", tempPath)) {
                CursesDialog::alert("Failed to install source...", "Install Failed");
                goto cleanup;
            }

            int status;
            console->titleChanged();
            int child_pid = fork();
            if (child_pid == 0) {
                execl("/usr/bin/nc-term", "nc-term", "--test-terminal", 0);
                _exit(-1);
            } else if(child_pid > 0) {
                int tLeft = 20; // Wait 2 seconds for test to complete before assuming it failed
                while(tLeft > 0 && waitpid(child_pid, &status, WNOHANG) == 0) {
                    usleep(100000); // Sleep 100ms
                    tLeft--;
                }

                kill(child_pid, SIGKILL);
                if(tLeft > 0 && WEXITSTATUS(status) == 0)
                    execl("/usr/bin/nc-term", "nc-term", "--shell", 0);

                CursesDialog::alert("The built update could not be run...", "Update Failed");
            }
        } else
            CursesDialog::alert("Cannot create directory to build in...", "Cannot Continue");

cleanup:
        console->_upgraded = false;
        console->titleChanged();
        return;
    }
}
