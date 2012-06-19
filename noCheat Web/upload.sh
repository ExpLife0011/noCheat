#!/bin/bash

# SSH command
echo -e "Uploading/installing noCheatW to Natoga"
tar cvfm _upload_.tar nc_remote_install.sh Back/ Front/ >NUL
cat _upload_.tar | gzip | ssh -p 2222 natogac1@natoga.com 'cat | gzip -d > ./nctemp/_upload_.tar && cd ./nctemp && tar xvf _upload_.tar > NUL && rm _upload_.tar && chmod 0777 nc_remote_install.sh && mv nc_remote_install.sh ../ && cd .. && ./nc_remote_install.sh && echo Done!'

# Remove tar archine
rm _upload_.tar
