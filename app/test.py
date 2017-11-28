import subprocess

svg = subprocess.Popen(('cat', './lib/svg2gcode/t20.svg'), stdout=subprocess.PIPE)
output = subprocess.check_output(('python', './lib/svg2gcode/svg2gcode.py'),stdin=svg.stdout)

print(output)
