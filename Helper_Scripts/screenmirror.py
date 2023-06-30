# Hit strg+s to save a screenshot

import tkinter as tk
from tkinter import simpledialog
import requests
import json
import PIL.ImageGrab

class LedScreenApp:
    def __init__(self, root, ip_address):
        self.root = root
        self.ip_address = ip_address
        self.width = 32
        self.height = 8
        self.labels = []
        
        screen_width = root.winfo_screenwidth()
        label_width = (screen_width // 32) // 2
        label_height = label_width
        padding = 1
        top_margin = 0

        # Erstellen der Labels
        for y in range(self.height):
            row = []
            for x in range(self.width):
                label = tk.Label(root, bg="black")
                label.place(x=x * (label_width + padding), y=top_margin + y * (label_height + padding), width=label_width, height=label_height)
                row.append(label)
            self.labels.append(row)

        # Festlegen der Fenstergröße, um alle Labels aufzunehmen
        window_width = self.width * (label_width + padding)
        window_height = self.height * (label_height + padding) + top_margin
        root.geometry(f"{window_width-1}x{window_height-1}")


        # Shortcut zum Speichern des Bildes als PNG
        root.bind('<Control-s>', self.save_image)

        # Starte den screen_tick, um die Farben regelmäßig zu aktualisieren
        self.screen_tick()

    def screen_tick(self):
        # HTTP-Anfrage senden und Farben abrufen
        response = requests.get(f"http://{self.ip_address}/api/screen")
        if response.status_code == 200:
            self.set_led_colors(response.text)
        else:
            print("Fehler beim Abrufen der Farben:", response.status_code)
        
        # Wiederhole den screen_tick nach 1000 ms
        self.root.after(250, self.screen_tick)

    def set_led_colors(self, json_string):
        # Farben aus der JSON-Antwort extrahieren
        colors_list = json.loads(json_string)
        for i, color in enumerate(colors_list):
            y = i // self.width
            x = i % self.width
            r = (color & 0xFF0000) >> 16
            g = (color & 0x00FF00) >> 8
            b = color & 0x0000FF
            self.labels[y][x].config(bg=f"#{r:02x}{g:02x}{b:02x}")

    def save_image(self, event):
        # Füge eine kurze Verzögerung hinzu, bevor das Bild aufgenommen wird
        self.root.after(200, self.capture_image)

    def capture_image(self):
        # Bild als PNG speichern
        x = self.root.winfo_rootx()
        y = self.root.winfo_rooty()
        w = self.root.winfo_width()
        h = self.root.winfo_height()
        image = PIL.ImageGrab.grab(bbox=(x, y, x + w, y + h))
        image.save("screenshot.png")


# Hauptteil des Programms
root = tk.Tk()
root.resizable(False, False)
ip_address = simpledialog.askstring("AWTRIX LIGHT", "Please enter IP address:")
app = LedScreenApp(root, ip_address)
root.mainloop()