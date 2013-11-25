. /etc/bash.bashrc

PROMPT_COMMAND='echo -ne "\033]0;Root Shell on ${USER}@${HOSTSTR}\007"'
PS1="\[\033[1;34m\]\u@\h:${HOSTSTR}#\[\033[0m\] "
