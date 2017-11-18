function gridData() {
  var data = new Array();
  var xpos = 1;
  var ypos = 1;
  var width = 30;
  var height = 30;
  var click = 0;

  for (var row = 0; row < 40; row++) {
    data.push( new Array() );

    for (var column = 0; column < 40; column++) {
      data[row].push({
        x: xpos,
        y: ypos,
        width: width,
        height: height,
        click: click
      })
      xpos += width;
    }

    xpos = 1;
    ypos += height;
  }
  return data;
}

var gridData = gridData();

var grid = d3.select("#plot")
  .append("svg")
  .attr("width","100%")
  .attr("height","100%");

var row = grid.selectAll(".row")
  .data(gridData)
  .enter().append("g")
  .attr("class", "row");

var column = row.selectAll(".square")
  .data(function(d) { return d; })
  .enter().append("rect")
  .attr("class","square")
  .attr("x", function(d) { return d.x; })
  .attr("y", function(d) { return d.y; })
  .attr("width", function(d) { return d.width; })
  .attr("height", function(d) { return d.height; })
  .style("fill", "#fff")
  .style("stroke", "#222");
