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
  :DocumentRoot=>"./",
  :Port=>80 })

server.mount('/index.html','/spiffs/index.html')

server.mount_proc('/load.html') do |req,res|
  puts req[:method]
  aplist = "<p>SSID</p>"
  res[:body]= <<EOD
<html>
 <head>
  <meta charset="utf-8">
  <title>WiFi AP list</title>
 </head>
 <body>
  <p>WiFi AP list</p>
  <div>#{aplist}</div>
  <form action="/form.cgi" method="post">
    <div>
      <label for="apname">AP name</label>
      <input type="text" id="apname" name="apname">
    </div>
    <div>
      <label for="password">Password</label>
      <input type="text" id="pass" name="pass">
    </div>
  <input type="submit" value="Set">
  </form>
 </body>
</html>
EOD
end

server.run

)"