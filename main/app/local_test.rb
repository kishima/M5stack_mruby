R"( #"

include M5
include ESP32

wifi = WiFi.new

connect = false
wifi.on_connected do
  puts "Connected:"
  connect = true
end

wifi.on_disconnected do
  puts "Disconnected"
end

puts "Connecting to wifi"
#wifi.connect('SSID', 'PASSWORD')
wifi.connect()

loop do
  System.delay(1000)
  break if connect
end

Storage::mount_spiffs('/spiffs')

server = TinyHttpServer.new({
# :DocumentRoot=>"./",
  :Port=>80 })

server.mount('/index.html','/spiffs/index.html')

server.mount_proc('/load.html') do |req,res|
  puts req[:method]
  list = "<p>TEST</p>"
  res[:body]= <<EOD
<html>
 <head>
  <meta charset="utf-8">
  <title>TEST</title>
 </head>
 <body>
  <div>#{list}</div>
 </body>
</html>
EOD
end

server.run

)"