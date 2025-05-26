Viewport = {}

OR, XOR, AND = 1, 3, 4

function bitoper(a, b, oper)
   local r, m, s = 0, 2^31
   repeat
      s,a,b = a+b+m, a%m, b%m
      r,m = r + m*oper%(s-a-b), m/2
   until m < 1
   return r
end

function Viewport:init(o)
  print("viewport gui init")
  keyboard.onPressed(32, function (event)
    if event.mods == 1 then
      print("space pressed")
    end
  end)
end