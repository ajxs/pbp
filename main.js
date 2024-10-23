let imageWidth = 0;
let imageHeight = 0;

let sourceCanvas = null;
let canvas_main = null;
let sourceContext = null;
let mainContext = null;

/**
 * The number of square plotting cycles per frame.
 * Increasing this number will result in faster painting of the image, but
 * higher numbers will have a noticeable performance on the browser.
 */
const squaresPerFrame = 24;
const squareMaxSize = 5;
const squareMinSize = 1;


function initialise() {
	sourceCanvas = document.getElementById('canvas_source');
	canvas_main = document.getElementById('canvas_main');

	sourceContext = sourceCanvas.getContext('2d');
	mainContext = canvas_main.getContext('2d');

	handleImgSelect(document.getElementById("img_5"));
};


function handleFileInput() {
	document.getElementById('fileInput').click();
};


function handleImgSelect(img) {
	imageWidth = img.naturalWidth;
	imageHeight = img.naturalHeight;

	sourceCanvas.width = imageWidth;
	sourceCanvas.height = imageHeight;
	canvas_main.width = imageWidth;
	canvas_main.height = imageHeight;

	sourceContext.drawImage(img, 0, 0);

	const mainLoop = function () {
		renderFrame();
		window.requestAnimationFrame(mainLoop);
	};

	mainContext.fillStyle = "#000000";
	mainContext.fillRect(0, 0, imageWidth, imageHeight);

	window.requestAnimationFrame(mainLoop);
};


function loadFile(files) {
	if (!files[0].type.match('image.*')) {
		return false;
	}

	let img = new Image();
	img.onload = function () {
		handleImgSelect(img);
	};

	img.src = URL.createObjectURL(files[0]);
};


function renderFrame() {
	for (let k = 0; k < squaresPerFrame; k++) {
		// Randomly select the origin, and size of the square.
		const squareOriginX = (Math.random() * imageWidth) | 0;
		const squareOriginY = (Math.random() * imageHeight) | 0;

		const squareWidth = squareMinSize + (Math.random() * squareMaxSize) | 0;
		const squareHeight = squareMinSize + (Math.random() * squareMaxSize) | 0;

		// Randomly select a colour for the square.
		const r = (Math.random() * 255) | 0;
		const g = (Math.random() * 255) | 0;
		const b = (Math.random() * 255) | 0;

		// Get the pixel data for the randomly selected square.
		const sourceImageData = sourceContext.getImageData(
			squareOriginX,
			squareOriginY,
			squareWidth,
			squareHeight
		);

		const mainImageData = mainContext.getImageData(
			squareOriginX,
			squareOriginY,
			squareWidth,
			squareHeight
		);

		let testTotalDeviation = 0;
		let mainTotalDeviation = 0;

		const totalSquarePixelDataLength = sourceImageData.data.length;

		// Sum the the total deviation between the red, green and blue channels of the
		// pixels in the source image, and the pixels of the random square being plotted.
		// If the pixels in the test square have less deviation from the source image than
		// the current randomly plotted pixels, then plot the test square.
		for (let i = 0; i < totalSquarePixelDataLength; i += 4) {
			testTotalDeviation += Math.abs(sourceImageData.data[i] - r)
				+ Math.abs(sourceImageData.data[i + 1] - g)
				+ Math.abs(sourceImageData.data[i + 2] - b);

			mainTotalDeviation += Math.abs(sourceImageData.data[i] - mainImageData.data[i])
				+ Math.abs(sourceImageData.data[i + 1] - mainImageData.data[i + 1])
				+ Math.abs(sourceImageData.data[i + 2] - mainImageData.data[i + 2]);
		}

		if (testTotalDeviation < mainTotalDeviation) {
			mainContext.fillStyle = "rgb(" + r + "," + g + "," + b + ")";
			mainContext.fillRect(squareOriginX, squareOriginY, squareWidth, squareHeight);
		}
	}
};

