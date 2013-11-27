#!/usr/bin/python

import sys
import time
import zmq

from Tkinter import Tk, Canvas, PhotoImage, mainloop
import threading
import itertools as it


def grouper(iterable, n):
    "Collect data into fixed-length chunks or blocks"
    # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx
    args = [iter(iterable)] * n
    return it.izip(*args)

def refresh(img, receiver, root):
    for task_nbr in range(768):
        s = receiver.recv()
        decompose = s.split();
        line_no = int(decompose[1])
        print line_no
        x = 0
        for r, g, b in grouper(decompose[3:],3):
            r = int(r)
            g = int(g)
            b = int(b)
            img.put("#%02x%02x%02x" % (r,g,b), (x, 767-line_no))
            x+=1
        root.update()

if __name__ == "__main__":

    #GUI part
    WIDTH, HEIGHT = 1024, 768
    window = Tk()
    canvas = Canvas(window, width=WIDTH, height=HEIGHT, bg="#000000")
    canvas.pack()
    img = PhotoImage(width=WIDTH, height=HEIGHT)
    canvas.create_image((WIDTH/2, HEIGHT/2), image=img, state="normal")

    #0MQ kicks in:
    context = zmq.Context()

    # Socket to receive messages on
    receiver = context.socket(zmq.PULL)
    receiver.bind("tcp://*:5558")

    # Wait for start of batch
    s = receiver.recv()

    #start watching thread
    th = threading.Thread(target=refresh, args=(img, receiver, window))
    th.daemon = True
    th.start()

    window.mainloop()

    


