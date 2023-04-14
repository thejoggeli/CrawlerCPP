function Gallery(){}
Gallery.init = function(){
	$("#gallery .image-upload").on("change", function(e){
		e.preventDefault();
		e.stopPropagation();
		Gallery.processFiles(e.originalEvent.target.files);
	});
	$("#gallery .image-drop").on("click", function(e){
		if($(e.target).hasClass("image-drop")){
			$("#gallery .image-upload").trigger("click")
		}
	})
	$("#gallery .image-drop").on("dragenter", function(e){
		e.preventDefault()
		e.stopPropagation()
		$(this).addClass("drag")
	})
	$("#gallery .image-drop").on("dragleave", function(e){
		e.preventDefault()
		e.stopPropagation()
		$(this).removeClass("drag")
	})
	$("#gallery .image-drop").on("dragover", function(e){
		e.preventDefault()
		e.stopPropagation()
	})
	$("#gallery .image-drop").on("drop", function(e){
		e.preventDefault()
		e.stopPropagation()
		$(this).removeClass("drag")
		var ee = e.originalEvent
		var dt = ee.dataTransfer
		var files = dt.files
		Gallery.processFiles(files)
	})
	$("#gallery .begin-upload").on("click", function(){
		Gallery.uploadFile()
	})
	var normal_str = "Normal"
	var reverse_str = "Normal-Reverse"
	$("#gallery .playback-mode").on("click", function(){
		if($(this).val() == normal_str){
			$(this).val(reverse_str)
		} else {
			$(this).val(normal_str)
		}
		switch($(this).val()){
			case normal_str:
				Gallery.playbackMode = "normal"; break;
			case reverse_str:
				Gallery.playbackMode = "reverse"; break;
		}
	})
	var fit_str = "Fit (black borders)"
	var fill_str = "Fill (no black borders)"
	var stretch_str = "Stretch"
	$("#gallery .crop-mode").on("click", function(){
		if($(this).val() == fill_str){
			$(this).val(fit_str)
		} else if($(this).val() == fit_str){
			$(this).val(stretch_str)
		} else {
			$(this).val(fill_str)
		}
		switch($(this).val()){
			case fit_str:
				Gallery.cropMode = "fit"; break;
			case fill_str:
				Gallery.cropMode = "fill"; break;
			case stretch_str:
				Gallery.cropMode = "stretch"; break;
		}
	})
	$("#gallery .video-fps").on("input", function(){
		Gallery.fps = $(this).val()
		$(".video-fps-value").text(Gallery.fps)
	})
	$("#gallery .video-scale").on("input", function(){
		Gallery.scale = $(this).val()
		$(".video-scale-value").text(Gallery.scale)
	})
	$("#gallery .crop-mode").val(fill_str)
	$("#gallery .playback-mode").val(normal_str)
	Gallery.setScale(8)
	Gallery.setFps(15)
	Gallery.playbackMode = "normal"
	Gallery.cropMode = "fill"
	Gallery.offscreen = {}
	Gallery.offscreen.canvas = document.createElement("canvas")
	Gallery.offscreen.ctx = Gallery.offscreen.canvas.getContext("2d", {preserveDrawingBuffer: true})	
	Gallery.videoCanvas = $("#gallery .video-canvas")[0];
	Gallery.videoCtx = Gallery.videoCanvas.getContext("2d")
}
Gallery.setScale = function(s){
	Gallery.scale = s
	$(".video-scale").val(Gallery.scale)
	$(".video-scale-value").text(Gallery.scale)
}
Gallery.setFps = function(fps){
	Gallery.fps = fps
	$(".video-fps").val(Gallery.fps)
	$(".video-fps-value").text(Gallery.fps)
}
Gallery.open = function(){	
	Gallery.restoreDefaultView()
	MatrixClient.addEventListener(Gallery);
}
Gallery.close = function(){
	MatrixClient.removeEventListener(Gallery);	
}
Gallery.restoreDefaultView = function(){
	$("#gallery .video-options").hide()
	$("#gallery .image-drop").show()
	$("#gallery .video-target").hide()
	$("#gallery .loading-overlay").hide()
	$("#gallery .gamepad-button").show()
	$("#gallery .image-upload").val("")
	$("#gallery .gallery-title").text("Gallery")
}
Gallery.processFiles = function(files){
	var file = files[0]	
	console.log("file type: " + file.type)
	Gallery.file = file
	$("#gallery .image-drop").hide()
	$("#gallery .video-target").hide()
	$("#gallery .gamepad-button").hide()
	$("#gallery .video-options").show()
	$("#gallery .video-options .option-row").hide()
	if(file.type.match("^image\/.*$")){
		$("#gallery .video-options .option-scale").show()
		$("#gallery .video-options .option-crop").show()
		$("#gallery .gallery-title").text("Image options")		
		$("#gallery .video-options .begin-upload").text("Upload image")		
		Gallery.setScale(8)
		Gallery.setFps(1)
	} else if(file.type.match("^video\/.*$")){
		$("#gallery .video-options .option-scale").show()
		$("#gallery .video-options .option-crop").show()
		$("#gallery .video-options .option-playback").show()
		$("#gallery .video-options .option-fps").show()
		$("#gallery .gallery-title").text("Video options")		
		$("#gallery .video-options .begin-upload").text("Upload video")
		Gallery.setScale(4)
		Gallery.setFps(15)
	} else {
		alert("invalid file type: " + file.type)
		Gallery.onUploadFinished();
	}
}
Gallery.uploadFile = function(){
	var file = Gallery.file
	Gallery.videoCtx.clearRect(0, 0, Gallery.videoCanvas.width, Gallery.videoCanvas.height)
	$("#gallery .video-progress").text("0%")
	$("#gallery .video-target").show()
	$("#gallery .video-options").hide()
	$("#gallery .gallery-title").text("Uploading  ...")		
	if(file.type.match("^image\/.*$")){
		console.log("image upload")
		// $("#gallery .loading-overlay").show()
		var img = new Image()
		img.name = file.name
		var reader = new FileReader()
		reader.onload = function(e){
			img.src = e.target.result
		}
		img.onload = function(){
			Gallery.uploadImage(img)
		}
		reader.readAsDataURL(file)
	} else if(file.type.match("^video\/.*$")){
		console.log("video upload")
		var video = document.createElement("video")
		var videoSource = document.createElement("source")
		var frame_id = 0	
		video.addEventListener("loadeddata", function(e){
			console.log("video loadeddata")
			Gallery.uploadVideo(video)
		})
		const reader = new FileReader();
		reader.onload = function (e) {
			videoSource.setAttribute('src', e.target.result);
			video.appendChild(videoSource);
			console.log("begin video load")
			video.load();
		};		
		reader.onprogress = function (e) {
			console.log('video progress: ', Math.round((e.loaded * 100) / e.total));
		};		
		reader.readAsDataURL(file);
	} else {
		Gallery.onUploadFinished();
	}	
}
Gallery.extractDataUrl = function(canvas, format){
	if(format == "jpg"){
		var jpg_header = "data:image/jpeg;base64,"
		var base_64 = canvas.toDataURL('image/jpeg', 0.9).substring(jpg_header.length)
		return base_64
	}
	if(format == "png"){
		var png_header = "data:image/png;base64,"
		var base_64 = canvas.toDataURL().substring(png_header.length)
		return base_64
	}
	return null
}
Gallery.uploadImage = function(image){
	
	Gallery.generateOffscreenImage(image)
	
	Gallery.videoCanvas.width = window.innerWidth - 20
	Gallery.videoCanvas.height = Gallery.videoCanvas.width/2
	Gallery.videoCtx.drawImage(Gallery.offscreen.canvas, 0, 0, Gallery.videoCanvas.width, Gallery.videoCanvas.height)
	
	// JPG Single
	var dataUrl = Gallery.extractDataUrl(Gallery.offscreen.canvas, "jpg")	
	MatrixClient.sendMessage("upload_stream_begin", []);
	MatrixClient.sendMessage("upload_stream_meta_data", ["image", "jpg", 0.0, 1]);
	MatrixClient.sendMessage("upload_stream_frame_data", [dataUrl]);
	MatrixClient.sendMessage("upload_stream_end");
		
	// PNG Single
/*	var png_header = "data:image/png;base64,"
	var base_64 = canvas.toDataURL().substring(png_header.length)
	MatrixClient.sendMessage("gallery_upload_png", [base_64])  */
	
	// PNG Chunks
/*	var png_header = "data:image/png;base64,"
	var chunk_size = 8 * 1024 // 16KB
	var base_64 = canvas.toDataURL().substring(png_header.length)
	var chunks = chunkSubstr(base_64, chunk_size)
	
	MatrixClient.sendMessage("gallery_upload_png", ["begin"])
	for(var i in chunks){		
		MatrixClient.sendMessage("gallery_upload_png", ["chunk", chunks[i]])
	}
	MatrixClient.sendMessage("gallery_upload_png", ["end"])
	console.log(chunks) */
	
	Gallery.onUploadFinished()
	
}
Gallery.uploadVideo = function(video){
	Gallery.video_frame_id = -1
	Gallery.video_frame_duration = 1.0/Gallery.fps
	Gallery.video_numFrames = Math.floor(video.duration/Gallery.video_frame_duration)
	
	video.width = video.videoWidth
	video.height = video.videoHeight
	
	Gallery.videoCanvas.width = window.innerWidth - 20
	Gallery.videoCanvas.height = Gallery.videoCanvas.width/2
	
	MatrixClient.sendMessage("upload_stream_begin", []);
	MatrixClient.sendMessage("upload_stream_meta_data", ["video", "jpg", Gallery.fps, Gallery.video_numFrames, Gallery.playbackMode == "normal" ? 0 : 1]);
	
	Gallery.video = video;
	
	video.addEventListener("seeked", function(e){
		console.log("video seeked, frame_id = " + Gallery.video_frame_id)
		if(Gallery.video_frame_id == -1){
			Gallery.video_frame_id = 0
			video.currentTime = 0.0
		} else {
			// current frame
			Gallery.generateOffscreenImage(video)
			Gallery.videoCtx.clearRect(0, 0, Gallery.videoCanvas.width, Gallery.videoCanvas.height)
			Gallery.videoCtx.drawImage(Gallery.offscreen.canvas, 0, 0, Gallery.videoCanvas.width, Gallery.videoCanvas.height)
			var dataUrl = Gallery.extractDataUrl(Gallery.offscreen.canvas, "jpg")
			MatrixClient.sendMessage("upload_stream_frame_data", [dataUrl]);
		}
	})	
	video.currentTime = video.duration
}
Gallery.onWebsocketMessage = function(json){
	if(json.type == "frame-received"){
		$("#gallery .gallery-title").text("Uploading ... " + Math.round(Gallery.video_frame_id/Gallery.video_numFrames*100) + "%")
		// next frame?
		Gallery.video_frame_id++
		if(Gallery.video_frame_id < Gallery.video_numFrames){
			// next frame
			Gallery.video.currentTime = Gallery.video.duration*(1.0/Gallery.video_numFrames)*Gallery.video_frame_id
		} else {
			// done
			Gallery.onUploadFinished()
			MatrixClient.sendMessage("upload_stream_end");
			Gallery.video = null
		}		
	}
}
Gallery.onUploadFinished = function(){
	Gallery.restoreDefaultView()	
}
Gallery.generateOffscreenImage = function(image){
	if(!image){
		alert("no image")
		return false
	}	
	var canvas = Gallery.offscreen.canvas
	var ctx = Gallery.offscreen.ctx
	// ACHTUNG: resulting canvas dimensions have to be power of 2 (mipmaps) 
	var scaling_factor = Gallery.scale
	canvas.width = 64*scaling_factor
	canvas.height = 32*scaling_factor
	
	if(Gallery.cropMode == "stretch"){
		ctx.drawImage(image, 0, 0, canvas.width, canvas.height)		
	} else if(Gallery.cropMode == "fill"){
		var canvas_aspect = canvas.width/canvas.height
		var image_aspect = image.width/image.height
		var image_w = 0
		var image_h = 0
		var image_x = 0
		var image_y = 0
		if(image_aspect > canvas_aspect){
			image_h = canvas.height
			image_w = canvas.height*image_aspect
			image_x = -image_w/2 + canvas.width/2
			image_y = 0
		} else {
			image_w = canvas.width
			image_h = canvas.width/image_aspect
			image_x = 0
			image_y = -image_h/2 + canvas.height/2
		}
		ctx.drawImage(image, image_x, image_y, image_w, image_h)		
	} else {
		var canvas_aspect = canvas.width/canvas.height
		var image_aspect = image.width/image.height
		var image_w = 0
		var image_h = 0
		var image_x = 0
		var image_y = 0
		if(image_aspect < canvas_aspect){
			image_h = canvas.height
			image_w = canvas.height*image_aspect
			image_x = -image_w/2 + canvas.width/2
			image_y = 0
		} else {
			image_w = canvas.width
			image_h = canvas.width/image_aspect
			image_x = 0
			image_y = -image_h/2 + canvas.height/2
		}
		ctx.drawImage(image, image_x, image_y, image_w, image_h)		
	}
	
		
	return true
}
