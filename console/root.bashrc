. /etc/bash.bashrc

PROMPT_COMMAND='echo -ne "\033]0;Root Shell on ${USER}@$(hostname)\007"'
PS1="\[\033[1;31m\]\u\[\033[1;34m\]@\H:\w#\[\033[0m\] "
