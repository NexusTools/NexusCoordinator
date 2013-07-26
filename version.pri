BRANCH_COUNT = $$system(git branch -a | wc -l)
message(BRANCH_COUNT)

greaterThan(BRANCH_COUNT, 1) {
	GIT_HEAD = $$system(git rev-parse HEAD)
	GIT_BRANCH = $$system(git branch | cut -c 3-)
	GIT_REVISION = $$system(git rev-list HEAD | wc -l)
	message("$$GIT_BRANCH $$GIT_HEAD")
	message("Revision $$GIT_REVISION")

	DEFINES += $$quote(GIT_BRANCH=\'\"$$GIT_BRANCH\"\')
	DEFINES += $$quote(GIT_REVISION=\'\"$$GIT_REVISION\"\')
	DEFINES += $$quote(GIT_HEAD=\'\"$$GIT_HEAD\"\')
	
	VER_PAT = $$GIT_REVISION
} else {
	message("Source not connected to git repo.")
	message("Debugging tools will not be compiled for this build...")
}

isEmpty(VER_MAJ):isEmpty(VER_MIN) {
	message(_PRO_FILE_PWD_)
} else {
	isEmpty(VER_MAJ): VER_MAJ = 0
	isEmpty(VER_MIN): VER_MIN = 0
}

DEFINES += $$quote(VER_MAJ=\'\"$$VER_MAJ\"\')
DEFINES += $$quote(VER_MIN=\'\"$$VER_MIN\"\')

