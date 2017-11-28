from app import app
from flask import render_template, request
import subprocess
import os

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
    #this is where we would maybe want to call svg-gcode
    return render_template('tattoo.html')

@app.route('/convert')
def convert():
    svg_path = os.path.join(app.root_path, 'test.svg')
    py_path  = os.path.join(app.root_path, 'svg2gcode.py')
    try:
        svg = subprocess.Popen(("cat", svg_path), stdout=subprocess.PIPE)
        output = subprocess.check_output(('python', py_path),stdin=svg.stdout, shell=True)
        # print(output)
    except subprocess.CalledProcessError as e:
        raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))
    return "nice"

@app.route('/uploader', methods = ['GET', 'POST'])
def upload_file():
    if request.method == 'POST':
        f = request.files['file']
        return "posted"
    else:
        return "not a post"

@app.route('/submissions')
def submissions():
    return render_template('submissions.html')

@app.route('/live')
def live():
    return render_template('live.html')

@app.route('/visualize')
def vis():
    return render_template('visualize.html')
