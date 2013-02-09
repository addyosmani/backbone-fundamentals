var connect = require('./');

var server = connect();

server.use(connect.static(__dirname));
server.use(connect.bodyParser());
server.use(function(req, res){
  res.end(JSON.stringify(req.body));
});

server.listen(3001);