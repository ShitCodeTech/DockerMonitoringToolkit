// server.go
package main

import (
    "log"
    "net/http"
    "time"
    "github.com/gorilla/websocket"
    "os/exec"
)

var upgrader = websocket.Upgrader{
    ReadBufferSize:  1024,
    WriteBufferSize: 1024,
    CheckOrigin: func(r *http.Request) bool {
        return true
    },
}

func collectMetrics() (string, error) {
    cmd := exec.Command("./metrics_collector")
    out, err := cmd.CombinedOutput()  // Собирает stdout и stderr
    if err != nil {
        log.Printf("Error collecting metrics: %v, Output: %s", err, out)
        return "", err
    }
    return string(out), nil
}


func wsEndpoint(w http.ResponseWriter, r *http.Request) {
    conn, err := upgrader.Upgrade(w, r, nil)
    if err != nil {
        log.Println(err)
        return
    }
    defer conn.Close()

    for {
        metrics, err := collectMetrics()
        if err != nil {
            log.Println("Error collecting metrics:", err)
            return
        }
        
        err = conn.WriteMessage(websocket.TextMessage, []byte(metrics))
        if err != nil {
            log.Println("Error sending message:", err)
            return
        }

        time.Sleep(1* time.Second)  // Отправляем данные каждые n секунд
    }
}

func setupRoutes() {
    http.HandleFunc("/ws", wsEndpoint)
}

func main() {
    setupRoutes()
    log.Fatal(http.ListenAndServe(":8080", nil))
}
