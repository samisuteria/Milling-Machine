var Path = require('path');
var Hapi = require('hapi');
var Good = require('good');
var serialport = require("serialport"); 
var SerialPort = serialport.SerialPort; //Object Constructor

var serverOptions = {
	views: {
		engines: {
			'html': require('handlebars')
		},
		path: Path.join(__dirname, 'templates'),
		isCached: false
	}
};

var server = new Hapi.Server("localhost",9001, serverOptions);

server.route({
	method: 'GET',
	path: '/',
	handler: {
		view: {
			template: 'index',
			context: {
				title: 'Milling Machine'
			}
		}
	}
});

server.route({
	method: 'GET',
	path: '/inc/{path*}',
	handler: {
		directory: {
			path: './inc',
			listing: true,
			index: true
		}
	}
});

server.route({
	method: 'GET',
	path: '/serialport/list',
	handler: function(request, reply){
		serialport.list(function (err, ports) {
			if(err){
				reply("Error");
			} else {
				reply(ports);
			}
		});

	}
});

server.route({
	method: 'GET',
	path: '/serialport/connect/{devicename}',
	handler: function(request, reply){
		setupMicrocontroller(request.params.devicename);
		reply("Connected");	
	}
});

server.route({
	method: 'POST',
	path: '/serialport/write',
	handler: function(request, reply){
		if(microcontroller){
			microcontroller.write(request.payload.toWrite + '\n', function(){
				microcontroller.drain(function(){
					reply("Write");
				});
			});
		}
	}
});


server.pack.register(Good, function(err){
	if(err){
		throw err; //Good didn't load
	}
	server.start(function(){
		server.log('info', 'Server running at: ' + server.info.uri);
	});

});

/* //less outputs
server.start(function(){
		server.log('info', 'Server running at: ' + server.info.uri);
});
*/

var io = require('socket.io')(server.listener);

io.on('connection', function(socket){
	console.log('Client Connected');
	socket.on('disconnect', function(){
		console.log('Client Disconnected');
		if(microcontroller){
			microcontroller.close(function(){
			console.log("Microcontroller Disconnected");
			});
		}
	});
});

var microcontroller;

function setupMicrocontroller(name) {

	microcontroller = new SerialPort("/dev/" + name, {
		baudrate: 9600,
		parser: serialport.parsers.readline("\n")
	});

	microcontroller.on('open', function(){
		io.emit('Microcontroller Status', 'Microcontroller Connected');

		microcontroller.on('data', function(data){
			if(data.lastIndexOf("<ENDMOVE>",0) === 0){
				io.emit('CNC Movement', 'ENDMOVE');
			} else if(data.lastIndexOf("<STARTMOVE>",0) === 0){
				io.emit('CNC Movement', 'STARTMOVE');
			} else {
				io.emit('Microcontroller Data', data);
			}
			
		});
	});

	microcontroller.on('close', function(){
		io.emit('Microcontroller Status', 'Microcontroller Disconnected');
	});
}


