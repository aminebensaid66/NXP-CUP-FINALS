import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque
import re

# === CONFIG ===
PORT = '/dev/cu.usbmodem166233301'  # Change if needed
BAUD = 115200
MAX_POINTS = 100

# === INIT SERIAL ===
ser = serial.Serial(PORT, BAUD, timeout=1)

# === INIT PLOTTING ===
fig, ax = plt.subplots()
lines = {}
data = {}
colors = ['blue', 'red', 'green', 'orange', 'purple', 'cyan']

def update(frame):
    global lines, data
    try:
        # Read all available lines to reduce latency
        while ser.in_waiting:
            line = ser.readline().decode('utf-8').strip()
            if not line:
                continue

            # Match "Label: value"
            matches = re.findall(r'(\w+):\s*(-?\d+)', line)
            for label, value in matches:
                value = float(value)

                if label not in data:
                    data[label] = deque([0]*MAX_POINTS, maxlen=MAX_POINTS)
                    color = colors[len(data) % len(colors)]
                    lines[label], = ax.plot(data[label], label=label, color=color)
                    ax.legend()

                data[label].append(value)

        # Update plots
        for label in data:
            lines[label].set_ydata(data[label])
            lines[label].set_xdata(range(len(data[label])))

        ax.relim()
        ax.autoscale_view()

    except Exception as e:
        print("Error:", e)

# Faster refresh rate and smoother animations
ani = animation.FuncAnimation(fig, update, interval=5, blit=False)
plt.xlabel("Time")
plt.ylabel("Value")
plt.title("Real-Time Labeled Serial Plot")
plt.tight_layout()
plt.show()