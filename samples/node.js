// run with node
var http = require("http")
http.createServer(function(req,res){
	res.writeHeader(200, {"Content-Type": "text/plain"})
	res.write("hello world");
	res.end();
}).listen(8080);
