var parentDiv = document.getElementById('parent');
var width = parentDiv.clientWidth;
var height = parentDiv.clientHeight;
var padding = 30;

var px = 100;
var py = 0;
var MM_PER_SEGMENT = 1;

var svg = d3.select("#plot")
.append("svg")
.attr("height", height)
.attr("width", width)
.attr("class", 'plot');

var xScale = d3.scaleLinear().domain([0, 400]).range([0, width]);
var yScale = d3.scaleLinear().domain([0, 400]).range([height, 0]);


var xScale2 = d3.scaleLinear().domain([0, .5]).range([0, width]);
var yScale2 = d3.scaleLinear().domain([0, .5]).range([height, 0]);


function atan3(dy,dx) {
  var a = Math.atan2(dy,dx);
  if(a<0) a = (Math.PI*2.0)+a;
  return a;
}

function arc(x,y,cx,cy,dir){
  // get radius
  var dx = px - cx;
  var dy = py - cy;
  var radius=Math.sqrt(dx*dx+dy*dy);

  // find angle of arc (sweep)
  var angle1=atan3(dy,dx);
  var angle2=atan3(y-cy,x-cx);
  var theta=angle2-angle1;

  if(dir>0 && theta<0) angle2+=2*Math.PI;
  else if(dir<0 && theta>0) angle1+=2*Math.PI;

  theta=angle2-angle1;
  // get length of arc
  var len      = Math.abs(theta) * radius;
  var segments = Math.ceil( len * MM_PER_SEGMENT );

  var nx, ny, angle3, scale;

  for(i=0;i<segments;++i) {
    // interpolate around the arc
    scale = (i)/(segments);
    console.log('does this get written');

    angle3 = ( theta * scale ) + angle1;
    nx = cx + Math.cos(angle3) * radius;
    ny = cy + Math.sin(angle3) * radius;
    // send it to the planner
    line(nx,ny);

    console.log(px);
    console.log(py);
    px = nx;
    py = ny;
  }

  line(x,y);

}

function line(newx,newy) {
  var i;
  var over = 0;

  var dx  = newx-px;
  var dy  = newy-py;
  var dirx = dx>0?1:-1;
  var diry = dy>0?1:-1;
  dx = Math.abs(dx);
  dy = Math.abs(dy);


  if(dx>dy) {
    over = dx/2;
    for(i=0; i<dx; ++i) {
      svg.append('line')
      .attr('x1', xScale(px))
      .attr('x2', xScale(px+dirx))
      .attr('y1', yScale(py))
      .attr('y2', yScale(py))
      .attr('stroke-width', 4)
      .attr('stroke', 'black');
      px += dirx;
      over += dy;
      if(over>=dx) {
        over -= dx;
        svg.append('line')
        .attr('x1', xScale(px))
        .attr('x2', xScale(px))
        .attr('y1', yScale(py))
        .attr('y2', yScale(py+diry))
        .attr('stroke-width', 4)
        .attr('stroke', 'black');
        py += diry;
      }
    }
  } else {
    over = dy/2;
    for(i=0; i<dy; ++i) {
      svg.append('line')
      .attr('x1', xScale(px))
      .attr('x2', xScale(px))
      .attr('y1', yScale(py))
      .attr('y2', yScale(py+diry))
      .attr('stroke-width', 4)
      .attr('stroke', 'black');
      py += diry;
      over += dx;
      if(over >= dy) {
        over -= dy;
        svg.append('line')
        .attr('x1', xScale(px))
        .attr('x2', xScale(px+dirx))
        .attr('y1', yScale(py))
        .attr('y2', yScale(py))
        .attr('stroke-width', 4)
        .attr('stroke', 'black');
        px += dirx;
      }
    }
  }

  px = newx;
  py = newy;
}

line(100,100);
arc(100,0,50,0,1);
