#!/bin/bash

clone_comand="git clone -b serverside https://github.com/ShitCodeTech/DockerMonitoringToolkit.git"
repo_dir="DockerMonitoringToolkit"
build_command="`docker build --build-arg ID="$ID" --build-arg PRETTY_NAME="$PRETTY_NAME" -t server_monitor .`"
start_command="docker run -p 8080:8080 -d server_monitor"

banner=$tput setaf 3;(cat << 'EOF'
  █████████ █████     ███ █████     █████████             █████       ███████████           █████     
 ███░░░░░██░░███     ░░░ ░░███     ███░░░░░███           ░░███       ░█░░░███░░░█          ░░███      
░███    ░░░ ░███████ ███████████  ███     ░░░  ██████  ███████  █████░   ░███  ██████ ██████░███████  
░░█████████ ░███░░██░░██░░░███░  ░███         ███░░██████░░███ ███░░███  ░███ ███░░█████░░██░███░░███ 
 ░░░░░░░░███░███ ░███░███ ░███   ░███        ░███ ░██░███ ░███░███████   ░███░██████░███ ░░░░███ ░███ 
 ███    ░███░███ ░███░███ ░███ ██░░███     ██░███ ░██░███ ░███░███░░░    ░███░███░░░░███  ██░███ ░███ 
░░█████████ ████ █████████░░█████ ░░█████████░░██████░░███████░░██████   ████░░█████░░██████████ █████
 ░░░░░░░░░ ░░░░ ░░░░░░░░░  ░░░░░   ░░░░░░░░░  ░░░░░░  ░░░░░░░░ ░░░░░░   ░░░░░ ░░░░░░ ░░░░░░░░░░ ░░░░░ 
EOF
);tput setaf default

cols=$(tput cols)
while IFS= read -r line; do
    if (( ${#line} > cols )); then
        echo "${line:0:cols}"
    else
        padding=$(( (cols - ${#line}) / 2 ))
        printf "%${padding}s%s\n" "" "$line"
    fi
tput setaf $1
done <<< "$banner"
tput setaf default

echo "Starting the installation process..."

echo "Cloning repository from GitHub..."
$clone_comand
cd "$repo_dir" || { echo "Failed to enter directory $repo_dir"; exit 1; }

echo "Initializing build process..."
bar_length=40
total_steps=100

for i in $(seq 1 $total_steps); do
    num_hashes=$((i * bar_length / total_steps)) 
    num_equals=$((bar_length - num_hashes))  

    progress="["
    for j in $(seq 1 $num_hashes); do progress+="#"; done
    for k in $(seq 1 $num_equals); do progress+="="; done
    progress+="]"
    percentage=$((i))
    echo -ne "\r$progress $percentage%"
    sleep 0.01
done
echo

if [[ -f /etc/os-release ]]; then
    HOST_ID=$(grep "^ID=" /etc/os-release | cut -d'=' -f2 |  tr -d '"')
    HOST_PRETTY_NAME=$(grep "^PRETTY_NAME=" /etc/os-release | cut -d'=' -f2 |  tr -d '"')
    ID=$HOST_ID
    PRETTY_NAME=$HOST_PRETTY_NAME
else
    echo "/etc/os-release not found. Exiting."
    ID=''
    PRETTY_NAME=''
fi

echo "Building container..."
$build_command

echo "Starting container..."
$start_command

cd ..
rm -rf $repo_dir

docker ps

echo "Installation completed successfully!"