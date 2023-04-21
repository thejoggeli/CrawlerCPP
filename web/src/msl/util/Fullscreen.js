import Log from "msl/log/Log.js"

function Fullscreen(){}
Fullscreen.timeouts = []
Fullscreen.createTimeouts = function(){}
Fullscreen.install = function(){
    Log.print(Log.Info, "Fullscreen", "install")
	$(window).on("resize", function(e){
		if($(".fullscreen:visible").length > 0){
			window.scrollTo(0, -1)
			var w = window.innerWidth;			
			var h = window.innerHeight
			$(".fullscreen").width(w)
			$(".fullscreen").height(h)
			window.scrollTo(0, 0)
		}
	})
	$(window).on("orientationchange", function(e){
		if($(".fullscreen:visible").length > 0){
			Fullscreen.refresh();			
		}
	});	 
}
Fullscreen.refresh = function(){
	$(window).trigger("resize");	
	// clear timeouts
	for(var i in Fullscreen.timeouts){
		clearTimeout(Fullscreen.timeouts[i]);		
	}
	Fullscreen.timeouts = []
	// create new timeouts	
	Fullscreen.timeouts.push(setTimeout(function(){
		$(window).trigger("resize");			
	}, 250))
	Fullscreen.timeouts.push(setTimeout(function(){
		$(window).trigger("resize");			
	}, 500))
	Fullscreen.timeouts.push(setTimeout(function(){
		$(window).trigger("resize");			
	}, 1000))
}

export default Fullscreen