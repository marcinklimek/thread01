#!/bin/bash
#start  workers
worker/build/worker &
worker/build/worker &
worker/build/worker &

producer/build/producer &

sink-py/sink.py &

