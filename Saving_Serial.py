import serial
import threading
import tkinter as tk
from tkinter import messagebox
import os

class SerialSaverApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Serial Monitor Saver")

        # Fixed save directory
        self.save_directory = "/Users/aminebensaid/Documents/nxp/NXP V2 TEENSY4 PIXY2/test/data storing"
        os.makedirs(self.save_directory, exist_ok=True)

        # GUI elements
        self.filename_label = tk.Label(root, text="Filename:")
        self.filename_label.pack()

        self.filename_entry = tk.Entry(root, width=50)
        self.filename_entry.pack()

        self.start_button = tk.Button(root, text="Start Saving", command=self.start_saving)
        self.start_button.pack(pady=5)

        self.stop_button = tk.Button(root, text="Stop Saving", command=self.stop_saving, state=tk.DISABLED)
        self.stop_button.pack(pady=5)

        self.status_label = tk.Label(root, text="Status: Idle", fg="blue")
        self.status_label.pack(pady=10)

        # Serial settings
        self.port = "/dev/cu.usbmodem166233301"  # Change this to your actual serial port
        self.baudrate = 115200

        # Control variables
        self.running = False
        self.thread = None

    def start_saving(self):
        filename = self.filename_entry.get().strip()
        if not filename:
            messagebox.showerror("Error", "Please enter a filename.")
            return

        full_path = os.path.join(
            self.save_directory, filename if filename.endswith(".txt") else filename + ".txt"
        )

        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
        except serial.SerialException as e:
            messagebox.showerror("Serial Error", str(e))
            return

        self.running = True
        self.thread = threading.Thread(target=self.save_data, args=(full_path,))
        self.thread.start()
        self.status_label.config(text=f"Saving to: {full_path}", fg="green")
        self.start_button.config(state=tk.DISABLED)
        self.stop_button.config(state=tk.NORMAL)

    def stop_saving(self):
        self.running = False
        self.thread.join()
        self.ser.close()
        self.status_label.config(text="Status: Stopped", fg="red")
        self.start_button.config(state=tk.NORMAL)
        self.stop_button.config(state=tk.DISABLED)

    def save_data(self, filename):
        with open(filename, "a") as f:
            while self.running:
                if self.ser.in_waiting:
                    line = self.ser.readline().decode(errors='replace').strip()
                    f.write(line + "\n")
                    f.flush()

if __name__ == "__main__":
    root = tk.Tk()

    # macOS fix: bring window to front
    root.lift()
    root.attributes('-topmost', True)
    root.after_idle(root.attributes, '-topmost', False)

    app = SerialSaverApp(root)
    root.mainloop()