from app import app
from flask import render_template

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/about')
def about():
    return render_template('about.html')

@app.route('/draw')
def draw():
    return render_template('draw.html')

@app.route('/tattoo')
def tat():
    return render_template('tattoo.html')

@app.route('/submissions')
def submissions():
    return render_template('submissions.html')

@app.route('/live')
def live():
    return render_template('live.html')

@app.route('/visualize')
def vis():
    return render_template('visualize.html')
