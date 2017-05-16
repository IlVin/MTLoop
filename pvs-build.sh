#!/bin/bash

# Install PVS-Studio
# wget -q -O - http://files.viva64.com/etc/pubkey.txt | sudo apt-key add -
# sudo wget -O /etc/apt/sources.list.d/viva64.list http://files.viva64.com/etc/viva64.list
# sudo apt-get update
# sudo apt-get install pvs-studio

cd ./bin \
&& make clean \
&& cmake .. \
&& pvs-studio-analyzer trace -- make -j2 \
&& pvs-studio-analyzer analyze -l../PVS-Studio.lic -o./project.log -f ./strace_out -j2 \
&& plog-converter -a GA:1,2 -t tasklist -o ./project.tasks ./project.log\
&& cd .. \
&& vim ./bin/project.tasks
