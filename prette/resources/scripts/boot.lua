local inspect = require "inspect"

print(runtime)
if runtime.isDebug() then
  print("booting....")

  if settings then
    print("resolution:" .. inspect(settings.getResolution("resolution")))
  end

  if world then
    world.onEvent(function(event)
      print("world event: " .. inspect(event))
    end)
  end
end