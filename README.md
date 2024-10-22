# BUILD

git clone https://github.com/ShitCodeTech/DockerMonitoringToolkit
cd DockerMonitoringToolkit

docker build -t server_monitor .

docker run --privileged -p 8080:8080

python -m http.server 8000

http://127.0.0.1:8000/123.html