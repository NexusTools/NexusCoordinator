unix {
    BRANCH_COUNT = $$system(git branch -a | wc -l)

    greaterThan(BRANCH_COUNT, 1) {
            GIT_HEAD = $$system(git rev-parse HEAD)
            GIT_BRANCH = $$system(git branch | cut -c 3-)
            GIT_REVISION = $$system(git rev-list HEAD | wc -l)
			GIT_AUTHORS = $$system("git log --format='%aN<%aE>' | sort -u | sed ':a;N;$!ba;s/\n/,/g' -")
			message("$$GIT_BRANCH $$GIT_HEAD")
			message("Revision $$GIT_REVISION")
			!isEmpty(GIT_AUTHORS) {
				message("Authors $$GIT_AUTHORS")
				DEFINES += $$quote(GIT_AUTHORS=\'\"$$GIT_AUTHORS\"\')
			}

            DEFINES += $$quote(GIT_BRANCH=\'\"$$GIT_BRANCH\"\')
            DEFINES += $$quote(GIT_REVISION=\'\"$$GIT_REVISION\"\')
            DEFINES += $$quote(GIT_HEAD=\'\"$$GIT_HEAD\"\')

            VER_PAT = $$GIT_REVISION
    } else {
            message("Source not connected to git repo.")
            message("Debugging tools will not be compiled for this build...")
    }
} else: warning("Windows git detection is not implemented yet.")

isEmpty(VER_MAJ): VER_MAJ = 0
isEmpty(VER_MIN): VER_MIN = 1

DEFINES += $$quote(VER_MAJ=\'\"$$VER_MAJ\"\')
DEFINES += $$quote(VER_MIN=\'\"$$VER_MIN\"\')
