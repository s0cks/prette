local old_print = print
print = function(value)
  if type(value) ~= "string" then
    Prette.log(INFO, tostring(value))
  else
    Prette.log(INFO, value)
  end
end

print(Prette)
if Prette.isDebug() then
  print("booting....")

  -- init Driver
  Driver.onEvent(print)


  -- init Engine
  local isTickEvent = function(event)
    local event_name = getmetatable(event).__name
    return event_name == "PreTick" or event_name == "Tick" or event_name == "PostTick"
  end

  Engine.onEvent(function (event)
    if not isTickEvent(event) then
      print(event)
    end
  end)

  -- init Window
  Window.onEvent(print)

  -- init Keyboard
  Keyboard.onEvent(print)

  local isMotionEvent = function(event)
    local event_name = getmetatable(event).__name
    return event_name == "MouseMotion"
  end

  -- init Mouse
  Mouse.onEvent(function(event)
    if not isMotionEvent(event) then
      print(event)
    end
  end)

  -- init Renderer
  local isFrameEvent = function(event)
    local event_name = getmetatable(event).__name
    return event_name == "PreFrame" or event_name == "PostFrame"
  end

  Renderer.onEvent(function(event)
    if not isFrameEvent(event) then
      print(event)
    end
  end)
end