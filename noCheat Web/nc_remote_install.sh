#!/bin/bash

# Create temp folder
echo -e " - Creating temp folder"
mkdir ./domains/tk.nochea.install

# Copy back contents to temp folder
echo -e " - Copying back-end contents"
cp -r ./nctemp/Back/* ./domains/tk.nochea.install/

# Delete n.php
echo -e " - Removing documentation nn.php"
rm ./domains/tk.nochea.install/n.php

# Install registry (nn.php -> n.php)
echo -e " - Install n.php"
mv ./domains/tk.nochea.install/src/nn.php ./domains/tk.nochea.install/src/n.php

# Copy front contents
echo -e " - Copy front-end contents"
cp -r ./nctemp/Front/* ./domains/tk.nochea.install/

# Move all .NCT files to the template folder
echo -e " - Move template files to NCT folder"
mkdir ./domains/tk.nochea.install/nct
find ./domains/tk.nochea.install -type f -name "*.nct" -print0 | xargs -0 -I {} mv {} ./domains/tk.nochea.install/nct

# Install directory
echo -e " - Staging new installation"
mv ./domains/tk.nochea ./domains/tk.nochea.old
mv ./domains/tk.nochea.install ./domains/tk.nochea

# Delete upload directories
echo -e " - Deleting upload directories"
rm -r ./nctemp/*

# Delete install script
echo -e " - Deleting install script"
rm nc_remote_install.sh
