function CollisionDetection(){}
CollisionDetection.checkIntersection = function(c1,c2){
	return CollisionDetection.matrix[c1.COLLIDER_TYPE][c2.COLLIDER_TYPE](c1,c2);
}
CollisionDetection.matrix = {}
CollisionDetection.buildMatrix = function(){
	for(var i in ColliderType){
		CollisionDetection.matrix[ColliderType[i]] = {};
		for(var j in ColliderType){
			var min, max;
			if(i < j){
				min = i;
				max = j;
			} else {
				min = j;
				max = i;
			}
			var funcName = min+"_"+max;
			var func;
			if(CollisionDetection.checkFunctions[funcName] === undefined){
				func =	CollisionDetection.checkFunctions["NOT_IMPLEMENTED"];
			//	console.log(funcName + " collision function not found");
			} else {
				func = CollisionDetection.checkFunctions[funcName];
			}
			CollisionDetection.matrix[ColliderType[i]][ColliderType[j]] = func;
		}	
	}	
}
CollisionDetection.checkFunctions = {
	"NOT_IMPLEMENTED": function(){
		console.log("not implemented");
	},
	"AABB_AABB": function(c1,c2){
		if(c1.position.x > c2.position.x+c2.width) return false;
		if(c1.position.y > c2.position.y+c2.height) return false;
		if(c1.position.x+c1.width < c2.position.x) return false;
		if(c1.position.y+c1.height < c2.position.y) return false;
		return true;
	},
	"AABB_POINT": function(c1,c2){
		
	},
	"POINT_POINT": function(c1,c2){
		if(c1.position.x != c2.position.x) return false;
		if(c1.position.y != c2.position.y) return false;
		return true;		
	},
	"CIRCLE_CIRCLE": function(c1,c2){
		var xx = c1.position.x - c2.position.x;
		var yy = c1.position.y - c2.position.y;
		var radius = c1.radius + c2.radius;
		radius *= radius;
		var distance = xx*xx+yy*yy;
		return distance <= radius;
	},
	"CIRCLE_POINT": function(c1,c2){
		
	},
};
CollisionDetection.isPointInsidePolygon = function(px, py, points){
	var j = points.length-1;
	var c = 0;
	for(var i = 0; i < points.length; j=i++){
		if(((points[i][1]>py) != (points[j][1]>py)) && 
			(px < (points[j][0]-points[i][0]) * (py-points[i][1]) / (points[j][1]-points[i][1]) + points[i][0]))
				c = c == 0 ? 1 : 0;
	} 
	return c == 0 ? false : true;
}
CollisionDetection.isPointInsideRectangle = function(px, py, rx, ry, rw, rh){
	if(px < rx-rw/2) return false;
	if(px > rx+rw/2) return false;
	if(py < ry-rh/2) return false;
	if(py > ry+rh/2) return false;
	return true;
}
CollisionDetection.isPointInsideCircle = function(px, py, cx, cy, cr){
	var x = px-cx;
	var y = py-cy;
	return x*x+y*y < cr*cr;
}

export default CollisionDetection