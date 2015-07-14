var img_width, img_height;

var srcCanvas, mainCanvas;
var srcContext, mainContext;

var _fileReader = new FileReader();
var urlCreator = window.URL || window.webkitURL;

var _x,_y, _width, _height, _color, _srcData, _mainData, _testSum, _mainSum, _dataLength;
var _r, _g, _b;

var _squaresPerCycle = 20;	// number of square plotting cycles per frame.
var _squareMaxSize = 5;
var _squareMinSize = 1;

var callFrame = window.requestAnimationFrame ||
		window.webkitRequestAnimationFrame ||
		window.mozRequestAnimationFrame    ||
		window.oRequestAnimationFrame      ||
		window.msRequestAnimationFrame     ||
		null;


function main_init() {
	srcCanvas = document.getElementById('srcCanvas');
	mainCanvas = document.getElementById('mainCanvas');
	
	srcContext = srcCanvas.getContext('2d');
	mainContext = mainCanvas.getContext('2d');

	if(!callFrame) {
		console.log("main: frame handler error: No frame handler!");
		return;
	}
	
	handleImgSelect(document.getElementById("img_5"));
	
};

function handleFileInput() {
	document.getElementById('fileInput').click();
};

function handleImgSelect(img) {
	img_width = img.naturalWidth;
	img_height = img.naturalHeight;

	srcCanvas.width = img_width;
	srcCanvas.height = img_height;
	mainCanvas.width = img_width;
	mainCanvas.height = img_height;
	
	srcContext.drawImage(img, 0, 0);
	
	var main_loop = function() {
		renderFrame();
		callFrame(main_loop);
	};
	
	mainContext.fillStyle = "#000000";
	mainContext.fillRect(0,0,img_width,img_height);
	
	callFrame(main_loop);
	
};


function loadFile(files) {
	if(!files[0].type.match('image.*'))  return false;
	
	_fileReader.readAsArrayBuffer(files[0]);
	document.getElementById('fileInfo').innerHTML = files[0].name;
	_fileReader.onload = function(e) {
		var dataURL = urlCreator.createObjectURL(new Blob( [new Uint8Array(_fileReader.result)], {
			type: "image/png"
		} ));

		var img = new Image();
		img.onload = function() {
			handleImgSelect(img);
		};
		
		img.src = dataURL;
	}
};


function renderFrame() {
	for(var k = 0; k < _squaresPerCycle; k++) {
		_x = (Math.random()*img_width)|0;
		_y = (Math.random()*img_height)|0;

		_width = _squareMinSize+(Math.random()*_squareMaxSize)|0;
		_height = _squareMinSize+(Math.random()*_squareMaxSize)|0;

		_r = (Math.random()*255)|0;
		_g = (Math.random()*255)|0;
		_b = (Math.random()*255)|0;

		_srcData = srcContext.getImageData(_x,_y,_width,_height);
		_mainData = mainContext.getImageData(_x,_y,_width,_height);
		
		_testSum = 0;
		_mainSum = 0;	

		_dataLength = _srcData.data.length;
		for(var i = 0; i < _dataLength; i+=4) {
			_testSum += Math.abs(_srcData.data[i] - _r);
			_testSum += Math.abs(_srcData.data[i+1] - _g);
			_testSum += Math.abs(_srcData.data[i+2] - _b);

			_mainSum += Math.abs(_srcData.data[i] - _mainData.data[i]);
			_mainSum += Math.abs(_srcData.data[i+1] - _mainData.data[i+1]);
			_mainSum += Math.abs(_srcData.data[i+2] - _mainData.data[i+2]);
		}
		
		if(_testSum < _mainSum) {
			mainContext.fillStyle = "rgb("+_r+","+_g+","+_b+")";
			mainContext.fillRect(_x,_y,_width,_height);
		}
	}
};

