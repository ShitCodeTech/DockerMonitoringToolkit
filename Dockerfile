# Dockerfile
FROM golang:latest as builder

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем Go код
COPY server.go .

# Устанавливаем зависимости и собираем Go приложение
RUN go mod init app && go get -u github.com/gorilla/websocket && go build -o websocket_server .

# Компилируем C++ код
FROM gcc:latest as cppbuilder
WORKDIR /app
COPY metrics_collector.cpp .
RUN g++ -o metrics_collector metrics_collector.cpp

# Финальный образ
FROM ubuntu:latest

# Копируем файлы из предыдущих стадий
COPY --from=builder /app/websocket_server /app/
COPY --from=cppbuilder /app/metrics_collector /app/

WORKDIR /app

# Запуск веб-сервера
CMD ["./websocket_server"]

