local inspect = require "inspect"
-- init Driver
if driver then
  driver.onEvent(function(event)
    print("driver event:" .. inspect(event))
  end)
end

if settings then
  settings.onEvent(function(event)
    print("settings event:" .. inspect(event))
  end)
end

-- init Engine
if engine then
  local isTickEvent = function(event)
    local event_name = getmetatable(event).__name
    return event_name == "PreTickEvent" or event_name == "TickEvent" or event_name == "PostTickEvent"
  end

  engine.onEvent(function (event)
    if not isTickEvent(event) or log.isVerboseLevel(1) then
      print("engine event:" .. inspect(event))
    end
  end)
end

-- init Window
if window then
  window.onEvent(function(event)
    print("window event:" .. inspect(event))
  end)
end

-- init Keyboard
if keyboard then
  keyboard.onEvent(function(event)
    print("keyboard event:" .. inspect(event))
  end)
end

-- init World
if world then
  world.onEvent(function(event)
    print("world event:" .. inspect(event))
  end)
end

-- init Mouse
if mouse then
  local isMotionEvent = function(event)
    local event_name = getmetatable(event).__name
    return event_name == "MouseMotionEvent"
  end

  mouse.onEvent(function(event)
    if not isMotionEvent(event) or log.isVerboseLevel(1) then
      print("mouse event:" .. inspect(event))
    end
  end)
end

-- init Renderer
if renderer then
  local isFrameEvent = function(event)
    local event_name = getmetatable(event).__name
    return event_name == "PreFrameEvent" or event_name == "PostFrameEvent"
  end

  renderer.onEvent(function(event)
    if not isFrameEvent(event) or log.isVerboseLevel(1) then
      print("renderer event:" .. inspect(event))
    end
  end)
end