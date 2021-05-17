
import queue
import sys
import socket
import threading
import scipy.signal as sc
import random

from matplotlib.animation import FuncAnimation
import matplotlib.pyplot as plt
import numpy as np
import sounddevice as sd

UDP_IP1 = "172.24.133.68"
UDP_IP2 = "172.24.134.109"
UDP_PORT = 4445

last_s = 0
s = 0

lock = threading.Lock()
global_beat = 0
last_global_beat = 0

length = 100
plotdata = np.zeros((length, 1))

peaks = 10;

def audio():
    global global_beat
    global last_global_beat
    global plotdata
    
    device = 2

    print(sd.query_devices())

    samplerate = sd.query_devices(device, 'input')['default_samplerate']
    fftsize = 1024
    q = queue.Queue()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    fig, ax = plt.subplots()
    lines = ax.plot(plotdata)
    ax.axis((0, length, 0, 150))
    ax.set_yticks([0])
    ax.yaxis.grid(True)
    ax.tick_params(bottom=False, top=False, labelbottom=False,
                    right=False, left=False, labelleft=False)
    fig.tight_layout(pad=0)

    def update_plot(frame):
        global plotdata
        global global_beat
        global last_global_beat
        global peaks
        while True:
            try:
                data, sum = q.get_nowait()
            except queue.Empty:
                break

            plotdata = np.roll(plotdata, -1, axis=0)
            plotdata[0] = data
            last_global_beat = global_beat
            global_beat = data
            #print(plotdata.reshape(-1))
            #peaks, _ = sc.find_peaks(plotdata[:50].reshape(-1), height=20, distance=3, prominence=0.6)
            
            if (global_beat > 30 and last_global_beat < 30):
                print(int((np.log10(sum)-1.9)*200))
                # first 2 bytes: type of control packet
                # second 2 bytes: beat strength
                # third 2 bytes: number of peaks (energy)
                MESSAGE = "0" + intToChrConstrained(global_beat/2) + intToChrConstrained(int((np.log10(sum)-1.9)*400))
                #MESSAGE2 = "1" + intToChrConstrained(global_beat/2) + intToChrConstrained(int(random.randint(0, 255)))
                sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP1, UDP_PORT))
                #sock.sendto(bytes(MESSAGE2, "utf-8"), (UDP_IP2, UDP_PORT))
            
        for column, line in enumerate(lines):
            line.set_ydata(plotdata[:, column])
        return lines

    
    def callback(indata, frames, time, status):
        global s, last_s
        if any(indata):
            magnitude = np.abs(np.fft.rfft(indata[:, 0], n=fftsize*2))
            noise = magnitude < 0.1
            magnitude[noise] = 0
            magnitude = [np.log(1+m)/np.log(1+i) for i, m in enumerate(magnitude)]
            #magnitude /= max(max(magnitude), 10)
            last_s = s
            s = sum(magnitude[1:])
            q.put((max(s-last_s, 0), s));
        else:
            print('no input')

    stream = sd.InputStream(device=device, channels=1, callback=callback,
                    blocksize=fftsize,
                    samplerate=samplerate)
    ani = FuncAnimation(fig, update_plot, interval=30, blit=True)
    with stream:
        plt.show()

def intToChrConstrained(x):
    return chr(min(255, max(0, int(x))))
    
if __name__ == "__main__":
    audio()

