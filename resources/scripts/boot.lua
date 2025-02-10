local old_print = print
print = function(value)
  if type(value) ~= "string" then
    Prette.log(INFO, tostring(value))
  else
    Prette.log(INFO, value)
  end
end

local isTickEvent = function(event)
  local event_name = getmetatable(event).__name
  return event_name == "PreTick" or event_name == "Tick" or event_name == "PostTick"
end

if Prette.isDebug() then
  print("prette v" .. Prette.getVersion() .. " booting....")
  Driver.onEvent(print)
  Engine.onEvent(function (event)
    if not isTickEvent(event) then
      print(event)
    end
  end)
  Window.onEvent(print)
  Keyboard.onEvent(print)
  Renderer.onEvent(print)
end