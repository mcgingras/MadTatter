from app import app
from flask import render_template, request
import subprocess
import os

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/draw')
def draw():
    return render_template('draw.html')

@app.route('/tattoo')
def tat():
    return render_template('tattoo.html')
