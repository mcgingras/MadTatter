// ---------------------------------------------------------------------------
// SVG PAINT
// draw your own SVG!
// Michael Gingras
// ---------------------------------------------------------------------------

var svgCanvas = document.getElementById("svgCanvas");
var svgPolyline;
svgCanvas.addEventListener("mousedown", startDrawTouch, false);
svgCanvas.addEventListener("touchmove", continueDrawTouch, false);
svgCanvas.addEventListener("touchend", endDrawTouch, false);

function startDrawTouch(event){
  console.log("ttt");
  var touch = event.changedTouches[0];
  svgPolyline = createSvgElement("polyline");
  svgPolyline.setAttribute("fill", "none");
  svgPolyline.setAttribute("shape-rendering", "geometricPrecision");
  svgPolyline.setAttribute("stroke-linejoin", "round");
  svgPolyline.setAttribute("stroke", "#000000");

  svgCanvas.appendChild(svgPolyline);
  continueDrawTouch(event);
}

function continueDrawTouch(event)
{
    if (svgPolyline)
    {
      var touch = event.changedTouches[0];
      var point = svgPolyline.ownerSVGElement.createSVGPoint();
      point.x = touch.clientX;
      point.y = touch.clientY;
      var ctm = event.target.getScreenCTM();
      if (ctm = ctm.inverse())
      {
        point = point.matrixTransform(ctm);
      }
      svgPolyline.points.appendItem(point);
    }
}

function endDrawTouch(event)
{
  continueDrawTouch(event);
  svgPolyline = null;
}

function createSvgElement(tagName)
{
  return document.createElementNS("http://www.w3.org/2000/svg", tagName);
}
