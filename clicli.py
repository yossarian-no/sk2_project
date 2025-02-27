import tkinter as tk
from tkinter import scrolledtext, messagebox
import socket
import threading

class TelnetClientGUI:
    def __init__(self, master):
        self.master = master
        self.master.title("Telnet Client")
        self.master.geometry("600x400")

        self.client_socket = None
        self.receive_thread = None

        self.create_widgets()

    def create_widgets(self):
        self.host_label = tk.Label(self.master, text="Server IP:")
        self.host_label.pack(pady=5)
        
        self.host_entry = tk.Entry(self.master)
        self.host_entry.pack(pady=5, fill=tk.X, padx=10)
        self.host_entry.insert(0, "127.0.0.1")

        self.port_label = tk.Label(self.master, text="Port:")
        self.port_label.pack(pady=5)

        self.port_entry = tk.Entry(self.master)
        self.port_entry.pack(pady=5, fill=tk.X, padx=10)
        self.port_entry.insert(0, "8888")

        self.connect_button = tk.Button(self.master, text="Connect", command=self.connect_to_server)
        self.connect_button.pack(pady=5)

        self.text_area = scrolledtext.ScrolledText(self.master, wrap=tk.WORD, state='disabled')
        self.text_area.pack(padx=10, pady=10, fill=tk.BOTH, expand=True)

        self.entry_field = tk.Entry(self.master)
        self.entry_field.pack(padx=10, pady=5, fill=tk.X, expand=True)
        self.entry_field.bind("<Return>", self.send_command)

        self.send_button = tk.Button(self.master, text="Send", command=self.send_command)
        self.send_button.pack(pady=5)

        self.disconnect_button = tk.Button(self.master, text="Disconnect", command=self.disconnect_from_server, state='disabled')
        self.disconnect_button.pack(pady=5)

    def connect_to_server(self):
        host = self.host_entry.get()
        port = int(self.port_entry.get())

        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect((host, port))

            self.text_area.config(state='normal')
            self.text_area.insert(tk.END, f"Connected to {host}:{port}\n")
            self.text_area.config(state='disabled')

            self.connect_button.config(state='disabled')
            self.disconnect_button.config(state='normal')

            self.receive_thread = threading.Thread(target=self.receive_data, daemon=True)
            self.receive_thread.start()
        except Exception as e:
            messagebox.showerror("Connection Error", f"Failed to connect: {e}")

    def disconnect_from_server(self):
        if self.client_socket:
            self.client_socket.close()
            self.client_socket = None

        self.text_area.config(state='normal')
        self.text_area.insert(tk.END, "Disconnected from server\n")
        self.text_area.config(state='disabled')

        self.connect_button.config(state='normal')
        self.disconnect_button.config(state='disabled')

    def send_command(self, event=None):
        if self.client_socket:
            command = self.entry_field.get().strip()
            if command:
                try:
                    self.client_socket.sendall(command.encode('utf-8'))
                    self.entry_field.delete(0, tk.END)
                except Exception as e:
                    messagebox.showerror("Error", f"Failed to send: {e}")

    def receive_data(self):
        while True:
            try:
                data = self.client_socket.recv(1024)
                if not data:
                    break
                self.display_text(data.decode('utf-8'))
            except:
                break

    def display_text(self, text):
        self.text_area.config(state='normal')
        self.text_area.insert(tk.END, text + '\n')
        self.text_area.config(state='disabled')
        self.text_area.yview(tk.END)

if __name__ == "__main__":
    root = tk.Tk()
    app = TelnetClientGUI(root)
    root.mainloop()
