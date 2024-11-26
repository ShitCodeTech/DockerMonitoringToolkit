# From sources
```
git clone https://github.com/ShitCodeTech/DockerMonitoringToolkit

cd DockerMonitoringToolkit/serverside

docker build --build-arg ID=<id of your system in /etc/os-release> --build-arg PRETTY_NAME=<pretty_name in same file(/etc/os-release)> -t server_monitor .`

docker run  --privileged -p 8080:8080 -d server_monitor

cd ../clientside

python -m http.server 8000

http://127.0.0.1:8000/server_test.html
```
# autoinstall 

```
wget  https://github.com/ShitCodeTech/DockerMonitoringToolkit/releases/download/v0.1.0-alpha/autobuild.sh

wget https://github.com/ShitCodeTech/DockerMonitoringToolkit/blob/rolling/client/server_test.html

chmod +x autoinstall.sh

./autoinstall

python -m http.server 8000

http://127.0.0.1:8000/server_test.html
```
