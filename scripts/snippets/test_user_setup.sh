#!/bin/bash

: '
Create a test user and give that teest user permissions to read the
default home directory and possibly others as needed

'

# the below currnetly doesn't work, ignore.
: ' TO RUN THIS SCRIPT:
sudo bash /Users/arielwhittingham/prod_workspace/fdb_2026/scripts/snippets/test_user_setup.sh
'
: '
export TEST_USER="crawler"


# Create the user record in the local directory node
sudo dscl . -create /Users/$TEST_USER

# Assign the default Zsh shell to the user
sudo dscl . -create /Users/$TEST_USER UserShell /bin/bash

# Assign a unique User ID (479 is typically safe and unused)
sudo dscl . -create /Users/$TEST_USER UniqueID 479

# Assign the user to the default staff group (GID 20)
sudo dscl . -create /Users/$TEST_USER PrimaryGroupID 20

# Set the path for the users home directory
sudo dscl . -create /Users/$TEST_USER NFSHomeDirectory /Users/$TEST_USER

# Physically create the home directory folder
sudo mkdir -p /Users/$TEST_USER

# Grant the new user ownership of their home folder
sudo chown -R $TEST_USER:staff /Users/$TEST_USER

sudo dscl . -passwd /Users/crawler "pwcrawler"

sudo chmod +a "$TEST_USER allow execute" /Users/arielwhittingham
sudo chmod +a "$TEST_USER allow execute" /Users/arielwhittingham/prod_workspace/fdb_2026/src/crawl

find "/Users/arielwhittingham" -exec sudo chmod +a "$TEST_USER allow read,execute,list,readattr,file_inherit,directory_inherit" {} +

'