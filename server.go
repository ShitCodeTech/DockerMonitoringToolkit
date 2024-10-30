package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os/exec"
	"strings"
	"time"

	"github.com/gorilla/websocket"
)

type User struct {
	CPU    string `json:"cpu"`
	Mem    string `json:"mem"`
	Uptime string `json:"uptime"`
	IP     string `json:"ip"`
	Name   string `json:"name"`
	ID     string `json:"id"`
}

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

func main() {
	http.HandleFunc("/ws", handleWebSocket)
	log.Println("Starting server on :8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func handleWebSocket(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println("Error upgrading to websocket:", err)
		return
	}
	defer conn.Close()

	// Continuously send data every second
	ticker := time.NewTicker(1 * time.Second)
	defer ticker.Stop()

	for range ticker.C {
		// Run the metrics_collector command and parse the output
		user, err := collectMetrics()
		if err != nil {
			log.Println("Error collecting metrics:", err)
			return
		}

		// Convert User struct to JSON
		userJSON, err := json.Marshal(user)
		if err != nil {
			log.Println("Error marshalling JSON:", err)
			return
		}

		// Send JSON data over WebSocket
		err = conn.WriteMessage(websocket.TextMessage, userJSON)
		if err != nil {
			log.Println("Error sending JSON over WebSocket:", err)
			return
		}
	}
}

func collectMetrics() (User, error) {
	cmd := exec.Command("./metrics_collector")
	out, err := cmd.CombinedOutput()
	if err != nil {
		return User{}, fmt.Errorf("command error: %w", err)
	}

	// Split the output by lines and parse it into User struct
	lines := strings.Split(strings.TrimSpace(string(out)), "\n")
	return parseLines(lines), nil
}

func parseLines(lines []string) User {
	user := User{}

	if len(lines) > 0 {
		user.CPU = lines[0]
	}
	if len(lines) > 1 {
		user.Mem = lines[1]
	}
	if len(lines) > 2 {
		user.Uptime = lines[2]
	}
	if len(lines) > 3 {
		user.IP = lines[3]
	}
	if len(lines) > 4 {
		user.Name = trimQuotes(lines[4])
	}
	if len(lines) > 5 {
		user.ID = lines[5]
	}

	return user
}

func trimQuotes(s string) string {
	if len(s) > 1 && s[0] == '"' && s[len(s)-1] == '"' {
		return s[1 : len(s)-1]
	}
	return s
}
