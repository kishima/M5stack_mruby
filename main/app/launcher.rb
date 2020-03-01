R"( #"

include M5

wifi = ESP32::WiFi.new

wifi.on_connected do |ip|
  puts "Connected: #{ip}"
end

wifi.on_disconnected do
  puts "Disconnected"
end

puts "Connecting to wifi"
wifi.connect('BCW710J-B0B1A-G', 'a48f3888cff3a')

#
# Loop forever otherwise the script ends
#
while true do
  mem = ESP32::System.available_memory() / 1000
  puts "Free heap: #{mem}K"
  wifi.status
  ESP32::System.delay(10000)
end



)"